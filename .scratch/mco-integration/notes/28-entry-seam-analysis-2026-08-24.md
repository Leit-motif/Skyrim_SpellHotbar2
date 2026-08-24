# Ticket 28 — static entry-seam analysis of the winning behavior graphs, 2026-08-24

Read-only decompile of the winning behavior files. No game, no MO2 state, no mod folder touched;
`hkxc convert -v xml` was run on copies in the scratchpad.

**Headline: the "entry is hardwired, the variable can never work" hypothesis is REFUTED.** Nothing in
any winning file bypasses `startStateId` on the way into the MCO attack states. Every transition
that enters the attack subtree, at every level, carries `toNestedStateId 0` **without**
`FLAG_TO_NESTED_STATE_ID_IS_VALID`. The index is selected by the binding and only by the binding, on
the working path and the failing path alike. The failure is therefore about *when and where the
value is read*, not about a competing selector.

**Second headline, and it kills the candidate refactor: `MCO_Attack.hkx` is not Nemesis-patchable.**
Authoring `SH2_ChainAttack1..N` transitions into the attack states is not a change this repo can
make. See §6.

## Which files were read, and why they are the winners

| file | winner | evidence |
|---|---|---|
| `1hm_behavior.hkx` | `Nemesis Output` (modlist line 616) | only higher-priority mods shipping `meshes\actors\character\behaviors\1hm_behavior.hkx` are none; `TK Dodge RE` (991) and `TK Dodge` (993) lose. `MODS\overwrite\meshes` contains no `behaviors` directory at all. |
| `MCO_Attack.hkx` | `ADXP MCO 1.6.0.6 Bug Fixes` (modlist line **110**) | beats `ADXP - MCO` (1002). Not produced by Nemesis — `Nemesis Output\...\behaviors` contains 18 vanilla-named files and no `MCO_*.hkx`. |
| `0_master.hkx` | `Nemesis Output` (616) | `Jump Behavior Overhaul` (1356) loses. |

## 1. Where the attack states live, and it is ONE file for every melee weapon

`AttackNodes_StateMachine` is **not** in `1hm_behavior.hkx`. Grep for `AttackNodes` there returns
nothing. It lives in `MCO_Attack.hkx`, exactly as ticket 11 recorded:

- `#0029 hkbStateMachine 'AttackNodes_StateMachine'` (line 1759), states `AttackNodesState1..10`,
  `stateId` 1..10.
- `#0099 hkbStateMachine 'AttackNodes_StateMachine_Duplicate'` (line 2974), states
  `DuplicateAttackNodesState1..10`, `stateId` 1..10.

Both sets of clips are fixed paths — `#0035 'MCO_ClipGenerator_MCO_attack1' animationName
Animations\MCO_attack1.hkx` through `#0082 ... MCO_attack10.hkx` in the first copy, `#0102..#0129`
in the duplicate. There are **no per-weapon variants of these generators**: the same ten paths serve
every weapon, and per-weapon movesets come from OAR replacing `Animations\MCO_attackN.hkx`.

Greatsword and sword share this one graph. `1hm_behavior.xml` has exactly one behavior reference to
it, `#0863 hkbBehaviorReferenceGenerator 'MCO_BRG_Attack' -> 'Behaviors\MCO_Attack.hkx'` (line
23498), and that single object is the `generator` of **four** state infos:

- `#0862 'H2H_3rdPerson_AttackRight_State'` (line 23513)
- `#1015 '1HM_AttackRight_DefaultState'` (line 26831) — one-handed
- `#1148 'Default2HM_AttackRight'` (line 29753) — **greatsword / 2HM sword**
- `#1253 '3rdP_2HW_AttackRightState'` (line 31942) — 2H warhammer/axe

So "whichever graph hosts greatsword attacks" is the same `MCO_Attack.hkx`. The two weapons differ
only in which parent state machine reaches it (`AttackRight_1HM_Behavior` #1013 line 27319 vs
`AttackRight_2HM_SwordBehavior` #1146 line 29897) — both of those are plain
`hkbStateMachine`s with `startStateId` bound to `iIsInSneak`, both with the MCO reference at
`stateId 0`.

Power attacks and weapon arts are separate files: `#2056 'MCO_BRG_PowerAttack' ->
Behaviors\MCO_PowerAttack.hkx` (line 57447) and `#2365 'MCO_BRG WeaponArt' ->
Behaviors\MCO_WeaponArt.hkx` (line 66512).

## 2. The `startStateId` binding — confirmed, and it is on BOTH machines

```
#0030 hkbVariableBindingSet (MCO_Attack.xml line 634)
    memberPath   startStateId
    variableIndex 230
    bindingType  BINDING_TYPE_VARIABLE
```

`#0132 hkbBehaviorGraphStringData` `variableNames` has 246 entries; index **230 = `MCO_nextattack`**
(231 `MCO_nextpowerattack`, 232 `MCO_currentattack`, 233 `MCO_currentpowerattack`).

Both `#0029` and `#0099` declare `<hkparam name="variableBindingSet">#0030</hkparam>`, so the
duplicate reads the same variable. Each machine's own literal `startStateId` is `0` — a value that
matches **no** state in either machine (states are 1..10). Without the binding the machine is not
merely index-1, it is invalid; Havok falls back to the first state in the array, `AttackNodesState1`
→ `MCO_attack1`. **That fallback is exactly the greatsword symptom in note 28.**

For completeness: `1hm_behavior.xml` declares `MCO_nextattack` at variable index **106** of 154, and
**no object in `1hm_behavior` binds any of indices 106–109 to anything.** A sweep of all
`hkbVariableBindingSet` objects in that file returns zero hits for those indices. The whole selector
lives in `MCO_Attack.hkx`.

## 3. The transitions that ENTER the attack states — none bypasses the binding

**Level 1, `1HM_Behavior` (#0003, line 192734) → `AttackState` (#0781, `stateId 10`, line 73728).**
The fresh light attack from the drawn idle is `1HM_Ready_State` (#0005, `stateId 0`) local
transition array `#0013`, entry [3]:

```
event=attackStart(11)  toStateId=10  toNestedStateId=0  fromNestedStateId=0
priority=10  transition=#0017  condition=#0019  flags=0
```

`flags=0` — **no `FLAG_TO_NESTED_STATE_ID_IS_VALID`, no `FLAG_IS_LOCAL_WILDCARD`.** It is a
state-local transition on the ready state, not a wildcard; `1HM_Behavior` has
`wildcardTransitions null`. `selfTransitionMode` is `SELF_TRANSITION_MODE_NO_TRANSITION`.

The sibling entries that *do* set the flag are all power/directional variants —
`attackPowerStartLeft(16) toNested=3`, `attackPowerStartRight(12) toNested=4`,
`attackPowerStartInPlace(13) toNested=5`, `attackStartSprint(354) toNested=22`, and so on. Those
force a state one level down, inside `1HM_AttackBehavior` (#0820), which is the direction/kind
selector — **not** the MCO index. `toNestedStateId` addresses exactly one nesting level; it cannot
reach `AttackNodes_StateMachine`, which is six machines deeper (see §6).

The same shape repeats from `BlockState` (#2610) and `Bash_State` (#2632): the plain `attackStart`
entry is `toNested=0 flags=0`, only the power variants set the flag.

**Level 2, inside `MCO_Attack.hkx`.** `Main_StateMachine` (#0003, line 3055) has three states —
`AttackNodesState` (`stateId 0`, generator `#0013 hkbModifierGenerator
'AttackNodes_ModifierGenerator'` → `#0029`), `TransitionState` (`stateId 2`), and
`AttackNodesState_Duplicate` (`stateId 3`, → `#0099`). Its `startStateId` is bound too:

```
#0004 hkbVariableBindingSet (line 6): memberPath startStateId, variableIndex 241
      = MCO_Attack_StartStateId, BINDING_TYPE_VARIABLE
```

`wildcardTransitions null`. Every state-local transition into an AttackNodes state carries
`toNestedStateId 0` and `flags FLAG_USE_TRIGGER_INTERVAL|FLAG_USE_INITIATE_INTERVAL` — **never**
`FLAG_TO_NESTED_STATE_ID_IS_VALID`.

**Conclusion for §3: from ready, the engine enters `AttackState` plainly, walks down six machines
each of which resolves its own `startStateId`, and arrives at `AttackNodes_StateMachine`, which has
nothing to go on except the binding to `MCO_nextattack`. There is no hardwired index anywhere on the
path.** Note 28's first candidate explanation is closed.

## 4. The within-chain transition — the reference mechanism, and it is the SAME selector

There is no attack N → attack N+1 edge. `AttackNodesState1..10` all have `transitions null`,
`enterNotifyEvents null`, `exitNotifyEvents null`; the machine has `wildcardTransitions null`.
Ticket 11 recorded this correctly.

The chain is driven one level up, by `Main_StateMachine` **ping-ponging between two identical
subtrees**:

```
AttackNodesState (id 0) local transitions #0008 (line 134)
  [0] attackStart(22)                  -> toStateId 3, toNested 0, cond #0011, effect #0009
  [1] AttackStartH2HRight(635)         -> toStateId 3, toNested 0, cond #0011
  [2] attackStart(22)                  -> toStateId 3, toNested 0, cond #0012
  [3] AttackStartH2HRight(635)         -> toStateId 3, toNested 0, cond #0012
  [4] attackPowerStartInPlace(134)     -> toStateId 2 (TransitionState)
  [5] attackPowerStartForwardH2HRightHand(950) -> toStateId 2

AttackNodesState_Duplicate (id 3) local transitions #0087 (line 1883)
  [0..3] the same four events            -> toStateId 0
  [4,5]  the same two power events       -> toStateId 2
```

Each state also rewrites the ping-pong variable for the next entry:
`hkbExpressionDataArray #0028` (line 588) holds `MCO_Attack_StartStateId = 3`, and the array at line
2208 holds `MCO_Attack_StartStateId = 0`.

So a combo hit is: `attackStart` → `Main_StateMachine` swaps to the *other* copy → that copy's
`AttackNodes_StateMachine` activates → **its `startStateId` binding to `MCO_nextattack` picks the
clip.** The duplicate exists so that consecutive attacks alternate between two physically distinct
subtrees (you cannot cross-blend a clip generator with itself); it is not an index mechanism.

Two consequences matter:

- **The working path and the failing path use the identical selector.** Nothing about "fresh from
  ready" reads the index differently from "next hit in the chain". A refactor that adds per-index
  entry transitions would be adding a *second* selector next to a first one that already works.
- **The working path never leaves `AttackState`.** `AttackState`'s own transition array `#0791`
  (line 21461, 14 entries) contains **no `attackStart` self-transition** — it leaves only on
  `attackStop(8)`, `MCO_EndAnimation(473)`, `blockStart`, `shoutStart`, `BeginCast(114)`,
  `PowerAttackStop`. A combo continuation therefore stays inside `AttackState`, and
  **`MCO_Attack.hkb` stays continuously active for the whole chain.** A post-cast attack, by
  contrast, re-activates the nested graph from cold. That is the one real structural asymmetry
  between the path that works and the path that does not.

## 5. SH2's own entry, and why it is on the failing side of that asymmetry

`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/` patches `1HM_Ready_State`'s transition array
(`#4872.txt`, `MOD_CODE ~shtb~` block from line 965) with six additions, all of the same shape:

```
eventId $eventID[SH2_CastRight]$   toStateId 746002  toNestedStateId 0  flags FLAG_DISABLE_CONDITION
        $eventID[SH2_Cast2]$       toStateId 746003  toNestedStateId 0  flags FLAG_DISABLE_CONDITION
        $eventID[SH2_Cast3]$       toStateId 746004  ...
        $eventID[SH2_Cast4]$       toStateId 746005  ...
        $eventID[SH2_ArtStart]$    toStateId 746010  ...
        $eventID[SH2_CastChannel]$ toStateId 746006  ...
```

The exits are the mirror image. `#shtb$2.txt` is `SH2_CastRight_State` (`stateId 746002`,
`transitions #shtb$1`, `enterNotifyEvents null`, `exitNotifyEvents null`); `#shtb$1.txt` is its
transition array, and its two exit rows are:

```
eventId $eventID[SH2_CastExit]$  toStateId 0  toNestedStateId 0  flags FLAG_DISABLE_CONDITION
eventId $eventID[shoutStart]$    toStateId 0  toNestedStateId 0  flags FLAG_DISABLE_CONDITION
```

`#shtb$24.txt`, the art state's array, is identical in shape (`SH2_ArtExit` → `toStateId 0`,
`shoutStart` → `toStateId 0`, both `FLAG_DISABLE_CONDITION`, `toNestedStateId 0`).

**So the ticket-10 chain-out targets `stateId 0` = `1HM_Ready_State`, and nothing else.** SH2 owns no
transition into `AttackState` at all. Every SH2 exit returns to the drawn idle, and the attack that
follows is the ordinary `attackStart` transition [3] analysed in §3.

By the nested-state test the answer is therefore: **SH2's own entry does not ignore the variable
because of a nested-state flag — it has none, and neither does the vanilla transition it hands off
to.** SH2 is simply on the cold-reactivation side of §4's asymmetry: by construction it always
leaves the attack subtree entirely before the next attack, so the next attack always re-activates
`MCO_Attack.hkb` from scratch, which is precisely the case that measures wrong.

## 6. Conclusion, and the refactor judgment

### What makes the variable irrelevant at entry

Not a competing selector. Three file-level facts, taken together, put the failure on the
**nested-graph variable-propagation boundary at re-activation**:

1. **`MCO_Attack.hkb` is a nested behavior graph with `variableMode`
   `VARIABLE_MODE_DISCARD_WHEN_INACTIVE`** — `#0002 hkbBehaviorGraph 'MCO_Attack.hkb'`,
   `MCO_Attack.xml` line 11517. That flag is exactly the knob governing what a nested graph's
   variable values do across a deactivate/re-activate cycle. The working path never exercises it
   (§4: the graph stays active for the whole chain); the SH2 path exercises it on every single
   attack (§5).
2. **`MCO_nextattack` does not exist in the root graph.** `0_master.hkx` (Nemesis Output) declares
   290 variable names and `MCO_nextattack` is not among them — it carries `MCO_IsInSprintAttackCooldown`
   and the other SKSE-facing MCO variables, but not this one. `MCO_nextattack` is declared only in
   `1hm_behavior` (index 106 of 154) and in `MCO_Attack` (index 230 of 246). So the value SH2 writes
   has to cross at least one nested-graph boundary before it can reach the binding, and it crosses
   it at the moment the destination graph is being brought back to life.
3. **`MCO_Attack`'s own initial value for `MCO_nextattack` is 0** — `#0131 hkbVariableValueSet`,
   `wordVariableValues[230] = 0` (also 231/232/233 = 0, and `MCO_Attack_StartStateId`[241] = 0). Zero
   matches no state in a machine whose states are 1..10, so the fallback is the first state in the
   array, `AttackNodesState1` → `MCO_attack1`. **That is bit-for-bit the greatsword symptom**
   (`mco_attack1` after a channel, after a fnf cast, and on the mid-window press). The sword's
   `attack2` is not explained by the initial value and needs a runtime read to pin down — most
   likely a value left over from the interrupted swing's own activation rather than the value
   standing at press time.

I want to be explicit about the limit of a static read: the file proves the binding is the only
selector, proves the two paths differ in whether the nested graph was deactivated, and proves the
cold-start fallback lands on attack1. It cannot prove the exact intra-frame ordering of Havok's
variable sync against `hkbStateMachine::activate()`. That last step stays a hypothesis until a
runtime probe reads it.

### The candidate refactor: `SH2_ChainAttack1..N` — not feasible, and for two independent reasons

**(a) The file is out of reach.** `MCO_Attack.hkx` is not a Nemesis behavior. The Nemesis engine's
patchable set, enumerated from `Nemesis Unlimited Behavior Engine\Nemesis_Engine\mod\*\*`, is:
`0_master, 1hm_behavior, 1hm_locomotion, _1stperson, bashbehavior, blockbehavior,
bow_direction_behavior, defaultfemale, defaultmale, horsebehavior, magicbehavior,
magicmountedbehavior, mt_behavior, shout_behavior, sprintbehavior, staggerbehavior, weapequip,
turn, animationdatasinglefile, animationsetdatasinglefile`. No `MCO_Attack`. The winning
`MCO_Attack.hkx` is a static file shipped by `ADXP MCO 1.6.0.6 Bug Fixes` and consumed whole. Adding
`SH2_ChainAttackN` events and per-index transitions inside `AttackNodes_StateMachine` would mean
shipping a hand-edited replacement of another mod's behavior file — an outright overwrite of the
highest-priority MCO bug-fix mod, breaking on its next update and on any other mod that expects it.
That is not a change this repository can make.

**(b) `toNestedStateId` cannot reach the target anyway.** It addresses exactly one nesting level.
From `1HM_Ready_State` the destination is `AttackState` (id 10); the one level reachable is
`1HM_AttackBehavior` (#0820). `AttackNodes_StateMachine` sits six machines below that:

```
1HM_Behavior #0003
 └ AttackState #0781 (id 10)
    └ 1HM_AttackBehavior #0820
       └ AttackRight_State #0822 → AttackRightMSG #0846 (manual selector)
          └ AttackRight_WeapTypeSelection #0848   (startStateId ← iRightHandType)
             └ AttackRight_1HMSword #1008
                └ 1hm_JumpChecker #1009            (startStateId ← bCanJmpAtk)
                   └ 1hm_Not_Jumping_State #1011
                      └ AttackRight_1HM_Behavior #1013  (startStateId ← iIsInSneak)
                         └ 1HM_AttackRight_DefaultState #1015 → MCO_BRG_Attack #0863
                            └ [MCO_Attack.hkb] Main_StateMachine #0003 (startStateId ← MCO_Attack_StartStateId)
                               └ AttackNodesState #0005 → AttackNodes_StateMachine #0029
                                  └ AttackNodesState<N>          (startStateId ← MCO_nextattack)
```

Six of those levels already select themselves from a bound variable. A transition authored in
`1hm_behavior` cannot name a state inside `MCO_Attack.hkb`, per index or otherwise. Even with (a)
solved, the event would have to be consumed by a wildcard *inside* `MCO_Attack.hkb`.

**Verdict: drop the per-index-event refactor.** It buys a second selector for a mechanism whose
first selector is correct, in a file this project cannot patch.

### The simpler alternative the structure actually suggests

The graph says the seam is not "which state do I name" but "whose copy of `MCO_nextattack` is warm
when the nested graph wakes up". Two cheap, in-repo moves follow from that, in order of cost:

1. **Move the write from SKSE into the graph, on SH2's own exit edge.** MCO advances the index from
   *inside* the graph, via `@SGVI|MCO_nextattack|N` payloads — `1hm_behavior` already carries them
   as `#0009` (`@SGVI|MCO_nextattack|1`, ready-enter set `#0006`) and `#0786` (the identical payload
   in `AttackState`'s exit set `#0783`). `SH2_CastRight_State` (`#shtb$2.txt`) currently has
   `exitNotifyEvents null`. The shtb patch already authors state infos and payload arrays, so
   attaching an exit payload array is a small, Nemesis-legal change to a file this repo owns. Since
   a `hkbStringEventPayload` carries a constant, either author one payload per index behind the four
   cast states SH2 already has, or add one SH2-owned graph variable (the patch already adds
   `MSCO_attackspeed` to `1hm_behavior`'s variable list, so the mechanism is proven), have the DLL
   write *that*, and copy it across with an `hkbEvaluateExpressionModifier`
   (`MCO_nextattack = SH2_ComboRestore`) fired on the exit edge. Either way the write lands in
   `1hm_behavior`'s own copy — the direct parent `MCO_Attack.hkb` synchronises from — through the
   same event path MCO's own advance uses, rather than through an SKSE call into the root graph
   that does not declare the variable.
2. **Force the re-read rather than the value.** If (1) still measures wrong, the remaining lever is
   the deactivation itself: keep `MCO_Attack.hkb` from going cold across the cast, or re-enter it
   twice so the second activation sees a value the first one published. This is the "force
   reactivation" idea from note 28, and the graph does support testing it — but it is strictly worse
   than (1) as a first experiment, because (1) is a two-file edit and this is a routing change.

Both are decidable in one live run, using the OAR Animation Log as the clip oracle that note 28
established. Neither touches MCO's files.

## Files and object indices cited

- `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Nemesis Output\meshes\actors\character\behaviors\1hm_behavior.hkx`
- `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Nemesis Output\meshes\actors\character\behaviors\0_master.hkx`
- `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\ADXP MCO 1.6.0.6 Bug Fixes\meshes\actors\character\behaviors\MCO_Attack.hkx`
- `C:\Nolvus\Projects\spell-hotbar-2\nemesis\Nemesis_Engine\mod\shtb\1hm_behavior\#4872.txt`,
  `#shtb$1.txt`, `#shtb$2.txt`, `#shtb$24.txt`, `#0085.txt`

### Patch authored

`nemesis/Nemesis_Engine/mod/shtb/0_master/` now declares `MCO_nextattack` and
`MCO_nextpowerattack` (both `VARIABLE_TYPE_INT32`, initial value `1`) in the character root
behavior. The experiment: Havok links a nested graph's variables to the parent's by NAME, so a
parent-declared `MCO_nextattack` should sync into `MCO_Attack.hkb`'s
`VARIABLE_MODE_DISCARD_WHEN_INACTIVE` copy at activation, instead of that copy re-initialising
from its own defaults while actor-level writes never reach it.

Node IDs targeted — the three parallel structures, appended at the END of each array:

| Node | Class | Array | Base count | After |
|---|---|---|---|---|
| `#0106` | `hkbBehaviorGraphStringData` | `variableNames` | 230 | 232 |
| `#0107` | `hkbVariableValueSet` | `wordVariableValues` | 230 | 232 |
| `#0108` | `hkbBehaviorGraphData` | `variableInfos` | 230 | 232 |

`numelements` is left at the base `230` in all three, exactly as every installed patch does —
Nemesis recomputes the count from the merged array. Only the three arrays above are touched;
`eventNames`, `quadVariableValues`, `characterPropertyInfos`, and `eventInfos` stay untouched base.

Numbering cribbed from five installed mods that patch the same node set, all of which agree:
ADXP MCO 1.6.0.6 Bug Fixes (`amco`), True Directional Movement (`tdmv`), Precision (`colis`),
Throwable Weapons SKSE (`throwws`), and SCAR (`scar`, `#0106`/`#0108` only). Verified mechanically
rather than by eye: stripping each mod's own `MOD_CODE`/`ORIGINAL`/`CLOSE` blocks out of its
`#0106`/`#0107`/`#0108` yields a byte-identical base per node across all five (SHA-256
`23CA468D2BBC5CA1…` / `5ED39FD7105161AB…` / `921C3FED154242B1…`), and stripping the shtb blocks
back out of the three new files reproduces those same three hashes. Marker form is identical to
`amco`'s: markers at column 0, no `<!-- ORIGINAL -->` arm (pure append), content at the array's own
tab depth, CRLF, no BOM.

Duplicate check: the base `variableNames` array holds 230 entries and contains neither
`MCO_nextattack` nor `MCO_nextpowerattack`; MCO's own `0_master` patch adds only
`MCO_IsInSprintAttackCooldown`. That confirms the 290-name observation above from the patch side.

Not run: Nemesis. The `shtb` patch's file set changed (a new behavior folder), so this needs
Update Engine before Launch.
