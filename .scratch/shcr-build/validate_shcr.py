"""Static validation for ticket 58's casting-commitment payload, now inside `shtb`.

Checks, in order:
  1. the commitment-owned files exist, and each is ASCII, BOM-free, CRLF-terminated and
     ends with a newline;
  2. no leftover `shcr` tree or `~shcr~` marker remains under `nemesis/`;
  3. MOD_CODE / ORIGINAL / CLOSE markers are balanced, and commitment-owned vanilla-node
     files carry `shtb`;
  4. every file's `<hkobject name="#X">` matches its filename, and hkobject tags balance;
  5. every `#shtb$31`..`$36` referenced anywhere in those files is defined by exactly one
     of them, and every defined commitment node is referenced;
  6. the four target states are wired to those nodes, and keep their vanilla pointer in
     the ORIGINAL body;
  7. the plant binds `bAllowRotation` uninverted, and shtb's own #0077 declares it.

Run:  python .scratch/shcr-build/validate_shcr.py
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SHTB = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shtb"
SHCR = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shcr"

CODE = "shtb"
NODE_BASE = 31
BIND = f"{CODE}${NODE_BASE}"
PLANT = f"{CODE}${NODE_BASE + 1}"
MGS = [f"{CODE}${NODE_BASE + i}" for i in range(2, 6)]

failures: list[str] = []
checks = 0


def check(ok: bool, label: str) -> bool:
    global checks
    checks += 1
    if not ok:
        failures.append(label)
    return ok


OWNED = {
    f"magicbehavior/#{n}.txt" for n in [BIND, PLANT, *MGS]
} | {
    "magicbehavior/#0078.txt",
    "magicbehavior/#0926.txt",
    "magicbehavior/#0930.txt",
    "magicbehavior/#0965.txt",
    "magicbehavior/#0998.txt",
}

# ---------------------------------------------------------------- 1. census / encoding
check(not SHCR.exists(), f"leftover shcr tree still at {SHCR.relative_to(REPO)}")

files = []
for rel in sorted(OWNED):
    p = SHTB / rel
    files.append(p)
    check(p.is_file(), f"missing commitment file {rel}")

for p in files:
    if not p.is_file():
        continue
    raw = p.read_bytes()
    rel = p.relative_to(REPO)
    check(not raw.startswith(b"\xef\xbb\xbf"), f"{rel}: BOM")
    check(all(b < 128 for b in raw), f"{rel}: non-ASCII byte")
    check(raw.endswith(b"\r\n"), f"{rel}: missing trailing CRLF")
    check(raw.count(b"\n") == raw.count(b"\r\n"), f"{rel}: bare LF present")

# ---------------------------------------------------------------- 2. no shcr residue
nemesis_root = REPO / "nemesis"
for p in nemesis_root.rglob("*"):
    if not p.is_file():
        continue
    text = p.read_text(encoding="utf-8", errors="replace")
    check("~shcr~" not in text and "#shcr$" not in text,
          f"{p.relative_to(REPO)}: leftover shcr token")

# ---------------------------------------------------------------- 3. MOD_CODE markers
MARKER = re.compile(r"^<!-- (?:MOD_CODE ~(\w+)~ OPEN|(ORIGINAL)|(CLOSE)) -->$")

node_files = [p for p in files if p.suffix == ".txt" and p.is_file()]
for p in node_files:
    rel = p.relative_to(REPO)
    lines = p.read_text().replace("\r\n", "\n").split("\n")
    state = 0
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

# #0077 / #0079 are shared with the cast-state payload; they must declare bAllowRotation
# but may carry more than one shtb block. Checked separately below.

# ---------------------------------------------------------------- 4. node identity (owned files)
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

# ---------------------------------------------------------------- 5. reference closure among $31..$36
own = {n for n in defined if n.startswith(f"{CODE}$") and n.split("$")[-1].isdigit()
       and int(n.split("$")[-1]) >= NODE_BASE}
referenced: set[str] = set()
# Scan the whole shtb magicbehavior tree so the four state patches count as references.
for p in (SHTB / "magicbehavior").glob("*.txt"):
    for ref in re.findall(rf"#({re.escape(CODE)}\$\d+)", p.read_text()):
        if f"#{ref}" != p.stem:
            referenced.add(ref)
commitment_refs = {r for r in referenced if r.split("$")[-1].isdigit() and int(r.split("$")[-1]) >= NODE_BASE}
check(commitment_refs <= own, f"dangling commitment references: {sorted(commitment_refs - own)}")
check(own <= commitment_refs, f"unreferenced commitment nodes: {sorted(own - commitment_refs)}")
check(len(own) == 6, f"expected 6 commitment nodes, found {len(own)}: {sorted(own)}")

# ---------------------------------------------------------------- 6. the wiring
EXPECT_CLASS = {
    BIND: "hkbVariableBindingSet",
    PLANT: "BSIsActiveModifier",
    MGS[0]: "hkbModifierGenerator",
    MGS[1]: "hkbModifierGenerator",
    MGS[2]: "hkbModifierGenerator",
    MGS[3]: "hkbModifierGenerator",
}
for name, cls in EXPECT_CLASS.items():
    check(defined.get(name) == cls, f"#{name}: expected class {cls}, got {defined.get(name)}")

for name in MGS:
    text = (SHTB / "magicbehavior" / f"#{name}.txt").read_text()
    check('<hkparam name="generator">#0088</hkparam>' in text,
          f"#{name}: does not wrap vanilla #0088")
    check(f'<hkparam name="modifier">#{PLANT}</hkparam>' in text,
          f"#{name}: does not apply #{PLANT}")

WIRING = {
    "#0926.txt": ("MagicCastingLocomotionState", "#0923", f"#{MGS[0]}"),
    "#0930.txt": ("MagicCast_Standing", "#0088", f"#{MGS[1]}"),
    "#0965.txt": ("MagicCast_TurnLeft_State", "#0961", f"#{MGS[2]}"),
    "#0998.txt": ("MagicCast_TurnRight_State", "#0996", f"#{MGS[3]}"),
}
for fname, (state_name, vanilla, node) in WIRING.items():
    p = SHTB / "magicbehavior" / fname
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

# ---------------------------------------------------------------- 7. the plant / variables
bset = (SHTB / "magicbehavior" / f"#{BIND}.txt").read_text()
order = re.findall(r"variableIndex\">\$variableID\[(\w+)\]\$", bset)
check(order == ["bAllowRotation"], f"binding set changed: {order}")
check('<hkparam name="bindings" numelements="1">' in bset,
      "binding set numelements does not match its one binding")

plant = (SHTB / "magicbehavior" / f"#{PLANT}.txt").read_text()
inverts = re.findall(r'bInvertActive(\d)">(\w+)<', plant)
check(inverts == [(str(i), "false") for i in range(5)], f"invert flags changed: {inverts}")
actives = re.findall(r'bIsActive(\d)">(\w+)<', plant)
check(actives == [(str(i), "false") for i in range(5)], f"bIsActive flags changed: {actives}")

decl = (SHTB / "magicbehavior" / "#0077.txt").read_text()
check("<hkcstring>bAllowRotation</hkcstring>" in decl, "#0077.txt: bAllowRotation not declared")
var_blocks = re.findall(
    rf"<!-- MOD_CODE ~{CODE}~ OPEN -->(.*?)<!-- CLOSE -->",
    decl.replace("\r\n", "\n"),
    re.S,
)
check(any("<hkcstring>bAllowRotation</hkcstring>" in b for b in var_blocks),
      "#0077.txt: bAllowRotation is not inside a shtb MOD_CODE block")

info79 = (SHTB / "magicbehavior" / "#0079.txt").read_text().replace("\r\n", "\n")
var_info = info79.split('<hkparam name="variableInfos"')[1].split('<hkparam name="eventInfos"')[0]
check(f"<!-- MOD_CODE ~{CODE}~ OPEN -->" in var_info,
      "#0079.txt: variableInfos has no shtb MOD_CODE block")
block79 = var_info.split(f"<!-- MOD_CODE ~{CODE}~ OPEN -->")[-1].split("<!-- CLOSE -->")[0]
check("<hkparam name=\"type\">VARIABLE_TYPE_BOOL</hkparam>" in block79,
      "#0079.txt: commitment variableInfos block is not a BOOL")

info78 = (SHTB / "magicbehavior" / "#0078.txt").read_text()
block78 = info78.split(f"<!-- MOD_CODE ~{CODE}~ OPEN -->")[-1].split("<!-- CLOSE -->")[0]
check("<hkparam name=\"value\">0</hkparam>" in block78,
      "#0078.txt: wordVariableValues block is not a zero default")

# ---------------------------------------------------------------- report
print(f"{checks} checks, {len(failures)} failure(s)")
for f in failures:
    print(f"  FAIL {f}")
sys.exit(1 if failures else 0)
