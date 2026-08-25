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

---

## Round 2: the greatsword divergence — there isn't one in the graphs

Second read, after the `0_master` declaration was generated and measured live: **1H sword fixed
(a1 → a2 → cast → attack3), greatsword still resets to attack1 with the root variable provably
holding 3, and the owner then reported WARHAMMER fails identically.** The brief was to find the hop
on the 2H chain that still lacks the variable name and patch it.

**Headline: no such hop exists. The 2H light-attack entry path is node-for-node identical to the
1H one, and both terminate at the same single object.** Nothing was authored, because there is
nothing in a Nemesis-patchable behavior to author. Files re-decompiled fresh from the current
winners with `hkxc convert -v xml`; all object indices below are from those dumps.

### R2.1 Winners re-confirmed against the profile, and there is no 2H behavior file

Enumerated every `MODS\mods\*\meshes\actors\character\behaviors\` against
`profiles\Nolvus Awakening\modlist.txt` (top line = highest priority):

| file | providers (modlist line, enabled) | winner |
|---|---|---|
| `1hm_behavior.hkx` | Nemesis Output (616 +), TK Dodge RE (991 +), TK Dodge (993 +) | Nemesis Output |
| `0_master.hkx` | Nemesis Output (616 +), Jump Behavior Overhaul (1356 +) | Nemesis Output |
| `MCO_Attack.hkx` | ADXP MCO 1.6.0.6 Bug Fixes (110 +), ADXP - MCO (1002 +) | Bug Fixes |

**No mod anywhere ships `2hm_behavior.hkx`, and none exists in vanilla** — it is not in Nemesis's
patchable set either. All melee, 1H and 2H alike, lives in `1hm_behavior`. `0_master` reaches it
through exactly one ground-path reference, `#0531 'Weap_BehaviorBFR'`; the only other
`behaviors\1hm_behavior.hkx` references are `#2403 Jumping_Attacking_BFR`, `#2408
Jumping_Block_BFR`, `#2540 Falling_Attacking_BFR`, `#2548 Falling_Block_BFR`, `#2685
Landing_Attacking_BFR` — jump/fall/land, not the ground press. **One graph instance serves every
weapon on the ground.**

`1HM_Behavior` `#0003` has 41 states and exactly **one** ready state, `1HM_Ready_State` (`#0005`,
`stateId 0`) — there is no separate 2H ready state, so SH2's cast states and their `toStateId 0`
exits are shared by both weapon classes by construction.

### R2.2 The two entry chains, walked from the state info upward — identical

Ancestor walk of the four MCO-fed attack state infos (parent map built over every `#NNNN`
cross-reference in the file):

```
1H sword                                  2HM sword                                 2H warhammer/axe
#1015 1HM_AttackRight_DefaultState id 0   #1148 Default2HM_AttackRight   id 0        #1253 3rdP_2HW_AttackRightState id 0
 └#1013 AttackRight_1HM_Behavior           └#1146 AttackRight_2HM_SwordBehavior       └#1251 AttackRight_2HW_Behavior
   └#1012 AttackRight_1HM_iStateGen          └#1145 AttackRight_2HM_iStateGen           └#1250 DefaultAttackRight_2HW
     └#1011 1hm_Not_Jumping_State             └#1144 Not_Jumping_2hm_State               └#1248 AttackRight_2HWBehavior
       └#1009 1hm_JumpChecker                   └#1142 2hm_JumpChecker                     └#1247 AttackRight_2HW_iStateGen
         └#1008 AttackRight_1HMSword id 1         └#1141 AttackRight_2HM_Sword id 5          └#1246 Not_Jumping_2hw_State
           └#0848 AttackRight_WeapTypeSelection     └#0848 (same machine)                      └#1244 2hw_JumpChecker
             └#0846 AttackRightMSG …                                                             └#1243 AttackRight_2HW id ?
                                                                                                   └#0848 (same machine)
```

Same shape (`BSiStateTaggingGenerator` → jump checker → weapon-type state), same
`AttackRight_WeapTypeSelection` `#0848`, same `AttackRightMSG` `#0846` → `AttackRightMod` `#0840`
→ `AttackRight_State` `#0822` → `1HM_AttackBehavior` `#0820` → `AttackState` `#0781`. 2HW carries
one extra pass-through level (`#1251`/`#1250`/`#1248`); it is a plain state machine, not a graph
boundary.

**And all four states name the same generator.** `#1015`, `#1148`, `#1253` and `#0862` each carry
`<hkparam name="generator">#0863</hkparam>` — a single `hkbBehaviorReferenceGenerator
'MCO_BRG_Attack' -> Behaviors\MCO_Attack.hkx`. `1hm_behavior` contains exactly one reference to
that file (grep of `behaviorName` returns 14 hits; one is `MCO_Attack`). So there is **one**
nested-graph boundary and **one** nested `MCO_Attack.hkb` instance on the ground path, entered by
every melee weapon class. There is no 2H-side intermediate BRG, no extra nested graph, and
therefore no hop that could be missing a declaration.

### R2.3 Inside `MCO_Attack.hkx`: one selector, both machines, no weapon gate

- `#0029 AttackNodes_StateMachine` and `#0099 AttackNodes_StateMachine_Duplicate` both declare
  `variableBindingSet #0030`, and `#0030` is a single binding `startStateId -> variableIndex 230`
  (`= MCO_nextattack`). Confirmed by re-dump, answering round 2's question 2 directly.
- Both machines' ten states are index-identical: `AttackNodesState<N>` (`stateId N`) →
  `hkbModifierGenerator` whose expression is `MCO_currentattack = N` → clip
  `Animations\MCO_attack<N>.hkx`. `enterNotifyEvents`, `exitNotifyEvents` and `transitions` are
  `null` on all twenty states.
- The only other selector in the file is `Main_StateMachine #0003`, `startStateId` bound to index
  241 `MCO_Attack_StartStateId` (the ping-pong), plus `#0085 BRG_Transitions ->
  Behaviors\MCO_TransitionsNormalToPower.hkx`, reachable only from `TransitionState` (`stateId 2`),
  i.e. the power-attack transition, not the light chain.
- Nothing in the file reads `iRightHandType` or any weapon-type variable, and there is no second
  2H-side AttackNodes machine.

### R2.4 Who writes `MCO_nextattack`, per graph — all three writers are weapon-agnostic

Every object in `1hm_behavior` whose text contains `MCO_nextattack` (4 hits, one being the string
table):

| object | kind | owner | reaches |
|---|---|---|---|
| `#0009` | `hkbStringEventPayload` `@SGVI\|MCO_nextattack\|1` | `#0006`, enter set of `1HM_Ready_State` `#0005` | every weapon |
| `#0786` | `hkbStringEventPayload` `@SGVI\|MCO_nextattack\|1` | `#0783`, **exit** set of `AttackState` `#0781` | every weapon |
| `#0814` | `hkbExpressionDataArray` `MCO_nextattack = 1` | `#0813 MCO_ResetVariables_EEM` ← `#0812 MCO_ResetVariables_EDM` (activate `501 MCO_ResetVariables`, deactivate `502`) ← `#0807 ModifierList00` ← `#0806 1HM_AttackState_Generator` ← `AttackState` | every weapon |

Event `501 MCO_ResetVariables` is emitted from five state infos only — `AttackPowerFwd #1787`,
`AttackPowerBwd #1929`, `AttackPowerLft #1966`, `AttackPowerRt #2003`, `DualWield_Attack #2249` —
all under `1HM_AttackBehavior #0820`, all power/dual-wield, none of them 2H-specific and none on
the light-attack path. Both payload writers are the PIE (`event id 666`, Payload Interpreter)
form.

Subtree diff for completeness: descending `#1008` (1H sword), `#1129` (1H axe), `#1141` (2HM
sword), `#1243` (2HW) and scanning every object for `MCO` expressions or payloads returns **zero
hits in any of the four**. The weapon-type subtrees contain no MCO variable writer at all; the 1H
subtrees are larger only because the 1H jumping blend under `1hm_Jumping_State #1033` pulls in the
vanilla clip forest.

### R2.5 Variable declarations along the 2H chain — all present

| graph | declares `MCO_nextattack` | index / total |
|---|---|---|
| `0_master.hkx` (Nemesis Output, post-`63fcf81`) | yes | appended by the `shtb` patch |
| `1hm_behavior.hkx` (Nemesis Output, regenerated) | yes | **106** of 154 (`107 MCO_nextpowerattack`, `108 MCO_currentattack`, `109 MCO_currentpowerattack`, `110 MCO_AttackSpeed`, `142 MSCO_attackspeed`) |
| `MCO_Attack.hkx` | yes | 230 of 246 |

The chain root → `1hm_behavior` → `MCO_Attack` carries the name at every hop, and the 2H entry
crosses **exactly** those graphs and no others. **Question 3 answers itself: there is no hop on the
2H chain that lacks the declaration, because the 2H chain crosses no graph the 1H chain does not.**

### R2.6 Therefore: nothing authored, and the fix is not a Nemesis patch

Per the brief's rule ("if the file is NOT Nemesis-patchable or the divergence is structural, do NOT
author anything"), no patch was written. The divergence is neither: it is *absent* at the topology
layer. A weapon-class-wide failure with a weapon-agnostic graph means the difference lives in the
only layer that IS weapon-specific — the animation data OAR feeds into those fixed clip generators,
and the timing of the PIE writes those animations carry.

### R2.7 What actually is weapon-specific, measured

The clips are chosen by `Nolvus OAR Stance Combat Framework`, whose per-stance folders ship only a
`config.json`; the `.hkx` are merged in by other mods at the same VFS path. Enabled contributors and
their annotation content (`hkxc-anno-cli dump`):

| stance folder | winning contributor (modlist line) | indices shipped | advance annotation |
|---|---|---|---|
| `Sword Neutral` | `Elder Creed - Blade` (1448 +) — every other sword contributor is disabled | 1–5 | `PIE.@SGVI\|MCO_nextattack\|N+1` at `MCO_WinOpen`, 0.63 s of a 1.8 s clip; **attack5 has no advance** |
| `Greatsword Neutral` | `Animations - Mercenary Greatsword` (256 +) over `Berserker Greatsword Moveset` (1428 +) | 1,2,3,4,9,10 | advance at `MCO_WinOpen`, 0.82 s of a 2.17 s clip; **attack4 → 1** (four-hit loop); extra `PIE.@SGVF\|MCO_AttackSpeed\|1.15` at t=0.06 |
| `Warhammer Neutral` | `For Honor in Skyrim` (1461 +) | 1–10 | advance at **t = 0.000000**, i.e. clip start, not `MCO_WinOpen` |

Two facts worth keeping: index 3 exists in all three packs, so "greatsword plays attack1" is not an
OAR fallback for a missing `MCO_attack3.hkx`; and the SH2 cast clips (`MSCO_left*/right*` from
`Magic Casting Behavior Overhaul`) write only `PIE.@SGVI|msco_nextright|2` — they never touch
`MCO_nextattack`, so the cast itself is not the stomp.

Note the timing spread: the 1H pack publishes its advance 0.63 s in, the greatsword pack 0.82 s in,
the warhammer pack at 0 s. Every one of those writes is a PIE payload processed by an SKSE plugin
off the animation-event sink, not by Havok inline. A race between that asynchronous write, the
ready-enter stomp to 1, SH2's re-assert, and the moment Havok syncs the
`VARIABLE_MODE_DISCARD_WHEN_INACTIVE` nested copy at activation is the only remaining shape that
can produce a weapon-class-wide split over a weapon-agnostic graph. This is a hypothesis, not a
finding — a static read cannot order those four events.

### R2.8 Smallest viable next step, and a cheap oracle the graph already contains

**`MCO_currentattack` is written by the playing state itself** — `AttackNodesState<N>`'s modifier
evaluates `MCO_currentattack = N` (R2.3). It is therefore an exact, in-graph name for the clip that
loaded, and it does not depend on the owner keeping the OAR Animation Log window open, which is the
constraint that has made every measurement in this ticket expensive. It is declared in
`1hm_behavior` (108) and `MCO_Attack` (232) but **not** in `0_master`, so the DLL cannot read it
today.

Recommended, in order:

1. Add `MCO_currentattack` (and `MCO_currentpowerattack`) to the same three arrays the `shtb`
   `0_master` folder already appends to (`#0106`/`#0107`/`#0108`), then log, per press and per
   weapon: `MCO_nextattack` at SH2 exit, at ready-enter, at the press, and `MCO_currentattack` one
   frame after the press. That single run distinguishes "the value never reached the nested copy"
   from "the value reached it and the machine read it anyway", and it makes the greatsword and
   warhammer cases self-reporting. Cost: one Nemesis run **with Update Engine** (the `shtb` file set
   changed at `63fcf81` and would change again), and it perturbs the patch that currently makes 1H
   work — which is why it is left for the coordinator to authorize rather than done here.
2. If (1) shows the nested copy holding 1 while the root holds 3, the seam is the activation sync
   and the lever is §6's option 2 (keep `MCO_Attack.hkb` warm, or re-enter it), not another
   declaration.
3. If it shows the nested copy holding 3 while `MCO_currentattack` reads 1, the machine is being
   activated before the sync lands, and the fix has to move the write earlier — the exit-payload
   route of §6 option 1, authored on SH2's own exit edge inside `1hm_behavior`.

### Files read in round 2

- `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Nemesis Output\meshes\actors\character\behaviors\1hm_behavior.hkx`, `0_master.hkx`
- `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\ADXP MCO 1.6.0.6 Bug Fixes\meshes\actors\character\behaviors\MCO_Attack.hkx`
- `C:\Nolvus\Instances\Nolvus Awakening\MODS\profiles\Nolvus Awakening\modlist.txt`
- `…\MODS\mods\Animations - Mercenary Greatsword\…\Greatsword Neutral\mco_attack{1,2,3,4,9,10}.hkx`
- `…\MODS\mods\Elder Creed - Blade\…\Sword Neutral\mco_attack{1..5}.HKX`
- `…\MODS\mods\For Honor in Skyrim\…\Warhammer Neutral\MCO_Attack{1..10}.hkx`
- `…\MODS\mods\Magic Casting Behavior Overhaul\meshes\actors\character\animations\MSCO_left{1,2}.hkx`, `MSCO_right2.hkx`
