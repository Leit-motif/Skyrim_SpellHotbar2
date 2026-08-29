"""Static validation for ticket 33 (`shcc` Nemesis patch + its FOMOD option).

Checks, in order:
  1. every shcc file is ASCII, BOM-free, CRLF-terminated, and ends with a newline;
  2. MOD_CODE / ORIGINAL / CLOSE markers are balanced and correctly ordered, and every
     MOD_CODE block in a vanilla-node file carries this mod's own code (`shcc`);
  3. every file's `<hkobject name="#X">` matches its filename, and hkobject open/close tags
     balance;
  4. every `#shcc$N` referenced anywhere in the patch is defined by one of its own files,
     and every defined node is referenced;
  5. the six target states are wired to the nodes the ticket says they should be;
  6. the FOMOD ModuleConfig.xml the python script generates is well-formed XML, contains
     the new group gated on MSCO.esp, and installs nothing else;
  7. the FOMOD staging globs actually resolve to the whole shcc payload.

Run:  python .scratch/shcc-build/validate_shcc.py
"""

from __future__ import annotations

import re
import sys
import xml.etree.ElementTree as ET
from glob import glob
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SHCC = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shcc"
SCRATCH = Path(__file__).resolve().parent

CODE = "shcc"

failures: list[str] = []
checks = 0


def check(ok: bool, label: str) -> bool:
    global checks
    checks += 1
    if not ok:
        failures.append(label)
    return ok


# ---------------------------------------------------------------- 1. bytes / encoding
files = sorted(p for p in SHCC.rglob("*") if p.is_file())
check(len(files) == 15, f"expected 15 shcc files, found {len(files)}")

for p in files:
    raw = p.read_bytes()
    rel = p.relative_to(REPO)
    check(not raw.startswith(b"\xef\xbb\xbf"), f"{rel}: BOM")
    check(all(b < 128 for b in raw), f"{rel}: non-ASCII byte")
    check(raw.endswith(b"\r\n"), f"{rel}: missing trailing CRLF")
    check(raw.count(b"\n") == raw.count(b"\r\n"), f"{rel}: bare LF present")

# ---------------------------------------------------------------- 2. MOD_CODE markers
MARKER = re.compile(r"^<!-- (?:MOD_CODE ~(\w+)~ OPEN|(ORIGINAL)|(CLOSE)) -->$")

node_files = sorted((SHCC / "magicbehavior").glob("*.txt"))
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
check(len(own) == 5, f"expected 5 new nodes, found {len(own)}: {sorted(own)}")

# ---------------------------------------------------------------- 5. the wiring
EXPECT_CLASS = {
    f"{CODE}$0": "hkbVariableBindingSet",
    f"{CODE}$1": "BSIsActiveModifier",
    f"{CODE}$2": "hkbModifierList",
    f"{CODE}$3": "hkbModifierGenerator",
    f"{CODE}$4": "hkbModifierGenerator",
}
for name, cls in EXPECT_CLASS.items():
    check(defined.get(name) == cls, f"#{name}: expected class {cls}, got {defined.get(name)}")

# The six target states, by the node the plant is attached through.
WIRING = {
    "#0184.txt": ("MRh_AimedConcentration", f"#{CODE}$1"),   # vanilla hkbModifierList append
    "#0489.txt": ("MLh_AimedConcentration", f"#{CODE}$1"),   # vanilla hkbModifierList append
    "#0131.txt": ("MRh_SelfConcentration", f"#{CODE}$2"),    # modifier pointer -> wrapping list
    "#0440.txt": ("MLh_SelfConcentration", f"#{CODE}$2"),    # modifier pointer -> wrapping list
    "#0317.txt": ("DualMagic_SelfConcentration", f"#{CODE}$3"),   # generator -> new MG
    "#0337.txt": ("DualMagic_AimedConcentration", f"#{CODE}$4"),  # generator -> new MG
}
for fname, (_state, node) in WIRING.items():
    p = SHCC / "magicbehavior" / fname
    check(p.exists(), f"missing target patch {fname}")
    if p.exists():
        check(node in p.read_text(), f"{fname}: does not reference {node}")

# The plant must bind bAllowRotation without inverting it, or the caster freezes facing
# the wrong way (ticket 33 acceptance).
plant = (SHCC / "magicbehavior" / f"#{CODE}$1.txt").read_text()
bset = (SHCC / "magicbehavior" / f"#{CODE}$0.txt").read_text()
order = re.findall(r"variableIndex\">\$variableID\[(\w+)\]\$", bset)
check(order == ["bAnimationDriven", "bAllowRotation", "HKSMoveON", "bHeadTrackSpine"],
      f"binding order changed: {order}")
inverts = re.findall(r'bInvertActive(\d)">(\w+)<', plant)
check(inverts == [("0", "false"), ("1", "false"), ("2", "false"), ("3", "true"), ("4", "false")],
      f"invert flags changed: {inverts}")

# Variables the plant binds that vanilla magicbehavior does not define must be declared.
decl = (SHCC / "magicbehavior" / "#0077.txt").read_text()
block = decl.split("<!-- MOD_CODE ~shcc~ OPEN -->")[-1].split("<!-- CLOSE -->")[0]
for v in ("bAnimationDriven", "bAllowRotation", "HKSMoveON"):
    check(f"<hkcstring>{v}</hkcstring>" in block, f"#0077.txt: {v} not declared")
check("bHeadTrackSpine" not in block, "#0077.txt: bHeadTrackSpine redeclared (it is vanilla)")
# variableNames / wordVariableValues / variableInfos additions must be the same length.
counts = {
    "#0077.txt": len(re.findall(r"<hkcstring>", block)),
    "#0078.txt": len(re.findall(r"<hkobject>", (SHCC / "magicbehavior" / "#0078.txt").read_text()
                                .split("<!-- MOD_CODE ~shcc~ OPEN -->")[-1].split("<!-- CLOSE -->")[0])),
    "#0079.txt": len(re.findall(r"<hkparam name=\"type\">", (SHCC / "magicbehavior" / "#0079.txt").read_text()
                                .split("<!-- MOD_CODE ~shcc~ OPEN -->")[-1].split("<!-- CLOSE -->")[0])),
}
check(len(set(counts.values())) == 1 and set(counts.values()) == {3},
      f"variable-table additions disagree: {counts}")

# ---------------------------------------------------------------- 6. FOMOD XML
sys.path.insert(0, str(REPO / "python_scripts"))
import create_fomod_installer as cfi  # noqa: E402

cfi._add_spell_pack("vulcano")
cfi._add_perk_overhaul("Vanilla/Vokrii", None, [], cfi.DualCastPerkConfig.VANILLA)
packs = [(s[0], cfi._get_spell_pack_folder_name(0, s[0]), s[1], s[2]) for s in cfi.spell_packs]
xml_text = cfi._get_module_config_xml("0.0.14", packs)

out = SCRATCH / "ModuleConfig.generated.xml"
out.write_text(xml_text, encoding="utf-8")
try:
    root = ET.fromstring(xml_text)
    check(True, "ModuleConfig.xml parses")
except ET.ParseError as exc:
    check(False, f"ModuleConfig.xml is not well-formed: {exc}")
    root = None

if root is not None:
    groups = {g.get("name"): g for g in root.iter("group")}
    check("Combat Behavior" in groups, "Combat Behavior group missing")
    g = groups.get("Combat Behavior")
    if g is not None:
        check(g.get("type") == "SelectAny", f"group type is {g.get('type')}, want SelectAny")
        plugins = g.findall(".//plugin")
        check(len(plugins) == 1, f"expected 1 plugin in the group, found {len(plugins)}")
        pl = plugins[0]
        folders = pl.findall("./files/folder")
        check(len(folders) == 1 and folders[0].get("source") == cfi.rooted_concentration_mod_folder,
              "group does not install exactly the shcc staging folder")
        check(pl.find("./files/file") is None, "group installs loose files as well as the folder")
        check(pl.find(".//defaultType").get("name") == "Optional",
              "group is not unselected by default")
        dep = pl.find(".//fileDependency")
        check(dep is not None and dep.get("file") == "MSCO.esp" and dep.get("state") == "Active",
              "group is not gated on MSCO.esp being Active")
        check(pl.find(".//patterns/pattern/type").get("name") == "Recommended",
              "MSCO.esp pattern does not raise the option to Recommended")
    # the option must not have leaked into conditionalFileInstalls
    cond = ET.tostring(root.find("conditionalFileInstalls"), encoding="unicode")
    check(cfi.rooted_concentration_mod_folder not in cond,
          "shcc folder appears in conditionalFileInstalls (would install when unselected)")

# ---------------------------------------------------------------- 7. staging globs
staged: set[str] = set()
for pattern, (rel_root, arc) in cfi.rooted_concentration_files:
    for f in glob(str(pattern), recursive=True):
        staged.add(str(Path(arc) / Path(f).relative_to(rel_root)))
expected = {str(Path(cfi.rooted_concentration_mod_folder) / p.relative_to(REPO / "nemesis"))
            for p in files}
check(staged == expected,
      f"staging globs miss {sorted(expected - staged)} / add {sorted(staged - expected)}")

# ---------------------------------------------------------------- report
print(f"{checks} checks, {len(failures)} failure(s)")
for f in failures:
    print(f"  FAIL {f}")
print(f"generated FOMOD written to {out.relative_to(REPO)}")
sys.exit(1 if failures else 0)
