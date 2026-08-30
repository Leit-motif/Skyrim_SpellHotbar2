#!/usr/bin/env python3
"""Build the distributable archive for this fork of Spell Hotbar 2.

Ticket 59 (`.scratch/mco-integration/issues/59-...`). Our mod is an OVERWRITE over base
Spell Hotbar 2: the base mod is a hard requirement, ours installs after it and wins the
conflict. This script assembles only the files that are ours, proves that no
upstream-untouched asset rode along, and writes a reviewable manifest beside the archive.

Three rules do the real work here:

1.  **Only git-tracked files ship.** Trees are enumerated through `git ls-files`, so a
    local scratch file, a playtest animation drop, or anything else `.gitignore` covers
    cannot leak into a release. The two build outputs (the DLL, the two `.pex`) are the
    named exceptions.
2.  **Every member is classified against the installed base mod, byte for byte.**
    Absent from base -> ADDITION. Present and different -> OVERWRITE. Present and
    identical -> REDUNDANT, which fails the build: a byte-identical file is by definition
    an upstream-untouched asset and belongs to the base install, not to us.
3.  **The overwrite set is declared, not discovered.** `EXPECTED_OVERWRITES` below is the
    list of upstream files we replace. If the classification disagrees with it in either
    direction the build fails, so the overwrite ruling stays checkable instead of drifting.

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
import shutil
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
# Keep this in sync deliberately; the build fails rather than letting it drift.
EXPECTED_OVERWRITES = {
    "SKSE/Plugins/SpellHotbar2.dll",
    "Scripts/SpellHotbar.pex",
    "Scripts/SpellHotbarMCM.pex",
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
    """Repo-relative paths tracked by git under `prefix`, posix separators."""
    out = git("ls-files", "--", prefix).splitlines()
    return sorted(line.strip() for line in out if line.strip())


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
    """
    names: set[str] = set()
    try:
        names.add(git("config", "user.name").strip())
    except subprocess.CalledProcessError:
        pass
    try:
        for line in git("log", "--format=%an%n%cn", "-n", "200").splitlines():
            if line.strip():
                names.add(line.strip())
    except subprocess.CalledProcessError:
        pass
    names.discard("")
    names.discard(config.get("author_public", ""))
    names.update(config.get("extra_forbidden_strings", []))
    return sorted(n for n in names if len(n) >= 4)


# ------------------------------------------------------------------- compiled scripts


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
        psc = PROJECT_ROOT / psc_rel
        record[pex_name] = {
            "psc": psc_rel,
            "psc_sha256": sha256_of(psc),
            "pex_sha256": sha256_of(dst),
            "imported_from": str(src),
            "imported_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        }
        print(f"  imported {pex_name} <- {src}")

    PEX_RECORD.write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    print(f"  wrote {PEX_RECORD.relative_to(PROJECT_ROOT)}")


def verify_pex_currency() -> None:
    """Fail if a `.psc` changed since its `.pex` was imported."""
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
        print(f"  {pex_name} matches {psc_rel}")


# ----------------------------------------------------------------------- the manifest


def collect_members() -> list[Member]:
    """Enumerate every file that goes into the archive, with its archive path."""
    members: list[Member] = []
    seen: set[str] = set()

    def add(source: Path, arcname: str) -> None:
        if arcname in seen:
            fail(f"two sources map to the same archive path: {arcname}")
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

    # 3. The Nemesis patches (shtb, shcr) and the behaviour-data injector config.
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


def classify_against_base(members: list[Member], base_root: Path) -> dict[str, int]:
    """Tag each member ADDITION / OVERWRITE / REDUNDANT against the installed base mod."""
    counts = {"ADDITION": 0, "OVERWRITE": 0, "REDUNDANT": 0}
    for member in members:
        base_file = base_root / member.arcname
        if not base_file.is_file():
            member.classification = "ADDITION"
        elif sha256_of(base_file) == member.sha256:
            member.classification = "REDUNDANT"
        else:
            member.classification = "OVERWRITE"
        counts[member.classification] += 1
    return counts


def check_classification(members: list[Member]) -> None:
    redundant = [m.arcname for m in members if m.classification == "REDUNDANT"]
    if redundant:
        fail(
            "these files are byte-identical to the base mod and must not ship "
            "(upstream-untouched assets come from the base install):\n  "
            + "\n  ".join(redundant)
        )

    found = {m.arcname for m in members if m.classification == "OVERWRITE"}
    unexpected = sorted(found - EXPECTED_OVERWRITES)
    missing = sorted(EXPECTED_OVERWRITES - found)
    if unexpected:
        fail(
            "these files overwrite a base-mod file but are not in EXPECTED_OVERWRITES:\n  "
            + "\n  ".join(unexpected)
        )
    if missing:
        fail(
            "EXPECTED_OVERWRITES names files that do not actually overwrite the base "
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
        if b"\n" in data and data.count(b"\r\n") != data.count(b"\n"):
            offenders.append(member.arcname)
    if offenders:
        fail(
            "Nemesis patch files are not CRLF in the working tree "
            f"({len(offenders)} of them, e.g. {offenders[0]}). Check `git config core.autocrlf`."
        )
    print(f"  {sum(1 for m in members if m.arcname.startswith('Nemesis_Engine/'))} "
          "Nemesis files are CRLF")


def check_forbidden_strings(members: list[Member], config: dict) -> None:
    """Scan packaged text for a real name. Nemesis shows `author=` to every user."""
    needles = forbidden_strings(config)
    if not needles:
        print("  (no committer names resolved; real-name guard did not run)")
        return
    text_suffixes = {".txt", ".ini", ".json", ".csv", ".psc", ".md", ".xml"}
    hits: list[str] = []
    for member in members:
        if member.source.suffix.lower() not in text_suffixes:
            continue
        content = member.source.read_text(encoding="utf-8", errors="ignore")
        for needle in needles:
            if needle in content:
                hits.append(f"{member.arcname}: contains {needle!r}")
    if hits:
        fail("real name found in packaged files:\n  " + "\n  ".join(hits))
    print(f"  clean against {len(needles)} committer name(s)")


# ------------------------------------------------------------------------- the output


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
        names = set(zf.namelist())
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

        # Acceptance: the archive byte-contains the shtb and shcr patch files.
        for code in ("shtb", "shcr"):
            prefix = f"Nemesis_Engine/mod/{code}/"
            patch_files = [n for n in names if n.startswith(prefix)]
            source_count = len(tracked_files(f"nemesis/Nemesis_Engine/mod/{code}"))
            if len(patch_files) != source_count:
                fail(
                    f"{code}: archive has {len(patch_files)} files, working tree has "
                    f"{source_count}"
                )
            if f"{prefix}info.ini" not in names:
                fail(f"{code}: info.ini is missing from the archive")
            print(f"  {code}: {len(patch_files)} patch files, info.ini present")

        # Acceptance: both .pex are present.
        for pex_name in PEX_SOURCES:
            if f"Scripts/{pex_name}" not in names:
                fail(f"missing compiled script in archive: Scripts/{pex_name}")
        print(f"  compiled scripts: {', '.join(sorted(PEX_SOURCES))}")

        # Acceptance: the DLL in the archive is the current build.
        archived_dll_sha = sha256_of_bytes(zf.read("SKSE/Plugins/SpellHotbar2.dll"))
        if archived_dll_sha != dll_sha:
            fail("archived DLL does not match skse_plugin/build/release/SpellHotbar2.dll")
        print(f"  DLL matches the current build ({dll_sha[:16]})")


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
        "publication_blocked": config.get("publication_blocked", True),
        "publication_blocked_reason": config.get("publication_blocked_reason", ""),
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
    parser.add_argument("--check", action="store_true", help="verify only; write no archive")
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
    if config.get("publication_blocked", True):
        print(f"  NOTE: publication is blocked. {config['publication_blocked_reason']}")

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
    counts = classify_against_base(members, base_root)
    print(f"  {counts['ADDITION']} additions, {counts['OVERWRITE']} overwrites, "
          f"{counts['REDUNDANT']} redundant")
    check_classification(members)
    for member in sorted(members, key=lambda m: m.arcname):
        if member.classification == "OVERWRITE":
            print(f"  overwrites base: {member.arcname}")

    print("\nLine endings")
    check_nemesis_line_endings(members)

    print("\nReal-name guard")
    check_forbidden_strings(members, config)

    dll_sha = next(m.sha256 for m in members if m.arcname.endswith("SpellHotbar2.dll"))
    readme = render_readme(config, name, version, dll_sha)

    stem = archive_stem(config, name, version)
    manifest_path = args.out / f"{stem}.manifest.json"

    if args.check:
        write_manifest(manifest_path, members, config, name, version, None, counts)
        print(f"\nCheck passed. Manifest: {manifest_path.relative_to(PROJECT_ROOT)}")
        return 0

    archive = args.out / f"{stem}.zip"
    print(f"\nWriting {archive.relative_to(PROJECT_ROOT)}")
    write_archive(members, readme, archive)

    print("Verifying the archive")
    verify_archive(archive, members, readme, dll_sha)

    write_manifest(manifest_path, members, config, name, version, archive, counts)
    size_mb = archive.stat().st_size / (1024 * 1024)
    print(
        f"\nDone. {archive.name}: {len(members) + 1} files, {size_mb:.2f} MB\n"
        f"Manifest: {manifest_path.relative_to(PROJECT_ROOT)}"
    )
    if config.get("publication_blocked", True):
        print("Do not upload this archive: publication is blocked in release.json.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
