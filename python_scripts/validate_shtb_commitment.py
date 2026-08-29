"""Static validation for ticket 39 -- commitment on the `shtb` cast states.

Ticket 39 replaced the flag-plant on the four fire-and-forget cast states with the
binding set MSCO's certified-correct cast uses: `bAnimationDriven` is gone, and the
commitment now comes from where those states sit in the graph (ADR-0015's 2026-08-29
amendment: the root is authored in the behavior graph, and on a state that routes
around locomotion it is the routing, not a flag).

This script mechanically asserts the four things the ticket-39 build decided, so a
later edit that quietly puts the flag back, or that reaches states it must not, fails
here instead of in the game:

  1. Neither graph's four fire-and-forget cast states bind `bAnimationDriven`. Each
     state is traced by name -> `generator` -> `_MG` modifier-generator -> `modifier`
     -> `variableBindingSet`, so the check follows the wiring rather than trusting the
     file numbering the survey happened to see.
  2. The `SH2_Channel_*` and `SH2_Art_*` chains are byte-identical to the branch point,
     end to end -- state, `_MG`, modifier, binding set, and clip -- and the four cast
     states' shared clip binding set is pinned the same way. Ticket 39's scope was the
     cast states' commitment only: the owner accepted the channel's root, and ticket 38
     deliberately left `SH2_Art_Clip` consuming animmotion while taking it off the cast
     clips. What is enforced here is byte-identity of those chains plus the wiring that
     reaches them, NOT anything about animmotion semantics -- no hkx is parsed. The
     wiring half matters as much as the hashes: a frozen node that nothing points at any
     more is still frozen, and would pass a hash-only check.
  3. Every `$variableID[...]$` name a shtb file references is declared, either by shtb
     itself or by a mod code recorded below as the provider. Binding an undeclared name
     makes Nemesis fail or silently no-op.
  4. `0_master` is untouched by the commitment work: its shtb block declares only the
     four `MCO_*` variables and `SH2_ArtSelector`, and shtb contributes no binding set
     there at all. (The `bAnimationDriven` string in `0_master/#0106.txt` is vanilla
     base content outside the shtb MOD_CODE block -- it is a declaration, not a bind.)

Run:  python python_scripts/validate_shtb_commitment.py
"""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
SHTB = REPO / "nemesis" / "Nemesis_Engine" / "mod" / "shtb"

failures: list[str] = []
checks = 0


def check(ok: bool, label: str) -> bool:
    global checks
    checks += 1
    if not ok:
        failures.append(label)
    return ok


def load(graph: str) -> dict[str, str]:
    """node name -> file text, for every node file in one graph directory."""
    out: dict[str, str] = {}
    for p in sorted((SHTB / graph).glob("*.txt")):
        text = p.read_text(encoding="utf-8")
        m = re.search(r'<hkobject name="([^"]+)"', text)
        if m:
            out[m.group(1)] = text
    return out


def param(text: str, name: str) -> str | None:
    m = re.search(r'<hkparam name="%s">([^<]*)</hkparam>' % re.escape(name), text)
    return m.group(1) if m else None


def by_name(nodes: dict[str, str], wanted: str) -> str | None:
    for node, text in nodes.items():
        if param(text, "name") == wanted:
            return node
    return None


GRAPHS = ("1hm_behavior", "magicbehavior")
FF_STATES = ("SH2_CastRight_State", "SH2_Cast2_State", "SH2_Cast3_State", "SH2_Cast4_State")

# The binds the ticket-39 build settled on, in order. MSCO's certified cast state
# (`#msco$30`/`#msco$31`, MSCO_IAM_LR) binds bAllowRotation uninverted and writes
# magicbehavior variable index 65 (= bHeadTrackSpine) inverted; it binds no
# bAnimationDriven. `HKSMoveON` is Hot Key Skill bookkeeping carried over from the
# template these states were copied from -- write-only, read by no condition in any
# installed graph -- and is left alone so this change moves exactly one variable.
EXPECTED_BINDS = ["bAllowRotation", "HKSMoveON", "bHeadTrackSpine"]
EXPECTED_INVERTS = [False, False, True, False, False]

# Renamed off `bAnimationDrivenIsActiveModifier` by ticket 39. `name` is a debug string
# with no consumer, so nothing breaks if it drifts -- but a modifier named after a
# variable it no longer touches is exactly the trap the rename removed, and a name-only
# revert is the cheapest way for this patch to start lying about itself.
EXPECTED_MODIFIER_NAME = "SH2_CastCommitIsActiveModifier"

nodes_by_graph = {g: load(g) for g in GRAPHS}

# ------------------------------------------------ 1. the four FF cast states per graph
for graph in GRAPHS:
    nodes = nodes_by_graph[graph]
    for state_name in FF_STATES:
        state = by_name(nodes, state_name)
        if not check(state is not None, f"{graph}: state {state_name} not found"):
            continue
        mg = param(nodes[state], "generator")
        if not check(mg in nodes, f"{graph}/{state_name}: generator {mg} not in this graph"):
            continue
        mg_name = param(nodes[mg], "name") or ""
        check(mg_name.endswith("_MG"),
              f"{graph}/{state_name}: generator {mg} is {mg_name!r}, not a _MG modifier-generator")
        mod = param(nodes[mg], "modifier")
        if not check(mod in nodes, f"{graph}/{mg_name}: modifier {mod} not in this graph"):
            continue
        bset = param(nodes[mod], "variableBindingSet")
        if not check(bset in nodes, f"{graph}/{mg_name}: binding set {bset} not in this graph"):
            continue

        binds = re.findall(r'variableIndex">\$variableID\[(\w+)\]\$', nodes[bset])
        check("bAnimationDriven" not in binds,
              f"{graph}/{state_name} ({mg_name} -> {mod} -> {bset}): binds bAnimationDriven")
        check(binds == EXPECTED_BINDS,
              f"{graph}/{state_name} ({bset}): binds {binds}, expected {EXPECTED_BINDS}")

        slots = re.findall(r'memberPath">bIsActive(\d)</hkparam>', nodes[bset])
        check(slots == [str(i) for i in range(len(binds))],
              f"{graph}/{state_name} ({bset}): bIsActive slots {slots} are not 0..{len(binds) - 1}")
        check(f'numelements="{len(binds)}"' in nodes[bset],
              f"{graph}/{state_name} ({bset}): numelements disagrees with {len(binds)} bindings")

        mod_name = param(nodes[mod], "name")
        check(mod_name == EXPECTED_MODIFIER_NAME,
              f"{graph}/{state_name} ({mod}): modifier is named {mod_name!r}, "
              f"expected {EXPECTED_MODIFIER_NAME!r}")

        inverts = [v == "true" for _, v in
                   sorted(re.findall(r'bInvertActive(\d)">(\w+)<', nodes[mod]))]
        check(inverts == EXPECTED_INVERTS,
              f"{graph}/{state_name} ({mod}): invert flags {inverts}, expected {EXPECTED_INVERTS}")
        actives = re.findall(r'bIsActive(\d)">(\w+)<', nodes[mod])
        check(all(v == "false" for _, v in actives),
              f"{graph}/{state_name} ({mod}): a bIsActive default is not false")

# --------------------------------- 2. Channel, Art, and the cast clip sets frozen whole
# sha256 of each file with CRLF normalised to LF, as of `5b29e96` -- the commit this
# branch forked from. (NOT `123ecf4`, the ticket-claim commit on top of it; that one
# touches only the ticket markdown, so the two agree on every file below, but the branch
# point is the honest baseline to name.)
#
# Each Channel/Art chain is pinned end to end: state -> _MG -> modifier -> binding set,
# plus the clip the _MG generates. Pinning only the set and the modifier would let a
# rewire of SH2_Channel_MG to a fresh modifier, or an edit to SH2_Art_Clip, walk straight
# past this check. Also pinned: the binding set the four cast clips share, so nobody
# re-plants bAnimationDriven on the clip side of the chain instead of the state side.
FROZEN = {
    # SH2_Art chain (1hm_behavior only -- magicbehavior has no Art state)
    "1hm_behavior/#shtb$25.txt":
        "8044e3c961edfe7f550ebbfdcc780b5df041ff8f57e1ac6ca0c2cbc74a02cdc8",  # SH2_Art_State
    "1hm_behavior/#shtb$29.txt":
        "42053c452117f535c34a38d3d283bcfae29beb9feea122152e164de9a42e2c99",  # SH2_Art_MG
    "1hm_behavior/#shtb$28.txt":
        "8d3f1911e8ce93dd10a31429ec9817712dbea64af8c37c63f21c075550e35c7c",  # Art modifier
    "1hm_behavior/#shtb$27.txt":
        "c3242c7623426a1d367865d011c47c56b4fb8d5980eed174ce7d7a6bad858de4",  # Art binding set
    "1hm_behavior/#shtb$23.txt":
        "7456090638618186514a0ec6574821e3f6edb5ab20d19eedcf4610e764846bfa",  # SH2_Art_Clip
    # SH2_Channel chain, 1hm_behavior
    "1hm_behavior/#shtb$32.txt":
        "fe024e6815529515676f69424aa11da025b07c4fe19d4731a28768eb7edd8a07",  # SH2_Channel_State
    "1hm_behavior/#shtb$35.txt":
        "86832077a9125d5a6f6a52284519970ca6b1e11599c9d8b78a69b5a43e66bb00",  # SH2_Channel_MG
    "1hm_behavior/#shtb$34.txt":
        "b2235b778a729d511bb848ba40e12cc373024ecac30ca0df007ceecd10235635",  # Channel modifier
    "1hm_behavior/#shtb$33.txt":
        "03d6ae1ef02370cdc033dc08cd2b07a7bd047854574b1afcaf5e33bfc6ae823d",  # Channel binding set
    "1hm_behavior/#shtb$30.txt":
        "af34edbb923e9717faa3fd3254cfe0d50768a4ede3b6f33082083c367bd98ab7",  # SH2_Channel_Clip
    # SH2_Channel chain, magicbehavior
    "magicbehavior/#shtb$27.txt":
        "2c0e4d14dbb02ebd3b411863c07c9a7b17361cffe12888244b27648e13b4eb76",  # SH2_Channel_State
    "magicbehavior/#shtb$30.txt":
        "7de3103a68f8ab7ab3c08604bcfb86ea20d12750a7d8b6d62c4a0fad4a0b228e",  # SH2_Channel_MG
    "magicbehavior/#shtb$29.txt":
        "4791c36364889a9adf4312f84b2831c9e63f6f7b9edd111d09f423613736cedb",  # Channel modifier
    "magicbehavior/#shtb$28.txt":
        "416cd749d52489e7da698a43f5bad8271056d4ebae532240c7b536c6ab2ba3e2",  # Channel binding set
    "magicbehavior/#shtb$25.txt":
        "97c99b5f6cfc0394b444e1caef54dcb7264415dce23be0ca91fdda1cc602b187",  # SH2_Channel_Clip
    # the binding set all four cast clips share (playbackSpeed -> MSCO_attackspeed)
    "1hm_behavior/#shtb$10.txt":
        "e054c234984186553c219e1c10addac2962a169f3c6cb17dbf6b6ee1e6900be6",
    "magicbehavior/#shtb$12.txt":
        "da384066364d5700bc906971378353f662d56f65870a9cc692ab1ba9e9bfe749",
}
for rel, want in FROZEN.items():
    p = SHTB / rel
    if not check(p.is_file(), f"frozen file missing: {rel}"):
        continue
    got = hashlib.sha256(p.read_bytes().replace(b"\r\n", b"\n")).hexdigest()
    check(got == want,
          f"{rel} changed since the branch point 5b29e96 -- ticket 39 touches only the "
          f"four fire-and-forget cast states' commitment, not Channel, Art, or the clips")

# A frozen node nothing points at any more is still frozen, so re-walk the wiring too.
for graph, state_name, want_mg, want_mod, want_set, want_clip in (
    ("1hm_behavior", "SH2_Art_State", "#shtb$29", "#shtb$28", "#shtb$27", "#shtb$23"),
    ("1hm_behavior", "SH2_Channel_State", "#shtb$35", "#shtb$34", "#shtb$33", "#shtb$30"),
    ("magicbehavior", "SH2_Channel_State", "#shtb$30", "#shtb$29", "#shtb$28", "#shtb$25"),
):
    nodes = nodes_by_graph[graph]
    state = by_name(nodes, state_name)
    if not check(state is not None, f"{graph}: state {state_name} not found"):
        continue
    mg = param(nodes[state], "generator")
    mod = param(nodes.get(mg, ""), "modifier")
    bset = param(nodes.get(mod, ""), "variableBindingSet")
    clip = param(nodes.get(mg, ""), "generator")
    for got, want, role in ((mg, want_mg, "generator"), (mod, want_mod, "modifier"),
                            (bset, want_set, "binding set"), (clip, want_clip, "clip")):
        check(got == want,
              f"{graph}/{state_name} now reaches {role} {got}, not the frozen {want}")

# The four cast clips must still share the pinned clip binding set, and it must still be
# the harmless one: playbackSpeed -> MSCO_attackspeed, and nothing else.
for graph, want_set in (("1hm_behavior", "#shtb$10"), ("magicbehavior", "#shtb$12")):
    nodes = nodes_by_graph[graph]
    for state_name in FF_STATES:
        state = by_name(nodes, state_name)
        mg = param(nodes.get(state or "", ""), "generator")
        clip = param(nodes.get(mg or "", ""), "generator")
        got = param(nodes.get(clip or "", ""), "variableBindingSet")
        check(got == want_set,
              f"{graph}/{state_name}: its clip {clip} binds {got}, not the pinned {want_set}")
    text = nodes.get(want_set, "")
    paths = re.findall(r'memberPath">([^<]*)</hkparam>', text)
    varz = re.findall(r'variableIndex">\$variableID\[(\w+)\]\$', text)
    check(paths == ["playbackSpeed"] and varz == ["MSCO_attackspeed"],
          f"{graph}/{want_set}: cast clip binding set now binds {list(zip(paths, varz))}, "
          f"expected playbackSpeed -> MSCO_attackspeed only")
    check("bAnimationDriven" not in text,
          f"{graph}/{want_set}: bAnimationDriven re-planted on the cast clips' binding set")

# ------------------------------------------------ 3. every referenced variable is declared
# Variables shtb references but does not declare, and the mod code that does declare them
# into the merged graph. Verified 2026-08-29 against the installed Nemesis mod tree.
EXTERNAL = {
    "1hm_behavior": {"bAllowRotation": "vanilla", "bAnimationDriven": "vanilla",
                     "bHeadTrackSpine": "vanilla", "HKSMoveON": "hotkey"},
    "magicbehavior": {"bHeadTrackSpine": "vanilla", "bAllowRotation": "hotkey/shcr",
                      "bAnimationDriven": "hotkey", "HKSMoveON": "hotkey",
                      "MSCO_attackspeed": "msco"},
}
DECL_FILE = {"1hm_behavior": "#0085.txt", "magicbehavior": "#0077.txt"}

for graph in GRAPHS:
    decl_path = SHTB / graph / DECL_FILE[graph]
    declared = set()
    if check(decl_path.is_file(), f"{graph}: {DECL_FILE[graph]} missing"):
        text = decl_path.read_text(encoding="utf-8")
        m = re.search(r'<hkparam name="variableNames"[^>]*>(.*?)</hkparam>', text, re.S)
        if m:
            declared = set(re.findall(r"<hkcstring>\s*(\S+?)\s*</hkcstring>", m.group(1)))
    referenced = set()
    for text in nodes_by_graph[graph].values():
        referenced |= set(re.findall(r"\$variableID\[(\w+)\]\$", text))
    unknown = sorted(v for v in referenced
                     if v not in declared and v not in EXTERNAL[graph])
    check(not unknown,
          f"{graph}: variables referenced but neither declared by shtb nor recorded as "
          f"externally provided: {unknown}")
    stale = sorted(v for v in EXTERNAL[graph] if v not in referenced)
    check(not stale,
          f"{graph}: EXTERNAL lists {stale}, which shtb no longer references -- prune it")

# ------------------------------------------------ 4. 0_master untouched by this ticket
master = (SHTB / "0_master" / "#0106.txt").read_text(encoding="utf-8")
block = master.split("<!-- MOD_CODE ~shtb~ OPEN -->")[-1].split("<!-- CLOSE -->")[0]
added = re.findall(r"<hkcstring>\s*(\S+?)\s*</hkcstring>", block)
check(added == ["MCO_nextattack", "MCO_nextpowerattack", "MCO_currentattack",
                "MCO_currentpowerattack", "SH2_ArtSelector"],
      f"0_master/#0106.txt shtb block declares {added} -- ticket 39 decided to leave it alone")
check("bAnimationDriven" not in block,
      "0_master/#0106.txt: shtb's own block declares bAnimationDriven")
master_nodes = {p.name for p in (SHTB / "0_master").glob("*.txt")}
check(master_nodes == {"#0106.txt", "#0107.txt", "#0108.txt"},
      f"0_master holds {sorted(master_nodes)}; ticket 39 adds no node there")
for p in sorted((SHTB / "0_master").glob("*.txt")):
    text = p.read_text(encoding="utf-8")
    check("hkbVariableBindingSet" not in text and "BSIsActiveModifier" not in text,
          f"0_master/{p.name} now carries a binding set or modifier -- shtb binds nothing here")

# ------------------------------------------------ report
print(f"{checks} checks, {len(failures)} failure(s)")
for f in failures:
    print(f"  FAIL {f}")
sys.exit(1 if failures else 0)
