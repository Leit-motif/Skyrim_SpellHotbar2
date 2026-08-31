#!/usr/bin/env python3
"""Build the distributable archive for this fork of Spell Hotbar 2.

Ticket 59 (`.scratch/mco-integration/issues/59-...`). Our mod is an OVERWRITE over base
Spell Hotbar 2: the base mod is a hard requirement, ours installs after it and wins the
conflict. This script assembles only the files that are ours, proves that no
upstream-untouched asset rode along, and writes a reviewable manifest beside the archive.

Four rules do the real work here:

1.  **Only git-tracked files ship.** Trees are enumerated through `git ls-files`, so a
    local scratch file, a playtest animation drop, or anything else `.gitignore` covers
    cannot leak into a release. The two build outputs (the DLL, the two `.pex`) are the
    named exceptions.
2.  **Every member is classified against the installed base mod, byte for byte.**
    Absent from base -> ADDITION. Present and different -> OVERWRITE. Present and
    identical -> REDUNDANT, which fails the build: a byte-identical file is by definition
    an upstream-untouched asset and belongs to the base install, not to us.
3.  **The overwrite set is declared, not discovered.** `REQUIRED_OVERWRITES` below is the
    list of upstream files we replace, and `ALLOWED_OVERWRITES` the superset that may. If
    the classification disagrees with either the build fails, so the overwrite ruling stays
    checkable instead of drifting.
4.  **Neither a real name nor a stale compile gets out.** Every packaged byte is scanned
    for committer names in ASCII and UTF-16, and the `.pex` headers are read for their own
    compilation timestamp and checked against the `.psc` on disk.

Usage:
    python python_scripts/build_mod_release.py                 # build + verify
    python python_scripts/build_mod_release.py --check         # verify only, no archive
    python python_scripts/build_mod_release.py --refresh-pex   # re-import compiled scripts
    python python_scripts/build_mod_release.py --version 0.2.0 --name "Some Name"
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import struct
import subprocess
import sys
import zipfile
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path, PurePosixPath

PROJECT_ROOT = Path(__file__).resolve().parent.parent
CONFIG_PATH = PROJECT_ROOT / "deploy" / "release" / "release.json"
README_TEMPLATE = PROJECT_ROOT / "deploy" / "release" / "README.template.md"
OUTPUT_DIR = PROJECT_ROOT / "build"

DLL_SOURCE = PROJECT_ROOT / "skse_plugin" / "build" / "release" / "SpellHotbar2.dll"
PEX_DIR = PROJECT_ROOT / "papyrus" / "Scripts"
PEX_RECORD = PEX_DIR / "compiled.json"

# The compiled scripts we ship, and the source each was compiled from.
PEX_SOURCES = {
    "SpellHotbar.pex": "papyrus/Scripts/Source/SpellHotbar.psc",
    "SpellHotbarMCM.pex": "papyrus/Scripts/Source/SpellHotbarMCM.psc",
}

# Archive paths that replace a file the base mod installs. Everything else must be new.
# Keep these in sync deliberately; the build fails rather than letting them drift.
#
# REQUIRED must all classify as overwrites: if one does not, either the list is stale or the
# configured base install is not the mod it claims to be. ALLOWED is the superset that may.
# The two `.psc` are in ALLOWED and not REQUIRED because upstream's own packer ships
# `Scripts/Source/*.psc` while the FOMOD does not install them, so whether ours overwrite
# anything depends on how the user installed the base mod.
REQUIRED_OVERWRITES = {
    "SKSE/Plugins/SpellHotbar2.dll",
    "Scripts/SpellHotbar.pex",
    "Scripts/SpellHotbarMCM.pex",
}
ALLOWED_OVERWRITES = REQUIRED_OVERWRITES | {
    "Scripts/Source/SpellHotbar.psc",
    "Scripts/Source/SpellHotbarMCM.psc",
}

# Files under a packaged tree that stay out, and why. The reason is printed, so an
# exclusion cannot quietly become folklore.
EXCLUSIONS = {
    "data/SKSE/Plugins/SpellHotbar/localization/translation.txt": (
        "Base's English translation with one typo fixed ('Globald Cooldown'). Shipping it "
        "would force English onto every user, because the base FOMOD installs the chosen "
        "language under this one filename. The DLL's compiled default already carries the "
        "corrected string, and it also carries the keys this file lacks."
    ),
}


@dataclass
class Member:
    """One file in the archive."""

    source: Path
    arcname: str
    sha256: str
    size: int
    classification: str = ""


@dataclass
class BuildResult:
    members: list[Member] = field(default_factory=list)
    archive: Path | None = None
    manifest: Path | None = None


# --------------------------------------------------------------------------- helpers


def sha256_of(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def sha256_of_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def git(*args: str) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
        check=True,
    )
    return result.stdout


def tracked_files(prefix: str) -> list[str]:
    """Repo-relative paths tracked by git under `prefix`, posix separators.

    `-z` matters: without it git quotes any path with a non-ASCII or control character and
    the quoted string is not a real filesystem path. NUL-separated output is verbatim.
    """
    out = git("ls-files", "-z", "--", prefix)
    return sorted(part for part in out.split("\0") if part)


def load_config() -> dict:
    if not CONFIG_PATH.exists():
        fail(f"missing config: {CONFIG_PATH.relative_to(PROJECT_ROOT)}")
    return json.loads(CONFIG_PATH.read_text(encoding="utf-8"))


def fail(message: str) -> None:
    print(f"\nBUILD FAILED: {message}", file=sys.stderr)
    sys.exit(1)


def forbidden_strings(config: dict) -> list[str]:
    """Real-name guard list, derived from git rather than written into the repo.

    Nemesis prints `author=` in its own mod list, and that is exactly where a real name
    leaked in the sibling repo's release. Committer names come from git history, so the
    guard needs no hard-coded name and covers whoever commits next.

    Whole names are not enough. The leak that actually shipped in the compiled `.pex` was
    `DESKTOP-AMRIT`, a machine name that contains one word of the committer's name and
    matches no full string, so every word of four characters or more is a needle too.
    """
    names: set[str] = set()
    try:
        names.add(git("config", "user.name").strip())
    except subprocess.CalledProcessError:
        pass
    # Only our own commits. `--all` alone would pull in upstream's authors, and upstream's
    # handle is a public credit that belongs in the README rather than a leak.
    log_args = ["log", "--format=%an%n%cn", "--all"]
    try:
        git("rev-parse", "--verify", "--quiet", "upstream/master")
        log_args += ["--not", "upstream/master"]
    except subprocess.CalledProcessError:
        pass
    try:
        for line in git(*log_args).splitlines():
            if line.strip():
                names.add(line.strip())
    except subprocess.CalledProcessError:
        pass

    needles: set[str] = set()
    for name in names:
        needles.add(name)
        needles.update(part for part in re.split(r"[^A-Za-z0-9]+", name) if len(part) >= 4)
    needles.update(config.get("extra_forbidden_strings", []))

    allowed: set[str] = set()
    for public in [config.get("author_public", ""), *config.get("public_identities", [])]:
        if not public:
            continue
        allowed.add(public.lower())
        allowed.update(part.lower() for part in re.split(r"[^A-Za-z0-9]+", public) if part)
    return sorted(n for n in needles if len(n) >= 4 and n.lower() not in allowed)


# ------------------------------------------------------------------- compiled scripts

PEX_MAGIC = 0xFA57C0DE


def read_pex_header(path: Path) -> dict:
    """Parse a Skyrim `.pex` header: compile time, source name, user, machine.

    The header is big-endian and carries three length-prefixed strings after the timestamp.
    Two of them are why this function exists: the compiler stamps the account and machine
    that built the file, and both `.pex` in this repo shipped `DESKTOP-AMRIT` until the
    import step started blanking them.
    """
    data = path.read_bytes()
    magic, = struct.unpack_from(">I", data, 0)
    if magic != PEX_MAGIC:
        fail(f"{path.name} is not a Papyrus .pex (magic {magic:#010x})")
    offset = 16  # magic(4) + major(1) + minor(1) + gameID(2) + compilationTime(8)
    compiled_at, = struct.unpack_from(">Q", data, 8)
    strings = []
    for _ in range(3):
        length, = struct.unpack_from(">H", data, offset)
        offset += 2
        strings.append(data[offset:offset + length].decode("utf-8", "replace"))
        offset += length
    source, user, machine = strings
    return {
        "compiled_at": compiled_at,
        "source": source,
        "user": user,
        "machine": machine,
        "strings_end": offset,
    }


def anonymize_pex(path: Path) -> bool:
    """Blank the user and machine names in a `.pex` header. Returns True if it changed.

    The `.ppj` sets `Anonymize="true"` and it plainly did not take, so this does the same
    job at import time. Only the two header strings change; the bytecode after them is
    copied verbatim, and nothing in the game reads either field.
    """
    header = read_pex_header(path)
    if not header["user"] and not header["machine"]:
        return False
    data = path.read_bytes()
    source = header["source"].encode("utf-8")
    rebuilt = (
        data[:16]
        + struct.pack(">H", len(source)) + source
        + struct.pack(">H", 0)
        + struct.pack(">H", 0)
        + data[header["strings_end"]:]
    )
    path.write_bytes(rebuilt)
    return True


def refresh_pex(config: dict) -> None:
    """Import the compiled `.pex` from the Papyrus compiler's output directory.

    There is no Papyrus compiler on this machine (Nolvus ships no Creation Kit), so the
    build cannot compile. It imports instead, and records the `.psc` hash each `.pex` was
    compiled from so `verify_pex_currency` can catch a stale import later.
    """
    source_dir = Path(config["compiled_scripts_dir"])
    if not source_dir.is_dir():
        fail(f"compiled_scripts_dir does not exist: {source_dir}")

    PEX_DIR.mkdir(parents=True, exist_ok=True)
    record: dict[str, dict[str, str]] = {}

    for pex_name, psc_rel in PEX_SOURCES.items():
        src = source_dir / pex_name
        if not src.is_file():
            fail(f"compiled script not found: {src}")
        dst = PEX_DIR / pex_name
        shutil.copy2(src, dst)
        if anonymize_pex(dst):
            print(f"  anonymized {pex_name} (blanked the compiler's user and machine names)")
        header = read_pex_header(dst)
        psc = PROJECT_ROOT / psc_rel
        if not psc.is_file():
            fail(f"Papyrus source missing: {psc_rel}")
        record[pex_name] = {
            "psc": psc_rel,
            "psc_sha256": sha256_of(psc),
            "psc_mtime": psc.stat().st_mtime,
            "pex_sha256": sha256_of(dst),
            "pex_compiled_at": header["compiled_at"],
            "pex_compiled_utc": datetime.fromtimestamp(
                header["compiled_at"], timezone.utc
            ).isoformat(timespec="seconds"),
            "imported_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        }
        if psc.stat().st_mtime > header["compiled_at"]:
            fail(
                f"{psc_rel} was modified after {pex_name} was compiled "
                f"({record[pex_name]['pex_compiled_utc']}). Recompile before importing."
            )
        print(f"  imported {pex_name} <- {src.name}, compiled "
              f"{record[pex_name]['pex_compiled_utc']}")

    PEX_RECORD.write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    print(f"  wrote {PEX_RECORD.relative_to(PROJECT_ROOT)}")


def verify_pex_currency() -> None:
    """Fail if a `.pex` is stale, anonymous no longer, or not from the source it claims.

    The recorded `.psc` hash alone only catches "changed since the last import", which a
    careless `--refresh-pex` resets. The `.pex` header's own compilation timestamp is the
    check that survives that: if the source is newer than the compile, the bytecode is
    stale whatever the record says.
    """
    if not PEX_RECORD.exists():
        fail(
            f"{PEX_RECORD.relative_to(PROJECT_ROOT)} is missing. "
            "Run with --refresh-pex after compiling the Papyrus scripts."
        )
    record = json.loads(PEX_RECORD.read_text(encoding="utf-8"))

    for pex_name, psc_rel in PEX_SOURCES.items():
        entry = record.get(pex_name)
        if entry is None:
            fail(f"{pex_name} has no entry in {PEX_RECORD.name}; run --refresh-pex")
        pex = PEX_DIR / pex_name
        if not pex.is_file():
            fail(f"missing compiled script: {pex}")
        if sha256_of(pex) != entry["pex_sha256"]:
            fail(f"{pex_name} changed since it was recorded; run --refresh-pex")
        psc = PROJECT_ROOT / psc_rel
        current = sha256_of(psc)
        if current != entry["psc_sha256"]:
            fail(
                f"{psc_rel} has changed since {pex_name} was compiled "
                f"(recorded {entry['psc_sha256'][:12]}, now {current[:12]}). "
                "Recompile the Papyrus scripts, then run --refresh-pex."
            )

        header = read_pex_header(pex)
        if header["user"] or header["machine"]:
            fail(
                f"{pex_name} still carries the compiler's identity "
                f"(user={header['user']!r}, machine={header['machine']!r}). "
                "Run --refresh-pex, which blanks both."
            )
        if header["source"] != Path(psc_rel).name:
            fail(
                f"{pex_name} was compiled from {header['source']!r}, not "
                f"{Path(psc_rel).name!r}"
            )
        if psc.stat().st_mtime > header["compiled_at"]:
            compiled = datetime.fromtimestamp(header["compiled_at"], timezone.utc)
            fail(
                f"{psc_rel} is newer than the bytecode in {pex_name} "
                f"(compiled {compiled.isoformat(timespec='seconds')}). Recompile, then "
                "run --refresh-pex."
            )
        print(f"  {pex_name} matches {psc_rel}, anonymous, compiled "
              f"{datetime.fromtimestamp(header['compiled_at'], timezone.utc):%Y-%m-%d %H:%M UTC}")


# ----------------------------------------------------------------------- the manifest


def collect_members() -> list[Member]:
    """Enumerate every file that goes into the archive, with its archive path."""
    members: list[Member] = []
    seen: set[str] = set()

    def add(source: Path, arcname: str) -> None:
        if arcname in seen:
            fail(f"two sources map to the same archive path: {arcname}")
        if source.is_symlink():
            # zipfile follows the link and packs the target's bytes, so a link pointing into
            # the base install would ship upstream content under one of our paths.
            fail(f"packaged source is a symlink: {source}")
        if not source.is_file():
            fail(f"source file missing: {source}")
        seen.add(arcname)
        members.append(
            Member(
                source=source,
                arcname=arcname,
                sha256=sha256_of(source),
                size=source.stat().st_size,
            )
        )

    # 1. Our built DLL.
    if not DLL_SOURCE.is_file():
        fail(
            f"{DLL_SOURCE.relative_to(PROJECT_ROOT)} not found. "
            "Build the SKSE plugin first (skse_plugin/build-release.bat)."
        )
    add(DLL_SOURCE, "SKSE/Plugins/SpellHotbar2.dll")

    # 2. The two compiled scripts, plus the sources they were compiled from.
    for pex_name, psc_rel in PEX_SOURCES.items():
        add(PEX_DIR / pex_name, f"Scripts/{pex_name}")
        add(PROJECT_ROOT / psc_rel, f"Scripts/Source/{Path(psc_rel).name}")

    # 3. The Nemesis patch (shtb) and the behaviour-data injector config.
    #    `nemesis/` in the repo is already the install layout, so it maps to the root.
    for rel in tracked_files("nemesis"):
        arc = str(PurePosixPath(rel).relative_to("nemesis"))
        add(PROJECT_ROOT / rel, arc)

    # 4. Our runtime data: OAR submods, Ability catalogues, Weapon Art icon atlas.
    #    `data/` also maps to the install root.
    for rel in tracked_files("data"):
        if rel in EXCLUSIONS:
            continue
        arc = str(PurePosixPath(rel).relative_to("data"))
        add(PROJECT_ROOT / rel, arc)

    return members


def verify_base_install(base_root: Path, base_config: dict) -> None:
    """Check the configured base install is the version the package pins itself to.

    Classification is only worth as much as the tree it compares against, and `is_dir()`
    proves nothing. MO2 records the installed version and the archive it came from in
    `meta.ini`, so read it.
    """
    meta = base_root / "meta.ini"
    pinned = base_config["supported_version"]
    if not meta.is_file():
        fail(
            f"{base_root} has no meta.ini, so the base mod's version cannot be confirmed "
            f"against the pinned {pinned}."
        )
    text = meta.read_text(encoding="utf-8", errors="ignore")
    installed = ""
    for line in text.splitlines():
        if line.startswith("version="):
            installed = line.split("=", 1)[1].strip()
            break
    # MO2 writes 0.0.14 as `0.0.14.0`; compare on the pinned prefix.
    if not installed.startswith(pinned):
        fail(
            f"base mod at {base_root} reports version {installed!r}, but the package pins "
            f"{pinned!r}. Fix base_mod.supported_version or install the pinned base."
        )
    print(f"  meta.ini reports {installed}, matching the pinned {pinned}")


def index_base_by_content(base_root: Path) -> dict[str, str]:
    """SHA-256 -> a representative path, over every file the base mod installs.

    Path-keyed comparison alone would let a base asset ship under a new name. This index
    catches it by content, which is what "upstream-untouched" actually means.
    """
    index: dict[str, str] = {}
    for path in base_root.rglob("*"):
        if path.is_file():
            index.setdefault(sha256_of(path), str(path.relative_to(base_root)).replace("\\", "/"))
    return index


def classify_against_base(
    members: list[Member], base_root: Path, content_index: dict[str, str]
) -> dict[str, int]:
    """Tag each member ADDITION / OVERWRITE / REDUNDANT against the installed base mod.

    REDUNDANT covers both shapes: identical bytes at the same path, and identical bytes
    anywhere else in the base tree.
    """
    counts = {"ADDITION": 0, "OVERWRITE": 0, "REDUNDANT": 0}
    for member in members:
        base_file = base_root / member.arcname
        if base_file.is_file() and sha256_of(base_file) == member.sha256:
            member.classification = "REDUNDANT"
        elif member.sha256 in content_index:
            member.classification = "REDUNDANT"
        elif base_file.is_file():
            member.classification = "OVERWRITE"
        else:
            member.classification = "ADDITION"
        counts[member.classification] += 1
    return counts


def check_classification(members: list[Member]) -> None:
    redundant = [m.arcname for m in members if m.classification == "REDUNDANT"]
    if redundant:
        fail(
            "these files are byte-identical to a file the base mod installs and must not "
            "ship (upstream-untouched assets come from the base install):\n  "
            + "\n  ".join(redundant)
        )

    found = {m.arcname for m in members if m.classification == "OVERWRITE"}
    unexpected = sorted(found - ALLOWED_OVERWRITES)
    missing = sorted(REQUIRED_OVERWRITES - found)
    if unexpected:
        fail(
            "these files overwrite a base-mod file but are not in ALLOWED_OVERWRITES:\n  "
            + "\n  ".join(unexpected)
        )
    if missing:
        fail(
            "REQUIRED_OVERWRITES names files that do not actually overwrite the base "
            "mod (stale list, or the base install changed):\n  " + "\n  ".join(missing)
        )


def check_nemesis_line_endings(members: list[Member]) -> None:
    """Nemesis patch files are CRLF, and the repo relies on `core.autocrlf` to restore that.

    A checkout with `autocrlf=false` (another machine, a CI runner) would hand this build LF
    files and ship them. Nothing downstream would say so, hence the check here rather than a
    note in a document.
    """
    offenders = []
    for member in members:
        if not member.arcname.startswith("Nemesis_Engine/"):
            continue
        data = member.source.read_bytes()
        crlf = data.count(b"\r\n")
        # Every LF must be part of a CRLF, every CR must be too, and there has to be at
        # least one line break. A file that is CR-only, or has no break at all, is not CRLF
        # however the counts happen to line up.
        if crlf == 0 or data.count(b"\n") != crlf or data.count(b"\r") != crlf:
            offenders.append(member.arcname)
    if offenders:
        fail(
            "Nemesis patch files are not CRLF in the working tree "
            f"({len(offenders)} of them, e.g. {offenders[0]}). Check `git config core.autocrlf`."
        )
    print(f"  {sum(1 for m in members if m.arcname.startswith('Nemesis_Engine/'))} "
          "Nemesis files are CRLF")


def check_forbidden_strings(members: list[Member], config: dict, readme: str) -> None:
    """Scan every packaged byte for a real name, text and binary alike.

    Nemesis shows `author=` from `info.ini` to every user, and the Papyrus compiler stamps
    its account and machine into the `.pex` header. A suffix allowlist would have missed
    the second one, so this reads whole files and matches case-insensitively in both ASCII
    and UTF-16, which is how a name appears in a Windows binary.
    """
    needles = forbidden_strings(config)
    if not needles:
        fail(
            "the real-name guard resolved no committer names, so it would pass everything. "
            "Check that git works here, or pin names in extra_forbidden_strings."
        )

    encoded = [
        (needle, needle.lower().encode("utf-8"), needle.lower().encode("utf-16-le"))
        for needle in needles
    ]
    hits: list[str] = []

    def scan(label: str, data: bytes) -> None:
        low = data.lower()
        for needle, ascii_form, utf16_form in encoded:
            if ascii_form in low or utf16_form in low:
                hits.append(f"{label}: contains {needle!r}")

    for member in members:
        scan(member.arcname, member.source.read_bytes())
    scan("README.md", readme.encode("utf-8"))

    if hits:
        fail("real name found in packaged files:\n  " + "\n  ".join(hits))
    print(f"  {len(members) + 1} files clean against {len(needles)} needle(s): "
          f"{', '.join(needles)}")


# ------------------------------------------------------------------------- the output


def display_path(path: Path) -> str:
    """Repo-relative when it can be, absolute when `--out` points elsewhere."""
    try:
        return str(path.resolve().relative_to(PROJECT_ROOT))
    except ValueError:
        return str(path)


def archive_stem(config: dict, name: str, version: str) -> str:
    stem = f"{name} {version}".replace("  ", " ")
    if not config.get("identity_frozen", False):
        stem += "-provisional"
    return stem


def render_readme(config: dict, name: str, version: str, dll_sha: str) -> str:
    if not README_TEMPLATE.exists():
        fail(f"missing README template: {README_TEMPLATE}")
    base = config["base_mod"]
    text = README_TEMPLATE.read_text(encoding="utf-8")
    substitutions = {
        "{{name}}": name,
        "{{version}}": version,
        "{{base_name}}": base["name"],
        "{{base_version}}": base["supported_version"],
        "{{upstream_tag}}": base["upstream_tag"],
        "{{upstream_commit_short}}": base["upstream_commit"][:7],
        "{{dll_sha256}}": dll_sha,
        "{{built_utc}}": datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M UTC"),
        "{{author_public}}": config["author_public"],
    }
    for key, value in substitutions.items():
        text = text.replace(key, value)
    leftover = [line for line in text.splitlines() if "{{" in line]
    if leftover:
        fail("README template has unsubstituted placeholders:\n  " + "\n  ".join(leftover))
    return text


def write_archive(members: list[Member], readme: str, out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    if out_path.exists():
        out_path.unlink()
    with zipfile.ZipFile(out_path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        for member in members:
            zf.write(member.source, arcname=member.arcname)
        zf.writestr("README.md", readme)


def verify_archive(archive: Path, members: list[Member], readme: str, dll_sha: str) -> None:
    """Reopen the archive and check it byte for byte. A build report is not evidence."""
    with zipfile.ZipFile(archive) as zf:
        listed = zf.namelist()
        names = set(listed)
        if len(listed) != len(names):
            duplicates = sorted({n for n in listed if listed.count(n) > 1})
            fail("archive has duplicate entries:\n  " + "\n  ".join(duplicates))
        expected = {m.arcname for m in members} | {"README.md"}

        surplus = sorted(names - expected)
        absent = sorted(expected - names)
        if surplus:
            fail("archive holds files the manifest does not:\n  " + "\n  ".join(surplus))
        if absent:
            fail("archive is missing manifest files:\n  " + "\n  ".join(absent))

        for member in members:
            data = zf.read(member.arcname)
            if sha256_of_bytes(data) != member.sha256:
                fail(f"archive content differs from source: {member.arcname}")

        if zf.read("README.md").decode("utf-8") != readme:
            fail("archive README.md does not match the rendered text")

        # Acceptance: the archive byte-contains the one shtb patch and no leftover shcr tree.
        prefix = "Nemesis_Engine/mod/shtb/"
        patch_files = [n for n in names if n.startswith(prefix)]
        source_count = len(tracked_files("nemesis/Nemesis_Engine/mod/shtb"))
        if len(patch_files) != source_count:
            fail(
                f"shtb: archive has {len(patch_files)} files, working tree has "
                f"{source_count}"
            )
        if f"{prefix}info.ini" not in names:
            fail("shtb: info.ini is missing from the archive")
        extra_codes = sorted({
            n.split("/")[2]
            for n in names
            if n.startswith("Nemesis_Engine/mod/") and n.count("/") >= 2
        } - {"shtb"})
        if extra_codes:
            fail(f"archive ships extra Nemesis mod codes: {extra_codes}")
        print(f"  shtb: {len(patch_files)} patch files, info.ini present")

        # Acceptance: both .pex are present.
        for pex_name in PEX_SOURCES:
            if f"Scripts/{pex_name}" not in names:
                fail(f"missing compiled script in archive: Scripts/{pex_name}")
        print(f"  compiled scripts: {', '.join(sorted(PEX_SOURCES))}")

        # Acceptance: the DLL in the archive is the current build. Re-read the build output
        # now rather than trusting the hash taken when members were collected.
        current_dll_sha = sha256_of(DLL_SOURCE)
        archived_dll_sha = sha256_of_bytes(zf.read("SKSE/Plugins/SpellHotbar2.dll"))
        if archived_dll_sha != current_dll_sha or current_dll_sha != dll_sha:
            fail(
                "archived DLL does not match skse_plugin/build/release/SpellHotbar2.dll "
                f"(archive {archived_dll_sha[:16]}, on disk {current_dll_sha[:16]})"
            )
        print(f"  DLL matches the current build ({current_dll_sha[:16]})")


def write_manifest(
    manifest_path: Path,
    members: list[Member],
    config: dict,
    name: str,
    version: str,
    archive: Path | None,
    counts: dict[str, int],
) -> None:
    payload = {
        "public_name": name,
        "version": version,
        "identity_frozen": config.get("identity_frozen", False),
        "built_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "repo_commit": git("rev-parse", "HEAD").strip(),
        "repo_dirty": bool(git("status", "--porcelain").strip()),
        "base_mod": config["base_mod"],
        "counts": counts,
        "exclusions": EXCLUSIONS,
        "archive": archive.name if archive else None,
        "archive_sha256": sha256_of(archive) if archive else None,
        "files": [
            {
                "arcname": m.arcname,
                "source": str(m.source.relative_to(PROJECT_ROOT)).replace("\\", "/"),
                "classification": m.classification,
                "size": m.size,
                "sha256": m.sha256,
            }
            for m in members
        ],
    }
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


# -------------------------------------------------------------------------------- cli


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--check",
        action="store_true",
        help="run every pre-archive check and write the manifest, but no zip. This does "
        "not inspect an existing archive; only a full build verifies one.",
    )
    parser.add_argument(
        "--refresh-pex",
        action="store_true",
        help="re-import the compiled .pex from the Papyrus output directory, then exit",
    )
    parser.add_argument("--version", dest="version", help="override the package version")
    parser.add_argument("--name", dest="name", help="override the public name")
    parser.add_argument("--out", type=Path, default=OUTPUT_DIR, help="output directory")
    args = parser.parse_args()

    config = load_config()

    if args.refresh_pex:
        print("Importing compiled Papyrus scripts")
        refresh_pex(config)
        return 0

    name = args.name or config["public_name"]
    version = args.version or config["version"]

    print(f"Spell Hotbar 2 fork release build: {name} {version}")
    if not config.get("identity_frozen", False):
        print(
            "  NOTE: identity is not frozen (release ticket 01). Name and version are\n"
            "        provisional and the archive filename says so. Verification only."
        )
    print("\nCompiled scripts")
    verify_pex_currency()

    print("\nCollecting files")
    members = collect_members()
    print(f"  {len(members)} files from the working tree (git-tracked, plus build outputs)")
    for path, reason in EXCLUSIONS.items():
        print(f"  excluded {path}\n    {reason}")

    base_root = Path(config["base_mod"]["install_path"])
    if not base_root.is_dir():
        fail(
            f"base mod not found at {base_root}. The overwrite proof compares every "
            "packaged file against the installed base mod; set base_mod.install_path in "
            "deploy/release/release.json."
        )
    print(f"\nClassifying against base {config['base_mod']['name']} "
          f"{config['base_mod']['supported_version']}")
    verify_base_install(base_root, config["base_mod"])
    content_index = index_base_by_content(base_root)
    print(f"  indexed {len(content_index)} distinct base files by content")
    counts = classify_against_base(members, base_root, content_index)
    print(f"  {counts['ADDITION']} additions, {counts['OVERWRITE']} overwrites, "
          f"{counts['REDUNDANT']} redundant")
    check_classification(members)
    for member in sorted(members, key=lambda m: m.arcname):
        if member.classification == "OVERWRITE":
            print(f"  overwrites base: {member.arcname}")

    print("\nLine endings")
    check_nemesis_line_endings(members)

    dll_sha = next(m.sha256 for m in members if m.arcname.endswith("SpellHotbar2.dll"))
    readme = render_readme(config, name, version, dll_sha)

    print("\nReal-name guard")
    check_forbidden_strings(members, config, readme)

    stem = archive_stem(config, name, version)
    manifest_path = args.out / f"{stem}.manifest.json"

    if args.check:
        write_manifest(manifest_path, members, config, name, version, None, counts)
        print(f"\nCheck passed. Manifest: {display_path(manifest_path)}")
        return 0

    archive = args.out / f"{stem}.zip"
    print(f"\nWriting {display_path(archive)}")
    write_archive(members, readme, archive)

    print("Verifying the archive")
    verify_archive(archive, members, readme, dll_sha)

    write_manifest(manifest_path, members, config, name, version, archive, counts)
    size_mb = archive.stat().st_size / (1024 * 1024)
    print(
        f"\nDone. {archive.name}: {len(members) + 1} files, {size_mb:.2f} MB\n"
        f"Manifest: {display_path(manifest_path)}"
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except FileNotFoundError as exc:
        # `git` off PATH lands here, as does a source file that vanished mid-build.
        fail(f"{exc.filename or exc}: {exc.strerror or exc}")
    except KeyError as exc:
        fail(f"deploy/release/release.json is missing the key {exc}")
    except json.JSONDecodeError as exc:
        fail(f"deploy/release/release.json is not valid JSON: {exc}")
    except subprocess.CalledProcessError as exc:
        fail(f"git failed: {' '.join(exc.cmd)}\n{exc.stderr}")
