"""Generate the `shcc` Nemesis patch (ticket 33 -- rooted concentration casts).

Ticket 33 roots the six vanilla `magicbehavior` concentration states for every actor
by planting SH2's proven `BSIsActiveModifier` + `hkbVariableBindingSet` pair on each.

This script is the lever: it derives the PRISTINE vanilla text of every node it patches
from the copies other Nemesis mods in the local stack ship (each of which is vanilla text
plus that mod's own MOD_CODE blocks), cross-verifies those derivations against each other
where two donors carry the same node, and then emits the `shcc` patch files with only
`shcc`'s own MOD_CODE blocks added.

Donors are read READ-ONLY from the MO2 mods directory. Nothing outside the repo is written.

Run:  python .scratch/shcc-build/build_shcc.py [--check]

  --check  regenerate into a temp dir and diff against the committed tree (no writes)
"""

from __future__ import annotations

import argparse
import difflib
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
OUT = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shcc"

MODS = Path(r"C:\Nolvus\Instances\Nolvus Awakening\MODS\mods")
DONOR_SBEEF = MODS / "State Behavior Framework" / "Nemesis_Engine" / "mod" / "sbeef" / "magicbehavior"
DONOR_MSCO = MODS / "MSCO Magic Casting Behavior Overhaul" / "Nemesis_engine" / "mod" / "msco" / "magicbehavior"
DONOR_HOTKEY = MODS / "Hot Key Skill" / "Nemesis_Engine" / "mod" / "hotkey" / "magicbehavior"

CODE = "shcc"
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


def replace_param(lines: list[str], param: str, new_value: str) -> list[str]:
    """Wrap the single-valued `param` line in an OPEN/ORIGINAL/CLOSE replacement."""
    target = f'{T}<hkparam name="{param}">'
    out, hits = [], 0
    for line in lines:
        if line.startswith(target):
            hits += 1
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
# the shcc payload
# --------------------------------------------------------------------------------------

# Variables the plant binds that are NOT in vanilla magicbehavior's 81-entry table.
# `bHeadTrackSpine` IS vanilla (index 65) and must not be redeclared.
NEW_VARIABLES = ["bAnimationDriven", "bAllowRotation", "HKSMoveON"]


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
    """The five nodes shcc contributes. Node text mirrors SH2's own proven plant
    (`nemesis/Nemesis_Engine/mod/shtb/magicbehavior/#shtb$13.txt` / `$14.txt`)."""
    nodes: dict[str, list[str]] = {}

    # $0 -- binding set. Invert semantics copied verbatim from #shtb$14:
    #   bAnimationDriven = true  (root: translation comes from the clip, not the controller)
    #   bAllowRotation   = true  (the caster still pivots to track its target)
    #   HKSMoveON        = true
    #   bHeadTrackSpine  = false (bInvertActive3=true)
    nodes["shcc$0"] = [
        '\t\t<hkobject name="#shcc$0" class="hkbVariableBindingSet" signature="0x338ad4ff">',
        f'{T}<hkparam name="bindings" numelements="4">',
        *binding("bIsActive0", "bAnimationDriven"),
        *binding("bIsActive1", "bAllowRotation"),
        *binding("bIsActive2", "HKSMoveON"),
        *binding("bIsActive3", "bHeadTrackSpine"),
        f"{T}</hkparam>",
        f'{T}<hkparam name="indexOfBindingToEnable">-1</hkparam>',
        "\t\t</hkobject>",
    ]

    # $1 -- the modifier itself, shared by all six states.
    nodes["shcc$1"] = [
        '\t\t<hkobject name="#shcc$1" class="BSIsActiveModifier" signature="0xb0fde45a">',
        f'{T}<hkparam name="variableBindingSet">#shcc$0</hkparam>',
        f'{T}<hkparam name="userData">2</hkparam>',
        f'{T}<hkparam name="name">SHCC_ConcentrationRoot_IsActiveModifier</hkparam>',
        f'{T}<hkparam name="enable">true</hkparam>',
        f'{T}<hkparam name="bIsActive0">false</hkparam>',
        f'{T}<hkparam name="bInvertActive0">false</hkparam>',
        f'{T}<hkparam name="bIsActive1">false</hkparam>',
        f'{T}<hkparam name="bInvertActive1">false</hkparam>',
        f'{T}<hkparam name="bIsActive2">false</hkparam>',
        f'{T}<hkparam name="bInvertActive2">false</hkparam>',
        f'{T}<hkparam name="bIsActive3">false</hkparam>',
        f'{T}<hkparam name="bInvertActive3">true</hkparam>',
        f'{T}<hkparam name="bIsActive4">false</hkparam>',
        f'{T}<hkparam name="bInvertActive4">false</hkparam>',
        "\t\t</hkobject>",
    ]

    # $2 -- modifier list wrapping vanilla #0106, for the two Self-concentration
    # modifier generators (#0131, #0440) whose `modifier` is a single pointer, not a list.
    nodes["shcc$2"] = [
        '\t\t<hkobject name="#shcc$2" class="hkbModifierList" signature="0xa4180ca1">',
        f'{T}<hkparam name="variableBindingSet">null</hkparam>',
        f'{T}<hkparam name="userData">1</hkparam>',
        f'{T}<hkparam name="name">SHCC_SelfConcentration_ML</hkparam>',
        f'{T}<hkparam name="enable">true</hkparam>',
        f'{T}<hkparam name="modifiers" numelements="2">',
        f"{T4}#0106",
        f"{T4}#shcc$1",
        f"{T}</hkparam>",
        "\t\t</hkobject>",
    ]

    # $3/$4 -- the two DualMagic concentration states have no vanilla modifier generator
    # at all; their `generator` points straight at the inner state machine. Wrap it, the
    # same way sbeef (#sbeef$21/$25) and pscd (#pscd$10) already do.
    for name, gen, sm in (("shcc$3", "SHCC_DualMagic_SelfConcentration_MG", "#0318"),
                          ("shcc$4", "SHCC_DualMagic_AimedConcentration_MG", "#0338")):
        nodes[name] = [
            f'\t\t<hkobject name="#{name}" class="hkbModifierGenerator" signature="0x1f81fae6">',
            f'{T}<hkparam name="variableBindingSet">null</hkparam>',
            f'{T}<hkparam name="userData">1</hkparam>',
            f'{T}<hkparam name="name">{gen}</hkparam>',
            f'{T}<hkparam name="modifier">#shcc$1</hkparam>',
            f'{T}<hkparam name="generator">{sm}</hkparam>',
            "\t\t</hkobject>",
        ]

    return nodes


def vanilla_patches() -> dict[str, list[str]]:
    """The six vanilla-node patch files, plus the three variable-table declarations."""
    files: dict[str, list[str]] = {}

    # --- variable declarations ---------------------------------------------------------
    # `bAnimationDriven` / `bAllowRotation` / `HKSMoveON` exist at runtime today only
    # because Hot Key Skill declares them. shcc declares its own so the patch stands alone;
    # duplicate NEW declarations across mods are tolerated (verified in the merged
    # temp_behaviors output, where `bAllowRotation` appears under five different codes).
    n = len(NEW_VARIABLES)
    files["#0077.txt"] = append_into_array(
        pristine(DONOR_MSCO / "#0077.txt", DONOR_HOTKEY / "#0077.txt"),
        "variableNames",
        [f"{T4}<hkcstring>{v}</hkcstring>" for v in NEW_VARIABLES],
    )
    files["#0078.txt"] = append_into_array(
        pristine(DONOR_MSCO / "#0078.txt", DONOR_HOTKEY / "#0078.txt"),
        "wordVariableValues",
        [ln for _ in range(n) for ln in zero_variable_value()],
    )
    files["#0079.txt"] = append_into_array(
        pristine(DONOR_MSCO / "#0079.txt", DONOR_HOTKEY / "#0079.txt"),
        "variableInfos",
        [ln for _ in range(n) for ln in bool_variable_info()],
    )

    # --- the six target states ---------------------------------------------------------
    # MRh_AimedConcentration (#0145) and MLh_AimedConcentration (#0453) reach a REAL
    # vanilla hkbModifierList, so the plant is a conflict-free array append.
    files["#0184.txt"] = append_into_array(
        pristine(DONOR_SBEEF / "#0184.txt"), "modifiers", [f"{T4}#shcc$1"])
    files["#0489.txt"] = append_into_array(
        pristine(DONOR_SBEEF / "#0489.txt"), "modifiers", [f"{T4}#shcc$1"])

    # MRh_SelfConcentration (#0130) / MLh_SelfConcentration (#0439) reach a single-pointer
    # `modifier` on a vanilla hkbModifierGenerator; wrap vanilla #0106 in a list.
    files["#0131.txt"] = replace_param(pristine(DONOR_SBEEF / "#0131.txt"), "modifier", "#shcc$2")
    files["#0440.txt"] = replace_param(pristine(DONOR_SBEEF / "#0440.txt"), "modifier", "#shcc$2")

    # DualMagic_SelfConcentration (#0317) / DualMagic_AimedConcentration (#0337) have no
    # modifier generator at all; insert one.
    files["#0317.txt"] = replace_param(pristine(DONOR_SBEEF / "#0317.txt"), "generator", "#shcc$3")
    files["#0337.txt"] = replace_param(pristine(DONOR_SBEEF / "#0337.txt"), "generator", "#shcc$4")

    return files


INFO_INI = [
    "name=Spell Hotbar 2 - Rooted Concentration Casts",
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
