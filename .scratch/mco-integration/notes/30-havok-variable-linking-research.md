# Ticket 30 — Havok Behavior nested-graph variable semantics

Research only. No code changed. All claims below are cited to a file+line actually read, or a URL
actually fetched. Where the trail runs out, that is stated explicitly rather than guessed.

## (a) Sources found and their provenance

**Local, on this machine:**

1. CommonLibSSE-NG (vcpkg-installed headers, the actual SKSE-plugin-facing API this repo builds
   against) at
   `C:\Nolvus\Projects\spell-hotbar-2\skse_plugin\build\release\vcpkg_installed\x64-windows-static-md\include\RE\`:
   - `H\hkbBehaviorGraph.h` — struct layout of the live graph instance (variableValueSet,
     variableIDMap, eventIDMap, attributeIDMap, characterPropertyIDMap, VariableMode enum).
   - `H\hkbBehaviorGraphData.h`, `H\hkbBehaviorGraphStringData.h` — the per-graph constant/template
     data (variableInfos, variableBounds, variableInitialValues, variableNames).
   - `H\hkbBindable.h`, `H\hkbNode.h` — confirms variable *bindings* (e.g. a state machine's
     startStateID bound to a variable) live on `hkbBindable::variableBindingSet`, i.e. per-node,
     not per-graph-name.
   - `H\hkbStateMachine.h` — `startStateID` is a plain int32 member (offset 0x068); the binding
     that ties it to a named variable is not stored on the state machine itself, it comes from the
     node's `hkbBindable::variableBindingSet`.
   - `H\hkbContext.h`, `H\hkbGenerator.h` — confirms `Activate`/`Update`/`Deactivate` vtable slots
     exist per node and that `hkbBehaviorGraph` itself overrides `Activate`/`Update`/`Deactivate`
     (it *is* a node from its parent's point of view).
   - `B\BShkbAnimationGraph.h` — the actual SKSE entry points: `SetGraphVariableInt`,
     `GetGraphVariableInt`, etc., all declared as members of `BShkbAnimationGraph`, all taking a
     `BSFixedString` name, all relocation-thunked into the game binary. `BShkbAnimationGraph` has
     exactly **one** `hkbBehaviorGraph* behaviorGraph` member (offset 0x208) — there is no
     collection of nested graph pointers reachable from this object.
   - `RE\Offsets_RTTI.h` / `Offsets_VTABLE.h` — confirms `hkbBehaviorReferenceGenerator` (line 5305)
     and `hkbSymbolIdMap` (line 5409) exist as real RTTI'd classes in the shipped game binary, even
     though CommonLibSSE-NG ships no struct-layout header for either — i.e. no local source gives
     their member layout or the body of any of their methods. This is a real, confirmed gap: the
     vcpkg install has **no `.cpp` implementation anywhere**, only headers describing memory layout
     and vtable slot order (CommonLibSSE-NG is a reverse-engineered structural binding, not a
     reimplementation).
   - `H\hkbCharacterSetup.h` — shows `hkbSymbolIdMap` used as `characterPropertyIdMap`, i.e.
     confirmed (locally) to be a *character-property* ID-remapping object, not obviously a
     variable-value store.
   - Searched but not present locally: `hkbBehaviorReferenceGenerator.h`, `hkbVariableValueSet.h`,
     `hkbVariableBindingSet.h`, `hkbSymbolIdMap.h` (struct bodies), any Havok SDK checkout, any
     `.cpp` for CommonLibSSE-NG's Havok classes. Checked `C:\Tools`, `C:\Nolvus` (all sibling
     projects), `C:\Users\Rando` broadly — no Havok Behavior SDK tree, no decompilation project,
     and the vcpkg overlay port (`skse_plugin\CommonLibSSE-NG_fsreg\ports\commonlibsse-ng\`) is
     only a portfile registry, not a source checkout.

**Web, fetched directly (raw file content, not summarized-only where noted):**

2. **`Bewolf2/projectanarchy`** on GitHub — the open-sourced *Havok SDK* that shipped inside
   Intel/Havok's free "Project Anarchy" mobile engine (build stamped `#20130624`, i.e. Havok 2013).
   This is genuine Havok Behavior SDK source with the real Havok license header on every file
   (`Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok`), not a
   Bethesda-specific or reverse-engineered artifact. Files fetched and read in full (saved locally
   under
   `C:\Users\Rando\AppData\Local\Temp\claude\...\scratchpad\havok_src\` for the record):
   - `Source/Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h` (844 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h
   - `Source/Behavior/Behavior/Generator/BehaviorReference/hkbBehaviorReferenceGenerator.h`
     (107 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/Generator/BehaviorReference/hkbBehaviorReferenceGenerator.h
   - `Source/Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphData.h` (122 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphData.h
   - `Source/Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphStringData.h` (67 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/BehaviorGraph/hkbBehaviorGraphStringData.h
   - `Source/Behavior/Behavior/Variables/hkbVariableValueSet.h` (136 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/Variables/hkbVariableValueSet.h
   - `Source/Behavior/Behavior/Variables/hkbVariableInfo.h` (82 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/Variables/hkbVariableInfo.h
   - `Source/Behavior/Behavior/Linker/hkbSymbolIdMap.h` (60 lines) —
     https://raw.githubusercontent.com/Bewolf2/projectanarchy/master/Source/Behavior/Behavior/Linker/hkbSymbolIdMap.h
   - Tried and **not found** (404): `hkbBehaviorGraph.cpp`, `hkbBehaviorReferenceGenerator.cpp`,
     `hkbBehaviorGraphData.cpp`, `hkbBehaviorLinkingUtils.h/.cpp`, `hkbSymbolLinker.h`. The public
     repo appears to ship public headers only — every method *body* (the actual algorithm for
     `activate()`, `copyVariablesToMembersRoot()`, the linking utilities) is compiled/private and
     not in this open-source drop. This is the hard ceiling on what "real source" can answer here.

3. **`BrannigansLaw/Skyrim-Behavior-Editor-`** — a Skyrim-specific hkx-xml behavior file
   editor/parser (targets Havok version string `hk_2010.2.0-r1`, i.e. the exact Havok build Skyrim
   LE/SE's `.hkx`/`.hkb` files declare — a different Havok point release than the 2013 SDK above,
   noted as a caveat). Fetched and read in full:
   - `src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h` (34 lines) —
     https://raw.githubusercontent.com/BrannigansLaw/Skyrim-Behavior-Editor-/master/src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h
   - `src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.cpp` (128 lines) —
     https://raw.githubusercontent.com/BrannigansLaw/Skyrim-Behavior-Editor-/master/src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.cpp
   This shows the exact serialized field set Skyrim's own `.hkb` XML uses for a
   `hkbBehaviorReferenceGenerator` node: `variableBindingSet`, `userData`, `name`, `behaviorName`.
   Nothing else. No per-node "which variables are shared with the parent" list.

4. Dead ends (found via web search, fetched, contained nothing usable): the "Skyrim Behavior
   Modding Guide" Google Doc (explicitly WIP, confirmed via WebFetch to contain no content on
   `hkbBehaviorReferenceGenerator` internals, variable syncing, or `VariableMode`) and
   nexusmods.com/skyrim/articles/50508 ("Notes on Behavior Editing", 403 Forbidden to WebFetch —
   could not be read). Both are noted so the caller doesn't re-tread them expecting new signal.

## (b) Mechanism per question, with citations

### Q1 — How are nested-graph variables linked to the parent's? What does `activate()` do? What does DISCARD_WHEN_INACTIVE do?

Confirmed from the Havok 2013 SDK source (`hkbBehaviorGraph.h`):

- Every `hkbBehaviorGraph` instance — root **and** every nested/referenced graph — owns its own
  private, non-serialized variable storage: `hkbVariableValueSet* m_variableValueSet; //+nosave`
  (hkbBehaviorGraph.h:640), sized from its own template's
  `hkbBehaviorGraphData::m_variableInfos` (hkbBehaviorGraphData.h:52). There is exactly one
  `hkbVariableValueSet` per graph *instance*, not one shared set for the whole tree.
- `VariableMode` has exactly two values, and the doc comment gives the exact semantics
  (hkbBehaviorGraph.h:280-290):
  ```
  VARIABLE_MODE_DISCARD_WHEN_INACTIVE = 0,
      // Throw away the variable values and memory on deactivate().
      // In this mode, variable memory is allocated and variable values are
      // reset each time activate() is called.
  VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE,
      // Don't discard the variable memory on deactivate(), and don't
      // reset the variable values on activate() (except the first time).
  ```
  So under `DISCARD_WHEN_INACTIVE` (value 0 — matches the ticket's description, and matches
  CommonLibSSE-NG's `VariableMode::kDiscardWhenActive = 0` at
  `hkbBehaviorGraph.h:19` in the local vcpkg header, even though the CommonLibSSE-NG enumerator's
  *name* looks backwards versus the real SDK's — the *value* is what matters and it lines up),
  **every time the nested graph is reactivated its variable memory is thrown away and reset to the
  template's declared defaults** (`hkbBehaviorGraphData::m_variableInitialValues`,
  hkbBehaviorGraphData.h:64). Nothing about a previous live value survives a deactivate/activate
  cycle on a DISCARD_WHEN_INACTIVE graph, by design, from the SDK's own comment — not an inference.
- `activate()`'s own doc comment (hkbBehaviorGraph.h:64-68): "Call activate() on all of the active
  nodes in the behavior. The nodes are activated in parent-before-child order, because a node must
  be activated before it knows which children are active." This applies recursively: a nested
  `hkbBehaviorGraph` is itself a node from its parent's perspective (it overrides
  `hkbNode::Activate`, confirmed locally in `hkbBehaviorGraph.h:28` in the CommonLibSSE-NG header
  and in the SDK header at hkbBehaviorGraph.h:68), and its own internal root generator (which may
  be a state machine) is activated *after* the nested graph itself, per the same parent-before-child
  rule applied one level down.
- **Linking (name → index) is explicitly a separate, prior step**, not something `activate()` does
  itself. `hkbBehaviorGraphStringData.h:16-18`: "These are symbol names that can be used to link the
  behavior to characters and other behaviors using an **hkbSymbolLinker**." `getVariableValueWord()`
   / `setVariableValueWord()` on `hkbBehaviorGraph` both say (hkbBehaviorGraph.h:112-115): "You
  should pass in the **external index** of the variable that results from **the linking process**.
  See hkbSymbolLinker, hkbBehaviorLinkingUtils::linkBehavior(), and the section of the manual
  entitled Linking Behaviors." `hkbBehaviorReferenceGenerator.h:24-27` repeats the same requirement
  for referenced graphs specifically: "You need to make sure to go through the linking process so
  that the referencing behavior graph and the referenced behavior graph can communicate events,
  attributes, and variables." So variable linkage between a parent and a nested graph is
  **name-based**, resolved once by a linking pass (`hkbBehaviorLinkingUtils::linkBehavior()` /
  `linkBehaviors()`), which populates per-graph ID-map objects (below) — it is *not* something that
  happens fresh on every `activate()` call, and it is *not* automatic just because two graphs
  declare the same string.
- `hkbSymbolIdMap.h:12-17` gives the map's exact job: "Some objects like hkbBehaviorGraph and
  hkbSequence can contain their own indexed lists of symbols such as event names, variable names,
  etc. An hkbSymbolIdMap maintains a map from **the local IDs of such an object**, and **the IDs in
  a global list or in another object**." Every `hkbBehaviorGraph` carries four of these:
  `m_eventIdMap`, `m_attributeIdMap`, `m_variableIdMap`, `m_characterPropertyIdMap`
  (hkbBehaviorGraph.h:628-637) — confirmed matching, field-for-field and in the same order, in the
  local CommonLibSSE-NG struct layout (`eventIDMap` 0xB8, `attributeIDMap` 0xC0, `variableIDMap`
  0xC8, `characterPropertyIDMap` 0xD0 in `hkbBehaviorGraph.h` under vcpkg). Crucially, the SDK also
  caches a **flattened** version of this specifically toward the root:
  `hkArray<hkInt32> m_internalToRootVariableIdMap; //+nosave` (hkbBehaviorGraph.h:668), built by
  `initCachedIdMaps( hkbCharacter& character, hkbBehaviorGraph& rootBehaviorGraph )`
  (hkbBehaviorGraph.h:779) and exposed via `getInternalToRootVariableIdMap()` (hkbBehaviorGraph.h:577).
  This is the concrete, named artifact that ties a *nested* graph's own local variable index to the
  corresponding index in the **root** graph's own variable list, when the same name is declared in
  both. Note it is explicitly root-relative, not parent-relative — an index cached against the true
  root graph, however many levels of nesting sit in between.
- **Where the SDK's public headers stop**: neither `hkbBehaviorGraph.cpp` nor
  `hkbBehaviorReferenceGenerator.cpp` nor the linking-utility source is published (all four 404'd on
  the same GitHub repo). So the *exact* runtime use of `m_internalToRootVariableIdMap` — i.e.
  whether it is consulted to copy a value between two separate `hkbVariableValueSet` instances, and
  if so at what call site and how often — is **not confirmed by any source found**. See gap list in
  (d).

### Q2 — What does `BShkbAnimationGraph::SetGraphVariableInt`/`GetGraphVariableInt` actually touch?

From `BShkbAnimationGraph.h` (local, vcpkg CommonLibSSE-NG header), lines 79-119 and 121-150:
- Both functions are members of `BShkbAnimationGraph`, take only `(name, value)`, and are pure
  relocation thunks into the game binary (`REL::Relocation<func_t> func{ RELOCATION_ID(...) }`) —
  no source for the function *body* exists locally or in any source found on the web (the game
  binary is closed; CommonLibSSE-NG only pins the call site, not the algorithm).
- `BShkbAnimationGraph` has **exactly one** `hkbBehaviorGraph* behaviorGraph;` member (offset
  `0x208`, BShkbAnimationGraph.h:134). There is no array, list, or map of nested graphs on this
  object. This one field is what Skyrim calls its "root"/"0_master" graph.
- Given (Q1) that variable *access* on any `hkbBehaviorGraph` is by *index*, resolved through that
  graph's own name tables (`hkbBehaviorGraphStringData::m_variableNames`) and its own
  `m_variableIdMap`, and given `BShkbAnimationGraph` exposes no path to any nested graph object at
  all, the only structurally consistent reading is: `SetGraphVariableInt`/`GetGraphVariableInt`
  resolve the name against the **root** graph's own string/variable tables and read/write the
  **root** graph's own `hkbVariableValueSet` (offset `0xD8` on `hkbBehaviorGraph`, per the local
  CommonLibSSE-NG `hkbBehaviorGraph.h:59`). This is a structural inference from the two headers
  together (no single source states "SetGraphVariableInt only touches the root"), flagged
  accordingly — the actual resolution algorithm inside the relocation target is not visible from
  any source read.

### Q3 — Do root writes reach a doubly-nested graph's own storage, and when? Does direction matter?

- Structurally: root and nested graphs have **separate** `hkbVariableValueSet` instances
  (Q1/hkbBehaviorGraph.h:640). A write into the root's set is not automatically the same memory
  cell as the nested graph's set for "the same-named" variable.
- The one confirmed connective-tissue artifact is `m_internalToRootVariableIdMap`
  (hkbBehaviorGraph.h:668/779) — it exists specifically to let a nested graph's internal variable
  index be resolved against the root graph's index space. Its existence is real, confirmed SDK
  source. Its *consumer* (what code reads it, and when) is not published anywhere found — so I can
  confirm the *plumbing for a root→nested (or nested→root) copy to be possible* exists, but I
  cannot confirm from source that such a copy actually executes on every frame, only at activate,
  or ever automatically for a *variable* (as opposed to an *event* or *character property*, which
  have their own separate maps and possibly separate propagation rules the SDK headers don't
  distinguish either).
- Directionality is asymmetric in the ticket's measured facts (root→nested appears to work for 1H
  once declared at root; nested→root never appears to work for `MCO_currentattack`). Nothing found
  in the SDK headers documents an intentional asymmetry for ordinary (non-output, non-role-flagged)
  variables — `hkbVariableInfo::m_role` (a `hkbRoleAttribute`, hkbVariableInfo.h:60-61) is the one
  field that could plausibly encode a directional/role distinction (e.g. an "output" vs "input"
  role used by hkbBehaviorLinkingUtils to decide sync direction), but `hkbRoleAttribute`'s own
  definition was not fetched/found in this pass (declared via `#include
  <Behavior/Behavior/Attributes/hkbAttributes.h>`, not read) — this is a concrete, named lead for a
  follow-up read rather than a closed question.

### Q4 — Activation order: does variable sync happen before or after the nested state machine reads its startStateId binding?

- From the `activate()` doc comment (hkbBehaviorGraph.h:64-68, quoted above), activation is
  strictly **parent-before-child**, and this applies recursively down through a nested
  `hkbBehaviorGraph` (itself a node) into its own internal state machine (a child node within it).
  So *if* any root→nested variable value copy happens as part of the nested `hkbBehaviorGraph`'s own
  `activate()` (as opposed to happening once, earlier, only at *link* time), it structurally has to
  happen **before** the nested graph's own child nodes — including the state machine that reads
  `startStateID`'s binding — are activated, simply because the nested graph node itself must finish
  activating (and, per `DISCARD_WHEN_INACTIVE`'s own doc comment, that is exactly the moment its
  variable memory gets allocated and reset to defaults, hkbBehaviorGraph.h:282-284) before it can
  activate its children.
- This is consistent with, and gives a plausible mechanical account for, the 1H fix: once
  `MCO_nextattack` was declared at the true root (0_master), the SDK-documented "reset to defaults
  each activate()" step for the nested `MCO_Attack.hkb` graph could plausibly pull its default from
  a value linked to the root's namespace rather than the nested graph's own static template default
  — but **no source found actually shows the default-reset step consulting anything other than
  `hkbBehaviorGraphData::m_variableInitialValues`** (hkbBehaviorGraphData.h:64, described only as
  "The initial values of the variables", with no mention of an override from a linked root value).
  This is the single biggest unresolved point in the whole investigation: whether "reset to default
  on activate" and "sync from a linked root variable" are the same mechanism, two mechanisms that
  both run at activate, or actually two different explanations where the true fix mechanism is
  something else entirely (e.g. `startStateID`'s own variable binding resolving through the state
  machine's *own* `variableBindingSet`, whose target index was only made valid — i.e., link
  succeeded instead of silently no-op'ing — once the name existed at root). Flagged explicitly as
  not settled by source.

## (c) Which measured facts each mechanism claim explains or contradicts

| Measured fact | Explained by sourced mechanism? |
|---|---|
| Nested `MCO_Attack.hkb`'s `MCO_nextattack` binding has index 230 in its own data, vs index 106 in `1hm_behavior` | **Explained.** Each `hkbBehaviorGraph` (root, `1hm_behavior`, `MCO_Attack`) has its own `hkbBehaviorGraphData`/`hkbBehaviorGraphStringData` (hkbBehaviorGraphData.h:52, hkbBehaviorGraphStringData.h:37) — variable indices are local to each graph's own declared list, never a shared global index space. Different indices for the same name in different graphs is exactly what the data model predicts. |
| Before root declared the name, SKSE writes were "readable back" but the nested machine always fell back to its first state | **Partially explained, one gap flagged.** `BShkbAnimationGraph::Set/GetGraphVariableInt` only ever reach the root's own storage (Q2) — a self-consistent round trip through the *root's* own set would indeed read back whatever was last written there, independent of whether any nested graph ever saw it. Since the nested graph's own storage resets to its *own template's* default on every `DISCARD_WHEN_INACTIVE` `activate()` (hkbBehaviorGraph.h:282-284) regardless of what's in the root's set, "always falls back to its first state" is exactly what the SDK's documented reset behavior predicts *if* the linkage from nested→root was not yet established for that name. The one gap: this only fully closes if `SetGraphVariableInt` on a name absent from the root's own declared variable list still *succeeds silently* rather than failing outright — not confirmed from any source (the relocation target's body is unavailable everywhere checked). |
| Declaring the name at root fixed 1H entry, but 2H (same node-for-node graph, same single nested instance) still ignores it | **Not explained by any source found.** Every mechanism above (name-based linking, per-graph index tables, parent-before-child activation, root ID-map caching) is described as graph-topology-driven, not weapon-type-driven. Nothing in the Havok SDK headers, the Skyrim behavior-editor source, or CommonLibSSE-NG distinguishes 1H vs 2H paths through an otherwise-identical nested-graph node. This strongly suggests the actual cause is Skyrim/behavior-*content*-specific (e.g. the 2H path's `hkbBehaviorReferenceGenerator` for `MCO_Attack.hkb` sitting under a different intermediate graph that never itself got the name added, or the 2H entry's own state-machine binding pointing at a different variable/index than assumed) rather than a Havok-runtime-level asymmetry — this is the strongest argument in the whole report for needing a runtime probe rather than more source reading. |
| `MCO_currentattack`, written from inside the nested graph, always reads 0 at the root via SKSE | **Explained, with the same directionality caveat as Q3.** Root and nested graphs keep separate `hkbVariableValueSet` instances (hkbBehaviorGraph.h:640); nothing found in any source describes an automatic *nested→root* copy for ordinary variables. A write made purely inside the nested graph's own local set staying invisible to `SetGraphVariableInt`/`GetGraphVariableInt` (which only touch root storage, Q2) is exactly the predicted outcome if no such reverse sync exists — consistent with the observed asymmetry (root→nested plausibly works for 1H; nested→root never works for either weapon type). |

## (d) Open gaps a runtime probe would still need to close

1. **The actual body of `hkbBehaviorGraph::activate()`/`update()`/`copyVariablesToMembersRoot()`,
   and of `hkbBehaviorReferenceGenerator::generate()`/`updateSync()`, and of
   `hkbBehaviorLinkingUtils::linkBehavior()`/`linkBehaviors()`.** None of these method bodies were
   found in any source, local or web — the Havok 2013 SDK drop on GitHub ships headers only. This
   is the ceiling on static analysis: whether a root→nested variable copy happens continuously every
   `update()`, once at `activate()`, or not automatically at all (and is instead something Skyrim's
   own engine code does explicitly around `hkbBehaviorReferenceGenerator` instantiation) is not
   confirmed by any document read. A memory-probe (breakpoint or logged read of both graphs'
   `hkbVariableValueSet` contents across several frames while forcing an SKSE write) is the only way
   to settle this now.
2. **Why 2H behaves differently from 1H given "the same node-for-node graph path, same single
   nested instance."** No source distinguishes weapon-type paths at the Havok-runtime level; this
   is very likely explained by the actual authored `.hkb` content (which graph instantiates
   `MCO_Attack.hkb`'s `hkbBehaviorReferenceGenerator` for the 2H path, and whether the intermediate
   graph on that path also declares `MCO_nextattack`) rather than anything Havok-generic. Needs
   inspection of the actual 2H behavior file content (not attempted in this pass — this ticket was
   scoped to the generic Havok mechanism, not to re-reading the specific `.hkb` files already
   covered by the prior "Read the greatsword path" ticket work referenced in the git log).
3. **`hkbRoleAttribute` (`hkbVariableInfo::m_role`)** was found to exist
   (`Behavior/Behavior/Attributes/hkbAttributes.h`, included but not fetched) and is a plausible
   carrier of directional/sync-role information per variable. Worth a follow-up fetch if the
   direction-of-sync question needs to be pinned down from source rather than from behavior alone.
4. **Whether `SetGraphVariableInt` on a name not declared at root fails silently (no-op, returns
   false) or writes into a fallback/dead slot that only round-trips within the call itself** was
   inferred, not confirmed — no source shows the resolution algorithm.
5. Version mismatch caveat: the Havok SDK read (2) is Havok build `#20130624` (2013); the Skyrim
   Behavior Editor's own parser (3) targets `hk_2010.2.0-r1`, the actual Havok point release Skyrim
   LE/SE's `.hkb` files declare. The class-level API and data model documented here (VariableMode,
   per-graph variable value sets, hkbSymbolIdMap-based linking) is consistent across both — nothing
   found suggests these particular mechanisms changed between the two Havok releases — but this
   should be named as a version gap rather than silently assumed identical.
