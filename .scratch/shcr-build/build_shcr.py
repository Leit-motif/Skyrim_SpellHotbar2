"""Generate the `shcr` Nemesis patch (ticket 58 -- casting commitment).

Ticket 58 replaces the generators behind the four vanilla `magicbehavior` casting-locomotion
state nodes so that casting stops routing through locomotion-blending generators. This is
Enemy Magelock's structural approach, applied globally (no player/NPC split): each of the four
states is re-pointed at a new `hkbModifierGenerator` that wraps the vanilla standing-cast
manual-selector generator `#0088` and applies a single `BSIsActiveModifier` bound to
`bAllowRotation`, so the caster can still pivot while committed.

This script is the lever: it derives the PRISTINE vanilla text of every node it patches from
the copies other Nemesis mods ship (each of which is vanilla text plus that mod's own MOD_CODE
blocks), cross-verifies those derivations against each other where more than one donor carries
the same node, and then emits the `shcr` patch files with only `shcr`'s own MOD_CODE blocks.

Donors are read READ-ONLY from the MO2 mods directory and the local Enemy Magelock download.
Nothing outside the repo is written.

Run:  python .scratch/shcr-build/build_shcr.py [--check]

  --check  rebuild in memory and diff against the committed tree (no writes)
"""

from __future__ import annotations

import argparse
import difflib
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
OUT = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shcr"

MODS = Path(r"C:\Nolvus\Instances\Nolvus Awakening\MODS\mods")
DONOR_MSCO = MODS / "MSCO Magic Casting Behavior Overhaul" / "Nemesis_engine" / "mod" / "msco" / "magicbehavior"
DONOR_HOTKEY = MODS / "Hot Key Skill" / "Nemesis_Engine" / "mod" / "hotkey" / "magicbehavior"
DONOR_ALTMAG = Path(
    r"C:\Users\Rando\Downloads\Enemy Magelock-49378-1-0-0-1619990342"
    r"\Enemy Magelock - NPC Magic Casting Commitment\Nemesis_engine\mod\altmag\magicbehavior"
)

CODE = "shcr"
OPEN = f"<!-- MOD_CODE ~{CODE}~ OPEN -->"
ORIGINAL = "<!-- ORIGINAL -->"
CLOSE = "<!-- CLOSE -->"

T = "\t\t\t"  # hkparam indent inside an hkobject
T4 = "\t\t\t\t"  # array-member / list-element indent


# --------------------------------------------------------------------------------------
# pristine-vanilla derivation
# --------------------------------------------------------------------------------------

MARKER = re.compile(r"^<!-- (MOD_CODE ~\w+~ OPEN|ORIGINAL|CLOSE) -->$")


def read_lines(path: Path) -> list[str]:
    text = path.read_bytes().decode("utf-8")
    if text.startswith("\ufeff"):
        raise SystemExit(f"BOM in donor {path}")
    return text.replace("\r\n", "\n").split("\n")


def strip_mod_code(lines: list[str]) -> list[str]:
    """Return the vanilla text: drop every MOD_CODE addition, keep every ORIGINAL body."""
    out: list[str] = []
    # state: 0 outside, 1 inside a MOD_CODE addition, 2 inside its ORIGINAL body
    state = 0
    depth = 0
    for line in lines:
        m = MARKER.match(line.strip())
        if m:
            kind = m.group(1)
            if kind.startswith("MOD_CODE"):
                if state != 0:
                    raise SystemExit("nested MOD_CODE block in donor")
                state, depth = 1, depth + 1
            elif kind == "ORIGINAL":
                if state != 1:
                    raise SystemExit("stray ORIGINAL in donor")
                state = 2
            else:  # CLOSE
                if state == 0:
                    raise SystemExit("unbalanced CLOSE in donor")
                state, depth = 0, depth - 1
            continue
        if state == 1:
            continue  # the mod's own addition
        out.append(line)
    if state != 0 or depth != 0:
        raise SystemExit("unbalanced MOD_CODE markers in donor")
    return out


def pristine(*donors: Path) -> list[str]:
    """Derive vanilla text from one or more donors; all donors must agree."""
    derived = [(d, strip_mod_code(read_lines(d))) for d in donors]
    base = derived[0][1]
    for path, other in derived[1:]:
        if other != base:
            diff = "\n".join(difflib.unified_diff(base, other, str(derived[0][0]), str(path), lineterm=""))
            raise SystemExit(f"donors disagree on vanilla text:\n{diff}")
    return base


# --------------------------------------------------------------------------------------
# patch primitives
# --------------------------------------------------------------------------------------


def append_into_array(lines: list[str], param: str, additions: list[str]) -> list[str]:
    """Insert `additions` just before the </hkparam> that closes `param`'s array."""
    open_re = re.compile(rf'<hkparam name="{re.escape(param)}"')
    out, i, done = [], 0, False
    while i < len(lines):
        line = lines[i]
        out.append(line)
        if not done and open_re.search(line) and "</hkparam>" not in line:
            # Array elements may themselves contain `</hkparam>` (e.g. variableInfos), so
            # close on the tag at the array's own indent, not on the first one seen.
            closing = f"{T}</hkparam>"
            i += 1
            while lines[i] != closing:
                out.append(lines[i])
                i += 1
                if i >= len(lines):
                    raise SystemExit(f"unterminated array param {param!r}")
            out.extend([OPEN, *additions, CLOSE])
            out.append(lines[i])
            done = True
        i += 1
    if not done:
        raise SystemExit(f"array param {param!r} not found")
    return out


def replace_param(lines: list[str], param: str, new_value: str, expect: str) -> list[str]:
    """Wrap the single-valued `param` line in an OPEN/ORIGINAL/CLOSE replacement.

    `expect` is the vanilla value the pristine text must already carry; a mismatch means the
    donor derivation drifted or the node is not the one the ticket names, so fail loudly.
    """
    target = f'{T}<hkparam name="{param}">'
    out, hits = [], 0
    for line in lines:
        if line.startswith(target):
            hits += 1
            have = line[len(target):].split("</hkparam>")[0]
            if have != expect:
                raise SystemExit(
                    f"pristine {param!r} is {have!r}, expected vanilla {expect!r}"
                )
            out.extend([OPEN, f'{T}<hkparam name="{param}">{new_value}</hkparam>', ORIGINAL, line, CLOSE])
        else:
            out.append(line)
    if hits != 1:
        raise SystemExit(f"expected exactly one {param!r} line, found {hits}")
    return out


def bool_variable_info() -> list[str]:
    return [
        f"{T4}<hkobject>",
        f'{T4}\t<hkparam name="role">',
        f"{T4}\t\t<hkobject>",
        f'{T4}\t\t\t<hkparam name="role">ROLE_DEFAULT</hkparam>',
        f'{T4}\t\t\t<hkparam name="flags">0</hkparam>',
        f"{T4}\t\t</hkobject>",
        f"{T4}\t</hkparam>",
        f'{T4}\t<hkparam name="type">VARIABLE_TYPE_BOOL</hkparam>',
        f"{T4}</hkobject>",
    ]


def zero_variable_value() -> list[str]:
    return [
        f"{T4}<hkobject>",
        f'{T4}\t<hkparam name="value">0</hkparam>',
        f"{T4}</hkobject>",
    ]


# --------------------------------------------------------------------------------------
# the shcr payload
# --------------------------------------------------------------------------------------

# The one variable the plant binds that vanilla magicbehavior's 81-entry table lacks.
NEW_VARIABLES = ["bAllowRotation"]

# The vanilla standing-cast generator every replacement modifier generator wraps.
CAST_GENERATOR = "#0088"

# The four vanilla casting-locomotion states, by file: (new node, vanilla generator, name).
STATE_PATCHES = {
    "#0926.txt": (f"#{CODE}$2", "#0923", "MagicCastingLocomotionState"),
    "#0930.txt": (f"#{CODE}$3", "#0088", "MagicCast_Standing"),
    "#0965.txt": (f"#{CODE}$4", "#0961", "MagicCast_TurnLeft_State"),
    "#0998.txt": (f"#{CODE}$5", "#0996", "MagicCast_TurnRight_State"),
}

MG_NAMES = {
    f"{CODE}$2": "SHCR_MagicCastingLocomotion_MG",
    f"{CODE}$3": "SHCR_MagicCast_Standing_MG",
    f"{CODE}$4": "SHCR_MagicCast_TurnLeft_MG",
    f"{CODE}$5": "SHCR_MagicCast_TurnRight_MG",
}


def binding(member: str, var: str) -> list[str]:
    return [
        f"{T4}<hkobject>",
        f'{T4}\t<hkparam name="memberPath">{member}</hkparam>',
        f'{T4}\t<hkparam name="variableIndex">$variableID[{var}]$</hkparam>',
        f'{T4}\t<hkparam name="bitIndex">-1</hkparam>',
        f'{T4}\t<hkparam name="bindingType">BINDING_TYPE_VARIABLE</hkparam>',
        f"{T4}</hkobject>",
    ]


def new_nodes() -> dict[str, list[str]]:
    """The six nodes shcr contributes.

    `$0`/`$1` mirror Enemy Magelock's `#altmag$52`/`#altmag$51` pair -- a rotation-lock
    release and nothing else. `$2`..`$5` are the four replacement generators.
    """
    nodes: dict[str, list[str]] = {}

    nodes[f"{CODE}$0"] = [
        f'\t\t<hkobject name="#{CODE}$0" class="hkbVariableBindingSet" signature="0x338ad4ff">',
        f'{T}<hkparam name="bindings" numelements="1">',
        *binding("bIsActive0", "bAllowRotation"),
        f"{T}</hkparam>",
        f'{T}<hkparam name="indexOfBindingToEnable">-1</hkparam>',
        "\t\t</hkobject>",
    ]

    nodes[f"{CODE}$1"] = [
        f'\t\t<hkobject name="#{CODE}$1" class="BSIsActiveModifier" signature="0xb0fde45a">',
        f'{T}<hkparam name="variableBindingSet">#{CODE}$0</hkparam>',
        f'{T}<hkparam name="userData">2</hkparam>',
        f'{T}<hkparam name="name">SHCR_AllowRotation_IsActiveModifier</hkparam>',
        f'{T}<hkparam name="enable">true</hkparam>',
        *[
            ln
            for i in range(5)
            for ln in (
                f'{T}<hkparam name="bIsActive{i}">false</hkparam>',
                f'{T}<hkparam name="bInvertActive{i}">false</hkparam>',
            )
        ],
        "\t\t</hkobject>",
    ]

    for name, label in MG_NAMES.items():
        nodes[name] = [
            f'\t\t<hkobject name="#{name}" class="hkbModifierGenerator" signature="0x1f81fae6">',
            f'{T}<hkparam name="variableBindingSet">null</hkparam>',
            f'{T}<hkparam name="userData">1</hkparam>',
            f'{T}<hkparam name="name">{label}</hkparam>',
            f'{T}<hkparam name="modifier">#{CODE}$1</hkparam>',
            f'{T}<hkparam name="generator">{CAST_GENERATOR}</hkparam>',
            "\t\t</hkobject>",
        ]

    return nodes


def vanilla_patches() -> dict[str, list[str]]:
    """The four vanilla-node patch files, plus the three variable-table declarations."""
    files: dict[str, list[str]] = {}

    # --- variable declarations ---------------------------------------------------------
    # `bAllowRotation` is not vanilla; shcr declares its own so the patch stands alone.
    # Duplicate NEW declarations across mods are tolerated by Nemesis (verified in the
    # merged temp_behaviors output, where `bAllowRotation` appears under several codes).
    n = len(NEW_VARIABLES)
    files["#0077.txt"] = append_into_array(
        pristine(DONOR_MSCO / "#0077.txt", DONOR_HOTKEY / "#0077.txt", DONOR_ALTMAG / "#0077.txt"),
        "variableNames",
        [f"{T4}<hkcstring>{v}</hkcstring>" for v in NEW_VARIABLES],
    )
    files["#0078.txt"] = append_into_array(
        pristine(DONOR_MSCO / "#0078.txt", DONOR_HOTKEY / "#0078.txt", DONOR_ALTMAG / "#0078.txt"),
        "wordVariableValues",
        [ln for _ in range(n) for ln in zero_variable_value()],
    )
    files["#0079.txt"] = append_into_array(
        pristine(DONOR_MSCO / "#0079.txt", DONOR_HOTKEY / "#0079.txt", DONOR_ALTMAG / "#0079.txt"),
        "variableInfos",
        [ln for _ in range(n) for ln in bool_variable_info()],
    )

    # --- the four casting-locomotion states --------------------------------------------
    # Only ALTMAG carries these four; the ticket-57 contention table confirms no installed
    # mod patches them, so a single-donor derivation is the whole available evidence.
    for fname, (node, vanilla_gen, state_name) in STATE_PATCHES.items():
        lines = pristine(DONOR_ALTMAG / fname)
        joined = "\n".join(lines)
        if f'<hkparam name="name">{state_name}</hkparam>' not in joined:
            raise SystemExit(f"{fname}: pristine text is not state {state_name!r}")
        files[fname] = replace_param(lines, "generator", node, vanilla_gen)

    return files


INFO_INI = [
    "name=Spell Hotbar 2 - Casting Commitment",
    "author=Amrit Chana",
    "site=null",
    "auto=null",
]


def build() -> dict[Path, bytes]:
    payload: dict[Path, bytes] = {}

    def emit(rel: str, lines: list[str]) -> None:
        body = "\r\n".join(ln.rstrip("\r") for ln in lines)
        if not body.endswith("\r\n"):
            body += "\r\n"
        payload[OUT / rel] = body.encode("ascii")

    emit("info.ini", INFO_INI)
    for name, lines in new_nodes().items():
        emit(f"magicbehavior/#{name}.txt", lines)
    for name, lines in vanilla_patches().items():
        emit(f"magicbehavior/{name}", lines)
    return payload


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="verify the committed tree matches")
    args = ap.parse_args()

    payload = build()

    if args.check:
        bad = 0
        for path, data in sorted(payload.items()):
            have = path.read_bytes() if path.exists() else b""
            if have != data:
                bad += 1
                print(f"MISMATCH {path.relative_to(REPO)}")
        extra = {p for p in (OUT.rglob("*") if OUT.exists() else []) if p.is_file()} - set(payload)
        for p in sorted(extra):
            bad += 1
            print(f"UNEXPECTED {p.relative_to(REPO)}")
        print(f"{'FAIL' if bad else 'OK'}: {len(payload)} files checked, {bad} problem(s)")
        return 1 if bad else 0

    for path, data in sorted(payload.items()):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        print(f"wrote {path.relative_to(REPO)} ({len(data)} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
