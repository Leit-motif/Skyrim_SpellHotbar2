"""Static validation for ticket 58 (the `shcr` Nemesis patch).

Ported from `validate_shcc.py`, minus the FOMOD checks -- ticket 58 deliberately ships no
FOMOD option until the mechanism is owner-certified.

Checks, in order:
  1. the expected file census, and that every file is ASCII, BOM-free, CRLF-terminated and
     ends with a newline;
  2. MOD_CODE / ORIGINAL / CLOSE markers are balanced and correctly ordered, and every
     MOD_CODE block in a vanilla-node file carries this mod's own code (`shcr`);
  3. every file's `<hkobject name="#X">` matches its filename, and hkobject tags balance;
  4. every `#shcr$N` referenced anywhere in the patch is defined by exactly one of its own
     files, and every defined node is referenced;
  5. the four target states are wired to the nodes the ticket says they should be, and keep
     their vanilla pointer in the ORIGINAL body;
  6. the plant binds `bAllowRotation` uninverted, and the variable tables all grew by one.

Run:  python .scratch/shcr-build/validate_shcr.py
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SHCR = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shcr"

CODE = "shcr"

failures: list[str] = []
checks = 0


def check(ok: bool, label: str) -> bool:
    global checks
    checks += 1
    if not ok:
        failures.append(label)
    return ok


# ---------------------------------------------------------------- 1. census / encoding
files = sorted(p for p in SHCR.rglob("*") if p.is_file())
EXPECTED = {"info.ini"} | {
    f"magicbehavior/{n}"
    for n in (
        [f"#{CODE}${i}.txt" for i in range(6)]
        + ["#0077.txt", "#0078.txt", "#0079.txt"]
        + ["#0926.txt", "#0930.txt", "#0965.txt", "#0998.txt"]
    )
}
found = {p.relative_to(SHCR).as_posix() for p in files}
check(found == EXPECTED,
      f"census mismatch: missing {sorted(EXPECTED - found)}, extra {sorted(found - EXPECTED)}")
check(len(files) == 14, f"expected 14 shcr files, found {len(files)}")

for p in files:
    raw = p.read_bytes()
    rel = p.relative_to(REPO)
    check(not raw.startswith(b"\xef\xbb\xbf"), f"{rel}: BOM")
    check(all(b < 128 for b in raw), f"{rel}: non-ASCII byte")
    check(raw.endswith(b"\r\n"), f"{rel}: missing trailing CRLF")
    check(raw.count(b"\n") == raw.count(b"\r\n"), f"{rel}: bare LF present")

# ---------------------------------------------------------------- 2. MOD_CODE markers
MARKER = re.compile(r"^<!-- (?:MOD_CODE ~(\w+)~ OPEN|(ORIGINAL)|(CLOSE)) -->$")

node_files = sorted((SHCR / "magicbehavior").glob("*.txt"))
for p in node_files:
    rel = p.relative_to(REPO)
    lines = p.read_text().replace("\r\n", "\n").split("\n")
    state = 0  # 0 outside, 1 in addition, 2 in ORIGINAL body
    opens = 0
    for ln in lines:
        m = MARKER.match(ln.strip())
        if not m:
            continue
        mod, orig, close = m.groups()
        if mod:
            check(state == 0, f"{rel}: nested MOD_CODE")
            check(mod == CODE, f"{rel}: foreign mod code ~{mod}~")
            state, opens = 1, opens + 1
        elif orig:
            check(state == 1, f"{rel}: ORIGINAL outside an open block")
            state = 2
        else:
            check(state in (1, 2), f"{rel}: unmatched CLOSE")
            state = 0
    check(state == 0, f"{rel}: unclosed MOD_CODE block")
    is_new_node = p.stem.startswith(f"#{CODE}$")
    check(opens == (0 if is_new_node else 1),
          f"{rel}: expected {'0' if is_new_node else '1'} MOD_CODE block, found {opens}")

# ---------------------------------------------------------------- 3. node identity
OBJ = re.compile(r'<hkobject name="#([^"]+)" class="(\w+)"')
defined: dict[str, str] = {}
for p in node_files:
    rel = p.relative_to(REPO)
    text = p.read_text()
    names = OBJ.findall(text)
    check(len(names) == 1, f"{rel}: expected exactly one hkobject, found {len(names)}")
    if names:
        name, cls = names[0]
        check(f"#{name}" == p.stem, f"{rel}: node name #{name} does not match filename")
        check(name not in defined, f"{rel}: node #{name} defined more than once")
        defined[name] = cls
    check(text.count("<hkobject") == text.count("</hkobject>"),
          f"{rel}: unbalanced hkobject tags")

# ---------------------------------------------------------------- 4. reference closure
own = {n for n in defined if n.startswith(f"{CODE}$")}
referenced: set[str] = set()
for p in node_files:
    for ref in re.findall(rf"#({re.escape(CODE)}\$\d+)", p.read_text()):
        if f"#{ref}" != p.stem:
            referenced.add(ref)
check(referenced <= own, f"dangling references: {sorted(referenced - own)}")
check(own <= referenced, f"unreferenced new nodes: {sorted(own - referenced)}")
check(len(own) == 6, f"expected 6 new nodes, found {len(own)}: {sorted(own)}")

# ---------------------------------------------------------------- 5. the wiring
EXPECT_CLASS = {
    f"{CODE}$0": "hkbVariableBindingSet",
    f"{CODE}$1": "BSIsActiveModifier",
    f"{CODE}$2": "hkbModifierGenerator",
    f"{CODE}$3": "hkbModifierGenerator",
    f"{CODE}$4": "hkbModifierGenerator",
    f"{CODE}$5": "hkbModifierGenerator",
}
for name, cls in EXPECT_CLASS.items():
    check(defined.get(name) == cls, f"#{name}: expected class {cls}, got {defined.get(name)}")

# Each replacement generator wraps vanilla #0088 and applies the one shared modifier.
for name in (f"{CODE}$2", f"{CODE}$3", f"{CODE}$4", f"{CODE}$5"):
    text = (SHCR / "magicbehavior" / f"#{name}.txt").read_text()
    check('<hkparam name="generator">#0088</hkparam>' in text,
          f"#{name}: does not wrap vanilla #0088")
    check(f'<hkparam name="modifier">#{CODE}$1</hkparam>' in text,
          f"#{name}: does not apply #{CODE}$1")

# The four target states: new generator in the MOD_CODE body, vanilla one in ORIGINAL.
WIRING = {
    "#0926.txt": ("MagicCastingLocomotionState", "#0923", f"#{CODE}$2"),
    "#0930.txt": ("MagicCast_Standing", "#0088", f"#{CODE}$3"),
    "#0965.txt": ("MagicCast_TurnLeft_State", "#0961", f"#{CODE}$4"),
    "#0998.txt": ("MagicCast_TurnRight_State", "#0996", f"#{CODE}$5"),
}
for fname, (state_name, vanilla, node) in WIRING.items():
    p = SHCR / "magicbehavior" / fname
    if not check(p.exists(), f"missing target patch {fname}"):
        continue
    text = p.read_text().replace("\r\n", "\n")
    check(f'<hkparam name="name">{state_name}</hkparam>' in text,
          f"{fname}: not state {state_name}")
    body = text.split(f"<!-- MOD_CODE ~{CODE}~ OPEN -->")[-1]
    added, rest = body.split("<!-- ORIGINAL -->")
    original = rest.split("<!-- CLOSE -->")[0]
    check(f'<hkparam name="generator">{node}</hkparam>' in added,
          f"{fname}: replacement generator is not {node}")
    check(f'<hkparam name="generator">{vanilla}</hkparam>' in original,
          f"{fname}: ORIGINAL body is not the vanilla generator {vanilla}")
    check(text.count('<hkparam name="generator">') == 2,
          f"{fname}: unexpected number of generator lines")

# ---------------------------------------------------------------- 6. the plant / variables
bset = (SHCR / "magicbehavior" / f"#{CODE}$0.txt").read_text()
order = re.findall(r"variableIndex\">\$variableID\[(\w+)\]\$", bset)
check(order == ["bAllowRotation"], f"binding set changed: {order}")
check('<hkparam name="bindings" numelements="1">' in bset,
      "binding set numelements does not match its one binding")

plant = (SHCR / "magicbehavior" / f"#{CODE}$1.txt").read_text()
inverts = re.findall(r'bInvertActive(\d)">(\w+)<', plant)
check(inverts == [(str(i), "false") for i in range(5)], f"invert flags changed: {inverts}")
actives = re.findall(r'bIsActive(\d)">(\w+)<', plant)
check(actives == [(str(i), "false") for i in range(5)], f"bIsActive flags changed: {actives}")

decl = (SHCR / "magicbehavior" / "#0077.txt").read_text()
block = decl.split(f"<!-- MOD_CODE ~{CODE}~ OPEN -->")[-1].split("<!-- CLOSE -->")[0]
check("<hkcstring>bAllowRotation</hkcstring>" in block, "#0077.txt: bAllowRotation not declared")

counts = {
    "#0077.txt": len(re.findall(r"<hkcstring>", block)),
    "#0078.txt": len(re.findall(r"<hkobject>", (SHCR / "magicbehavior" / "#0078.txt").read_text()
                                .split(f"<!-- MOD_CODE ~{CODE}~ OPEN -->")[-1].split("<!-- CLOSE -->")[0])),
    "#0079.txt": len(re.findall(r"<hkparam name=\"type\">", (SHCR / "magicbehavior" / "#0079.txt").read_text()
                                .split(f"<!-- MOD_CODE ~{CODE}~ OPEN -->")[-1].split("<!-- CLOSE -->")[0])),
}
check(set(counts.values()) == {1}, f"variable-table additions disagree: {counts}")

# ---------------------------------------------------------------- report
print(f"{checks} checks, {len(failures)} failure(s)")
for f in failures:
    print(f"  FAIL {f}")
sys.exit(1 if failures else 0)
