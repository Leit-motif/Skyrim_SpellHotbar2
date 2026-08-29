# 33 — Commit an NPC's concentration cast

**Type:** spike, then feature (Nemesis patch + FOMOD option)

**Status:** CLOSED 2026-08-29, superseded by ticket 58 — closed in the sweep that day, not on
the day the work landed. Every mechanism this ticket carried is retired and verified gone from
the tree: `nemesis/Nemesis_Engine/mod/` holds only `shcr` and `shtb` (no `shcc`), `.scratch/`
has no `shcc-build`, `grep` for `blocks_movement` / `is_movement_blocking_cast` /
`blocksMovement` across `skse_plugin/` returns nothing, and `SpellHotbar_RootedConcentration`
appears in neither the repo, the Nolvus Awakening `mods/` directory, nor any profile's
`modlist.txt`. The ask itself was delivered by ticket 58's `shcr` patch and owner-confirmed
live ("player rooting works", NPC cell likewise). Two of this ticket's acceptance cells did
not survive contact: the FOMOD option was overruled by the owner (`shcr` ships
unconditionally, ticket 58 / ticket 59), and the SpeedMult ESP route was rejected outright.
**Do not build anything in the "The build, restated" section below** — it describes a
mechanism that failed three times and has been deleted. It is kept as the record of why the
flag-plant shape does not work on layered states.

**Blocked by:** None.

## What the owner asked for

2026-08-24, on being told NPC concentration casts are unrooted:

> "this would be something i would want to cover with either sh2 or shout-mco, or even a small
> optional mod, but that just sounds annoying to the user. maybe it would be a separate patch in
> the fomod."

and the goal it serves:

> "i'm after a seamless modernized mco-feel combat experience utilizing sh2 as the interface."

An enemy mage who strafes while streaming a beam is the seam. Every other actor in the fight
commits; that one does not.

## Where it goes, and why it is not a new mod

ADR-0015 settles the ownership question: a root is authored on the state that owns the action, so
this is a patch on the vanilla concentration states, and NPC coverage falls out of the state being
shared rather than out of anything addressing NPCs. **It is not SH2's** — no NPC ever enters a
`shtb` state — and it is not ShoutMCO's, which owns shouts. It is MSCO coverage, and
`magic-casting-behavioral-overhaul/` is the workspace that holds MSCO's sources and patch
lifecycle.

Distribution is a separate question with a separate answer: ship it as an optional group in this
fork's FOMOD, which `python_scripts/create_fomod_installer.py` already builds, gated on
`MSCO.esp` being active the way the existing optional groups gate on their own plugins. The owner
is right that a standalone mod for one behavior file is friction the player should not have to
absorb.

## The spike — one read, before any authoring

MSCO's DLL does **no** rooting: `ref/src/` has no `bAnimationDriven` write, no `moveStop`, no
`ToggleControls`. Its Nemesis patch carries the plant, in the same shape SH2 uses — a
`BSIsActiveModifier` with an `hkbVariableBindingSet` over its `magicbehavior` states.
`Nemesis_engine/mod/msco/magicbehavior/#msco$30.txt` binds five members: `bIsMSCO`,
`bAllowRotation`, `bMSCO_LRCasting`, and **two raw variable indices, 65 and 66, that have not
been resolved to names.**

Answer three things from the patch and the graph, and the size of this ticket is known:

1. Which flags does MSCO's binding set actually carry — is `bAnimationDriven` among indices 65
   and 66, or does MSCO commit a cast some other way?
2. Which states does it cover, and which state does an NPC's concentration cast actually run
   through? "Concentration is not covered" is the owner's reading and the thing to confirm before
   patching anything.
3. Does MSCO's own OAR set already distinguish the case? It ships `Base - default NPCs 1` and
   `NPCs 2` submods, so NPCs are not an afterthought there and the gap may be narrower than it
   looks.

If MSCO's plant already covers the state and something else is defeating it, this becomes a
different ticket and the patch is not written.

## The build, if the spike says the state is unpatched

Copy the plant: a `BSIsActiveModifier` on the concentration state binding `bAnimationDriven`
(with `bAllowRotation` / `HKSMoveON` / `bHeadTrackSpine` as SH2's pair does —
`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$11.txt`, `$12.txt`). Nemesis mod code of its
own, not appended to `shtb`: this patch is optional and `shtb` is not.

## History of the rule, compressed — three owner rulings in one day, 2026-08-24

1. First pass: NPC concentration should be covered somewhere, "maybe a separate patch in the
   fomod." This ticket opened as a root on the vanilla concentration states, flagged as colliding
   with ticket 32's half-speed want.
2. Second pass: "the same rules need to apply to everyone consistently" — concentration became
   half speed for every actor, and this ticket flipped to a shared conditioned `SpeedMult`
   record.
3. Final: on learning half speed requires new animation assets, **movement went out of scope.
   "As long as both players and NPCs are rooted during concentration casts, I think we're good
   to proceed."** The conditioned record dies unbuilt (design preserved in ADR-0015's first
   amendment for the future endeavor), and this ticket returns to its original shape.

The collision that shaped versions 1 and 2 no longer exists: rooting the vanilla concentration
states roots the player's equipped-hand concentration casts too, and that is now the *desired*
outcome, not a conflict. The player's hotbar channel is already rooted today — ticket 28's held
`shtb` state plus the WASD capture — so the player's side of this rule costs zero work.

## The build, restated for the final rule

Root the concentration states MSCO does not cover, with the plant this stack has proven three
times: a `BSIsActiveModifier` binding `bAnimationDriven` (with `bAllowRotation` /
`bHeadTrackSpine` so the caster still pivots and tracks), per SH2's own pair at
`nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$11.txt` / `$12.txt`. Its own Nemesis mod
code, not appended to `shtb` — this patch is optional and `shtb` is not. Shipped as a
dependency-gated optional group in the FOMOD (`python_scripts/create_fomod_installer.py`), gated
on `MSCO.esp`.

The spike above still runs first and unchanged: if MSCO's binding set already covers the
concentration states and something else defeats it, this becomes a different ticket and no patch
is written.

## Built 2026-08-28 — the `shcc` Nemesis mod (authoring + static validation only)

New Nemesis mod code `shcc` ("Spell Hotbar 2 - Rooted Concentration Casts") at
`nemesis/Nemesis_Engine/mod/shcc/`, five new nodes and six vanilla-node patches, all in
`magicbehavior`. The plant is SH2's own proven pair copied verbatim in semantics from
`#shtb$13`/`$14`: a `BSIsActiveModifier` behind an `hkbVariableBindingSet` binding
`bAnimationDriven`, `bAllowRotation`, `HKSMoveON` (all uninverted, so all true while the state
is active — the caster stops translating but still pivots) and `bHeadTrackSpine` inverted.

The insertion point is **not** uniform across the six states, because the graph is not:

| state | node patched | how |
|---|---|---|
| MRh_AimedConcentration | `#0184` | vanilla `hkbModifierList` — append `#shcc$1` (conflict-free) |
| MLh_AimedConcentration | `#0489` | vanilla `hkbModifierList` — append `#shcc$1` (conflict-free) |
| MRh_SelfConcentration | `#0131` | `modifier` `#0106` → `#shcc$2` (list wrapping `#0106` + `#shcc$1`) |
| MLh_SelfConcentration | `#0440` | `modifier` `#0106` → `#shcc$2` (same shared list) |
| DualMagic_SelfConcentration | `#0317` | `generator` `#0318` → `#shcc$3` (new modifier generator) |
| DualMagic_AimedConcentration | `#0337` | `generator` `#0338` → `#shcc$4` (new modifier generator) |

The two Aimed states reach a real vanilla `hkbModifierList`, so those two are array appends.
The two Self states reach a single-pointer `modifier` on a vanilla `hkbModifierGenerator`, and
the two Dual states have no modifier generator at all — both are wrap-and-replace, the same
shape `sbeef` (`#sbeef$7`/`$21`/`$25`/`$48`) and `pscd` (`#pscd$7`/`$10`/`$13`) already use on
these exact params in the live stack.

`bAnimationDriven`, `bAllowRotation` and `HKSMoveON` are not vanilla `magicbehavior` variables
(the vanilla table holds 81 entries and none of the three); they exist at runtime today only
because Hot Key Skill declares them. `shcc` declares its own in `#0077`/`#0078`/`#0079` so the
patch stands alone. Duplicate `NEW` declarations across mod codes are tolerated — confirmed by
reading the merged `temp_behaviors/magicbehavior.txt`, where `bAllowRotation` is declared five
times under `hotkey`, `msco`, `tkds`, `tkuc` and `tudm`. `bHeadTrackSpine` is vanilla (index
65) and is not redeclared.

FOMOD: a new "Combat Behavior" `SelectAny` group in the Install Options step, one option
"Rooted Concentration Casts (MSCO)", `defaultType` Optional and Recommended only when
`MSCO.esp` is Active — the spell packs' own `fileDependency` shape. Unselected it installs
nothing; the payload is staged from this repository's `nemesis/` into
`2100 Optional - Rooted Concentration Casts/`.

Both the generator and the validator are committed under `.scratch/shcc-build/` so the patch
can be rebuilt and re-checked rather than re-read by hand.

**No runtime evidence.** Nemesis has not been regenerated and the game has not been launched;
every acceptance box below is still open.

## Landed + deployed 2026-08-28 late evening — runtime sweep queued behind a live game

Merged to `main` (`f79937d`, coordinator-reviewed: plant verified byte-equivalent to
`#shtb$13`/`$14`, variable declarations checked in all three tables, validators re-run clean,
FOMOD XML re-generated and parsed). The `shcc` folder (15 files) is copied into
`Dev - Spell Hotbar 2\Nemesis_Engine\mod\shcc` — inert: unticked in Nemesis, invisible to the
running session's snapshotted VFS.

Skyrim was LIVE at deploy time (started 22:58, likely the owner after the ticket 32 rejection),
so no Nemesis run and no relaunch were attempted. Remaining, in order, once the game is free:

1. `run-nemesis.ps1 -Tick shcc -Apply -UpdateEngine` (new file set ⇒ Update Engine; script in
   `thuum-fully-animated-shouts-mco/tools/`). Game closed, MO2 up, single instance.
2. Post-regen, before launch: read the merged `temp_behaviors/magicbehavior.txt` and confirm the
   dual-state generator chain — `#0317`/`#0337` are single-value params that `pscd`, `sbeef`,
   and now `shcc` all wrap, the one contended edit in this patch. Verify all three mods'
   modifiers remain reachable, not just the last writer's.
3. Relaunch, then the acceptance sweep. Cheap oracle first:
   `GetAnimationVariableBool("bAnimationDriven")` on the casting actor (rooted ⇒ true), then NPC
   displacement via position reads across a real enemy mage's channel (NPC movement is AI-driven,
   so headless displacement measurement is valid — the injected-WASD ban is player-only). Player
   equipped-hand cell: equip Flames via `EquipItemEx`, drive the channel with an injected
   `Left Attack/Block` hold (vanilla cast path, event-side — not the SH2 keybind hook). End-path
   cells: interrupt/stagger/death, root lifts, AI resumes.
4. FOMOD install/uninstall check on the generated installer.

## Nemesis regeneration 2026-08-28 23:28–23:32 — clean, verified in the compiled binary

`run-nemesis.ps1 -Tick shcc -Apply -UpdateEngine`: Engine update 124 s, generation 116 s,
1058 animations, no shcc-related warnings (`Mod Checked 25: shcc`). Verified against the
**compiled** `magicbehavior.hkx` (hkxc → XML), not just the merged text, because the merged text
stacks all contenders for a single-value param and only one compiles:

- All six states reach the root. Aimed lists composed (`{vanilla, pscd-twist, sbeef, shcc}`);
  Self states' generator → `SHCC_SelfConcentration_ML {#0106-equiv, root}`; both Dual states'
  generator → the SHCC wrapper → root + inner SM.
- **Contention resolved last-checked-wins, and shcc displaced two mods where the param is
  single-valued:** `sbeef` (State Behavior Framework 2.0) lost its
  `hkbEvaluateExpressionModifier`s on MRh/MLh Self and both Dual states; `pscd` (Proper Spell
  Cast Direction) lost its `hkbTwistModifier` arm-aim on **Dual Aimed only** (single-hand aimed
  keeps it — pscd sits in the lists we appended to). Precedent: pre-shcc, pscd was already
  displacing sbeef on Dual Aimed in this load order, with no observed fallout. If SBF-conditioned
  behavior misbehaves during concentration casts, this displacement is the first suspect — it is
  a property of Nemesis single-value merging, not fixable from shcc's side (a patch cannot
  reference another mod's `$` nodes without hard-depending on it).

## Runtime sweep, session 1 — 2026-08-28 ~23:45–00:00, save CS-Test (auto-loaded latest), profile Nolvus Awakening

Player cells (equipped-hand, vanilla cast path via injected attack holds — event-side, per playbook):

- **Left aimed (Flames, `MLh_AimedConcentration`): GREEN.** `bAnimationDriven` false at drawn
  idle → true 1.5 s into the channel (with `bAllowRotation` true — our binding's signature) →
  false 2 s after release.
- **Left self (Healing, `MLh_SelfConcentration` — the contested `#shcc$2` wrapper path): GREEN.**
  Root true during a genuinely draining channel (magicka −13/s); released to false within 1.5 s
  of the button-up reaching the graph. One release event was lost in injection (channel ran ~9 s
  past the first `up`; root held exactly as a physically-held button would; explicit second `up`
  released it) — input-injection raciness, not a patch fault.
- **Right aimed: NOT REACHED, pre-existing MSCO initiation bug, not a shcc fault.** The injected
  right press with Flames in both hands produced zero player clips and no magicka drain; MSCO.log
  shows the documented dead-caster signature (`BeginCastLeft` → `[GetEquippedSpell] No spells` →
  `InterruptedCast` — MSCO's `AttackBlockHandler::ProcessButton` hook owns that button). The
  MRh states carry the identical verified modifier in the compiled binary (`#0184` list holds
  `#0737`); the magic-casting-behavioral-overhaul workspace owns the initiation bug.

NPC cell (**the headline**) — real placed Master Necromancer `0x000D7790`, Fort Snowhawk
interior (`coc FortSnowhawk01`, tgm, `player.moveto`; it already knew Flames — AddSpell returned
false, nothing granted; nothing saved):

- **GREEN.** 30-sample loop (350 ms cadence) of `bAnimationDriven` + X/Y on the mage across three
  separate Flames channels: false stretches moved ~40 units/sample (closing/strafing); true
  stretches (samples 16–19, 28–29) froze — < 0.1 unit over ~1.4 s of channel. Every channel ended
  true→false with movement resuming at full rate within one sample. No AI wedge across the whole
  window. Displacement, not footfalls, per thuum 66.
- Skeletons in the cell are out of scope by construction — they run their own graph, not
  `magicbehavior`.

Owner interjected "this isn't working" mid-loop (session stopped for usage); the telemetry above
says the root planted and lifted cleanly, so if that was a visual observation it is unexplained
and needs the owner to describe what they saw.

Open cells after session 1: NPC rotation-tracking mid-channel (headless plan: displace the
player mid-channel, read the mage's heading), NPC fire-and-forget (confirm MSCO's DLL coverage),
stagger/death end paths, FOMOD install/uninstall in MO2 (static half done), owner-eyes feel pass.

## Runtime sweep, session 2 + the moving-entry defect — 2026-08-29 morning

Session 2 (fresh relaunch, CS-Test): the necromancer never channeled this fight (repositioned,
then held ground; root false throughout), so the rotation-tracking and NPC fire-and-forget cells
stay open. Clip ring flooded unfiltered (9,255 dropped) — filter from the start next time, per
the playbook's own note. Game closed via qqq, nothing saved either session.

**Owner-reported defect (2026-08-29): moving entry stutters.** Verbatim: already in motion, press
Flames from the left hand → "the character stutters and keeps moving then eventually stops after
a while." From standstill → roots correctly and cleanly. This is the ticket-32 rejection
signature (entry momentum carried, steering dead) and thuum 66's shout-slide class, now on the
rooted state's entry edge.

**Mechanism (static diagnosis, reference compared):** the owner-accepted hotbar channel root —
`SH2_Channel_State` (`1hm_behavior/#shtb$32`) — is a FULL-BODY state playing its own clip
(`1HM_Shout_Inhale.HKX`); entering it exits locomotion, so legs stop and `bAnimationDriven`
roots the controller with nothing left to fight. The vanilla concentration states are LAYERED
over live locomotion (that is why vanilla walks while channeling). shcc's plant roots the
controller, but held movement input keeps feeding the still-active locomotion layer: blend
fight → stutter → momentum decay → late stop. Standstill has no fight; NPC AI stops issuing
movement intent when it casts (session 1 showed a clean <1-sample freeze), so NPCs are immune.
Behavior-only rooting is structurally insufficient for the PLAYER on a layered state — no graph
primitive gates the player controller's input while the locomotion layer runs.

**Fix options, ranked:**

1. **Narrow DLL input capture (recommended)** — while a concentration cast is active on an
   equipped hand (cast-event gated, or `bAnimationDriven` + caster-state gated), SH2's DLL
   swallows movement input. This is ticket 39's "we just wanted to block input," scoped to the
   channel; it revives the ticket-35 capture with a tight gate. Needs an ADR-0015 amendment: the
   root stays behavior-owned; the player-side INPUT BLOCK on layered states is DLL-owned because
   the behavior graph cannot express it. NPC side stays behavior-only (proven green).
2. **moveStop on state enter / moveStart on exit** (enterNotifyEvents on the six states) — the
   vanilla-native way to idle the locomotion layer. Risk: player input may edge-retrigger
   moveStart while keys are held (same stutter, different flavor), and exit recovery must be
   airtight. Worth one instrumented trial only if 1 is rejected.
3. Accept the stutter, ship, park alongside thuum 66. Not recommended — the owner already
   rejected exactly this feel once.

Confirming the mechanism live needs owner hands (moving entry is injection-proof); a
frame-cadence probe of `bAnimationDriven` + player velocity during one moving-entry cast would
turn the hypothesis into evidence before any DLL work.

## Owner rulings 2026-08-29 morning

1. **NPC cell needs owner eyes to close**: "i will need to see the npc myself to sign off, just
   because i need to make sure there is no perceivable stutter. this needs to feel natural."
   Telemetry green stands, but the displacement acceptance box stays open until the owner watches
   a live enemy mage channel.
2. **Hotbar reference confirmed**: "we already signed off on the hotbar. it works perfectly (for
   the reason you stated)" — full-body state, no layer fight. Diagnosis validated.
3. **DLL input capture approved** for the moving-entry stutter. In build: narrow gate
   (equipped-hand concentration cast active AND bAnimationDriven true → swallow movement input),
   ADR-0015 amendment alongside.

## Built 2026-08-29 — fix option 1, the narrow DLL movement capture (no runtime evidence)

Option 1 above, owner-approved and scoped as ADR-0015's 2026-08-29 amendment records it. Three
files in `skse_plugin/`:

- `src/casts/combo_cache.h` — the pure half of the gate:
  `magic_caster_state_is_actively_casting`, `hand_holds_active_concentration_cast`,
  `concentration_cast_swallows_movement`, plus plain-int mirrors of
  `RE::MagicCaster::State::kCharging`/`kCasting` so the predicate is testable without
  CommonLibSSE.
- `src/input/input.cpp` — the engine half and the capture, at the same seam `c73b4f1` emptied
  (the `processAndFilter` button branch, after the frame-blocking block, before the cast
  chain-out). `equipped_hand_concentration_cast_active` reads both hand `MagicCaster`s;
  `concentration_root_is_swallowing_movement` adds `GetGraphVariableBool("bAnimationDriven")`;
  `is_movement_control` matches Forward / Back / Strafe Left / Strafe Right only. Non-up events
  only; keyboard, gamepad and mouse, as the retired capture did. Two `static_assert`s tie the
  int mirrors to the real enum.
- `src/casts/combo_cache_test.cpp` — three new cases covering both halves of the gate and the
  two must-pass cases (MCO attack, patchless user). The engine reads are runtime-only and are
  not mocked.

Deliberately NOT revived: the `blocks_movement` virtual chain, `is_movement_blocking_cast`, and
the `combo_cache` shtb-state helpers. The old capture keyed on SH2's own cast bookkeeping; this
one keys on the live engine state, so it also covers vanilla equipped-hand casts SH2 never
drives.

Build green (7/7 test binaries pass, `SpellHotbar2.dll` links) from a fresh configure in the
agent worktree. **Not deployed, game not launched — the moving-entry cell is owner-hands work
and stays open.**

## Deployed + regression-swept 2026-08-29 07:23–07:35 — game left running for the owner

Capture landed on main (`98350c9`, merge of the reviewed worktree build; ADR-0015 third
amendment and playbook lesson alongside, `c160e21`). `SpellHotbar2.dll` rebuilt from main and
deployed to `Dev - Spell Hotbar 2` (07:23:37); relaunched, CS-Test auto-loaded. Regression
sweep, all green:

- Standstill equipped-hand channel (Flames left): root true mid-channel, false 2 s after
  release — the capture does not disturb the root path.
- Hotbar `castSlot(0)`: full clean lifecycle in SpellHotbar2.log (left SpellFire, armed payload
  at 0.50 s, MSCO_WinOpen/Close, SH2_CastExit) — the input-hook change does not disturb the
  driver-cast path.
- No new startup errors; only the pre-existing CSV-loader warnings.

**Handed to the owner with the game running.** The two feel cells only they can judge: (1) the
moving-entry equipped-hand channel — with the capture, held WASD should go quiet the moment the
channel starts, no stutter, no slide; (2) watch a real enemy mage channel — "no perceivable
stutter, needs to feel natural." Also still open: NPC fire-and-forget confirmation, NPC
rotation-tracking mid-channel, FOMOD install/uninstall in MO2.

## PIVOT 2026-08-29 morning — the plant fails on layered states for everyone; SpeedMult record trial staged

Owner feel-tests rejected both halves live: player moving entry stutters/slides (screenshot:
scorch trail through the whole channel), and the NPC — telemetry caught `bAnimationDriven=true`
while the mage translated 200+ units/400 ms, moving "like a psychopath." **Session 1's NPC
freeze was a false positive: the AI had chosen to stand.** Verdict: the `bAnimationDriven`
plant only works on FULL-BODY states; on the layered vanilla concentration states it fights
live locomotion (player: held keys; NPC: AI intent) and roots nobody. The DLL event capture
also failed — movement is poll/state-driven; `MovementHandler`'s vector is set at key-down
(pre-cast) and only clears on the up we deliberately pass.

Owner approved trying ADR-0015's preserved design at full strength: **the conditioned SpeedMult
record, −100 while casting.** Owner's stated fallback if this fails too: remove concentration
rooting from SH2 entirely (reluctantly — "consistency is so important to the user experience").

Trial state, ready to test after the owner's computer restart:

- `SpellHotbar_RootedConcentration.esp` authored (mod `houseCARL - SpellHotbar_RootedConcentration`):
  MGEF `SH2_RootedConcentration_MGEF` (000800, ValueModifier/SpeedMult, ConstantEffect/Self,
  Detrimental+Recover+Painless+NoDuration, condition `IsCasting == 1` on Subject) + ability SPEL
  `SH2_RootedConcentration_Ability` (000801, magnitude 100). **NOT yet enabled in MO2** — owner
  ticks mod + plugin.
- `shcc` UNTICKED and Nemesis regenerated (07:49:17, 1058 anims; compiled magicbehavior.hkx has
  0 SHCC objects). The DLL capture is inert with the flag never set; decide its removal after
  the trial.
- Trial script: relaunch, `player.addspell 000801:...` + AddSpell to a real mage (Fort Snowhawk
  necromancer 0x000D7790 knows Flames), owner feels moving-entry + NPC channel.
- Known trial caveats: `IsCasting` also roots FF casts (consistent with the everything-roots
  rule) AND wards/telekinesis (over-scope — narrow later if it bothers); ability conditions
  re-evaluate on the engine's ~1 s clock, so watch for perceptible root-engage latency; the
  orphaned-AV residue trap applies — never save with the ability applied then disable the
  plugin (quarantine any such save).
- If accepted, the permanent shape: FOMOD ships this ESP (SPID for NPCs, DLL grants the player),
  the shcc Nemesis patch retires, ADR-0015 gets its fourth amendment.

## Acceptance

- [ ] An NPC streaming a concentration spell does not translate for the length of the channel —
      measured displacement on a real enemy mage, not a console-summoned one. (Displacement, not
      footfalls: thuum ticket 66 is the lesson.)
- [ ] The NPC still turns to track its target — `bAllowRotation` stays unbound or true. A mage
      frozen facing the wrong way is worse than one that strafes.
- [ ] An NPC's fire-and-forget cast is rooted, same bar as the player's ticket 19 — confirm
      MSCO's existing coverage rather than assuming (spike question 2).
- [ ] The root lifts on every end path: channel end, interrupt, stagger, death. NPC combat AI
      does not wedge — breaking line of sight or killing the target returns normal movement.
- [ ] The player's equipped-hand concentration cast is rooted by the same patch, matching the
      hotbar channel's existing root — one rule, however the cast started.
- [ ] The FOMOD option installs and uninstalls cleanly, and the patch is absent when the option
      is not chosen.
- [ ] Evidence names the commit, the Nemesis regeneration, the save, and the profile.
