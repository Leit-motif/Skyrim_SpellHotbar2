# 08 — Distribute the SH2 cast states into the weapon behaviours

**Type:** feature (Nemesis patch + driver), the priority axis of the 2026-08-11 direction

**What to build:** Entry into `SH2_CastRight_State` from a drawn **weapon** stance, so the
owner's target loop — weapon swings → SH2 cast → weapon swings — works. Today entry works
only from the drawn magic stance: the shtb patch registers its events and state in
`magicbehavior.hkx` alone, and per-graph event tables mean a graph that never registered
`SH2_CastRight` cannot hear it. The T05 save's greatsword refusals (seven `-> false`, two
sessions) are the proof, not a bug: `1hm_behavior` and friends are deaf until patched.

**Blocked by:** Nothing for the idle-stance slice. Mid-swing entry (pressing "1" during a
live MCO attack) is deliberately OUT of this ticket — that is the ShoutMCO ticket-50
cast-intent API per the 2026-08-08 driver boundary; this ticket only makes the weapon
graphs able to hear and play the cast at all.

**Status:** resolved

## What this slice is

With a weapon drawn and idle (not mid-swing): `castSlot(0)` / press "1" → notify returns
true → MCBO-style cast clip plays → spell commits at the clip's SpellFire annotation →
state exits at clip end → player is back in the weapon stance. MCBO coexistence is a hard
constraint: touch nothing msco/MCBO owns.

## Verified mechanics to build on (all live-verified 2026-08-11, do not re-derive)

- **The magicbehavior slice works and is owner-accepted** (commit `8effcad`). Replicate its
  shape, do not redesign it: state + clip generator + state-local exit transition + entry
  transition appended to the root state's transition array, events registered in the
  graph's own stringdata/eventInfos.
- **Entry semantics:** `NotifyAnimationGraph` return means "a transition consumed this" —
  it is the entry confirmation. `enterNotifyEvents` reach the event sink only at state
  EXIT; `exitNotifyEvents` never arrive; consumed external notifies do not echo to the
  sink. The driver already encodes this (`state_active` = notify return, cleared on
  observed `SH2_CastExit`); do not resurrect enter/exit-notify reliance.
- **The clip:** `Animations\MSCO_left1.hkx` fires `MLh_SpellFire_Event` at 0.483s (LEFT
  hand — `spellfire_mask` arms left), `MSCO_WinOpen` at ~0.8s, duration 1.667s; the shtb
  patch adds an end-of-clip trigger raising `SH2_CastExit` at −0.05s. Annotation-driven
  release is the MVP ruling.
- **Nemesis patch rules that cost builds to learn:** patch codes must be purely alphabetic
  (digit codes silently null cross-referenced new objects → load CTD; that is why the code
  is `shtb`); after every build, dissect the built state with hkxc (non-null
  generator/transitions) AND check zero literal `#shtb$N` tokens in
  `temp_behaviors/xml/<behavior>.xml` — a string grep of event names passed while the
  graph was fatal. Run via thuum's `tools/run-nemesis.ps1 -Tick shtb -UpdateEngine -Apply`
  (owner's rule: file change → Update Engine, then Build). Nemesis Output owns the merged
  files since the 2026-08-11 hygiene fix.
- **Do not touch another mod's replaced arrays.** In magicbehavior, tkds replaces the root
  SM's `wildcardTransitions`; the entry transition therefore appends to the root state's
  own transition array instead. Expect the analogous trap in the weapon graphs — find who
  replaces what before choosing an attach point.

## The work

1. **Map the weapon graphs.** Which behaviour file(s) does a drawn-weapon idle actually
   run, per weapon class, and which does MCO route attacks through? Start with
   `1hm_behavior.hkx` (MCO routes most weapon attacks through the 1hm graph — verify, do
   not assume; the T05 greatsword is the acceptance loadout, so 2H must be covered by
   whatever graph it really uses). The merged winners live in `Nemesis Output`; dissect
   with hkxc, never `strings`.
2. **Register the events per graph.** `SH2_CastRight`, `SH2_CastExit` into each target
   graph's own stringdata (`#0077`-analog) and eventInfos (`#0079`-analog) — events need no
   valueset entry; the 3-list ordinal alignment trap is variables-only.
3. **Add the state per graph.** Same three-object shape as magicbehavior's: clip generator
   (same MSCO_left1 for the slice), state-local transition array (`SH2_CastExit` → the
   graph's ready/root state), stateInfo with a large unique stateId. Entry transition
   appended to the drawn-idle root state's transition array with `FLAG_DISABLE_CONDITION`.
   Pick the return-to state per graph deliberately — the weapon graphs' "combat ready"
   state is not `MagicRoot` id 4.
4. **Driver:** nothing should need to change — `begin` already sends `SH2_CastRight`
   blind and trusts the return. Confirm the sheathed/wrong-stance failure path still tears
   down cleanly (it does today; keep it that way).
5. **Verify loop per graph:** Nemesis rebuild → hkxc dissect (non-null objects, no literal
   patch tokens, eventId of the entry transition == the event's index in that graph's
   table) → live: T05 save, greatsword drawn, `castSlot(0)` → notify true, clip plays,
   left SpellFire commit in the log, no timer floor.

## Acceptance

- The T05 save's OWN loadout (Incinerate left + greatsword right — no `equipspell`
  fixture), weapon drawn, idle: press "1" → the MCBO cast animation plays and the spell
  releases at the throw frame. Owner's eyes or continuous video; sparse frames do not
  count for the visual claim.
- A real MCO weapon swing still works untouched afterwards (MCBO coexistence check), and a
  real shout still plays SYHO's animation.
- Casting while sheathed still refuses cleanly (notify false, fail-safe teardown, no side
  effects).

## Live-session traps (they eat runs)

- **Sneak switches the bar**: sneaking resolves the Sneak Magic bar; empty slot logs
  `castSlot(0): skill type=0` and the driver never runs. Check the bar label in a frame.
- **Alt+Tab wedges the Alt modifier**: HUD shows the `A-` prefixed empty bar and casts
  resolve it. Fix: one Alt tap (DevBench `input`, key 56). A bare `up` does not clear it.
- Drive keys through DevBench's `input` tool with `userEvent` set; `Input.TapKey` is a
  no-op unfocused. `bAlwaysActive=1` is already in the profile. Never minimize the game
  window. Frame capture: `ancient-magic-frost-rework/tools/capture_ingame.ps1`.

## Out of scope, deliberately

- Mid-swing entry and combo-position continuity (attack3 not attack1) — ShoutMCO ticket-50
  API, thuum repo.
- Concentration (looping state + release-opened chain window) — its own ticket once this
  lands; the tier plan is in the spec's 2026-08-11 direction section.
- Weapon-specific cast clips (the hand-cast clip may look odd holding a sword — owner
  acceptance decides whether variants are needed; do not pre-build them).
- Pruning the now-dead enter/exit notify arrays from the magicbehavior patch (fold into
  the next patch iteration that touches those files anyway).

## Answer

Shipped as `nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/` — the TK Dodge distribution
pattern applied to the one graph that hosts every drawn melee stance. Owner-verified live
2026-08-11 on a copy of their latest save (`SH2_T08_WeaponCast`, Riverwood, Iron Rapier
right + Flames left): no T-poses, clean release frames, clean sheathing, no stuck
animations.

**The map (work item 1).** `1hm_behavior` is the weapon graph, singular: its root SM
(`1HM_Behavior`, vanilla `#4876`, 13 vanilla states) hosts ready/attack/block/bash/bow/
crossbow plus every combat mod's distributed states (MCO, SCAR, TK Dodge, Maxsu Block,
hotkey skills — 36 states in the merged winner). `1HM_Ready_State` (stateId 0, transitions
`#4872`) is the drawn-idle root state and the analog of magicbehavior's MagicRoot; a rapier
cast entering from it proved the stance live. No second graph needed for the slice.
Attach-point trap check: amco/hotkey/block/tkuc/tudm/zcbe all EDIT `#4872` in place and
scar/sbeef keep `#4874`'s transitions pointer intact, so appending to `#4872` merges clean —
the magicbehavior wildcard-replacement trap has no analog here (tkds edits `#0088` in
place too).

**The shape (work items 2–3).** Exactly the magicbehavior three-object shape plus the clip
trigger array: `#shtb$0` clip generator (same `MSCO_left1.hkx`), `#shtb$1` state-local exit
(`SH2_CastExit` → stateId 0), `#shtb$2` stateInfo (stateId 746002, **no** enter/exit notify
arrays — the dead pattern was not replicated), `#shtb$3` end-of-clip trigger (−0.05s).
Entry appended to `#4872` with `FLAG_DISABLE_CONDITION`, transition effect `#0091`
(`DefaultBlendTransition`, this graph's `#0082` analog). Patch bases were derived by
reverting other mods' MOD_CODE blocks and cross-checked identical from two independent
mods per object.

**The build cost this slice paid to learn:** the clip's SpellFire annotation resolves
against the HOSTING graph's event table. `MLh_SpellFire_Event` is native to magicbehavior
but absent from 1hm_behavior, so the first live cast entered, played, and fell to the
ADR-0006 timer floor ("no SpellFire event by the authored cast time"). Registering
`MLh_SpellFire_Event` in the patch's `#0085`/`#0087` fixed it: every later cast committed
on the annotation at +0.46–0.50s, zero floor deliveries. Any future graph this state is
distributed into needs the same registration.

**Live evidence (SpellHotbar2.log timestamps, 2026-08-11 22:33–22:44):** sheathed refusal
`-> false` ×2 (22:33:12, 22:43:56); drawn entry `-> true` ×5 (two scripted castSlot runs,
two more scripted in the owner demo, one from the owner's own "1" keypress at 22:40:47 —
the real input path); left SpellFire commit on every entry, no timer floor. Second cast
succeeding proves the exit transition returns to `1HM_Ready_State` (the entry transition
exists only there). DevBench recording `recording_1786505659.json` (38s) spans the first
fixed-build cast. Verified per graph-dissect: 21/21 checks (zero literal `#shtb$` tokens,
event ids == merged table indices 701/702/703 of 714).

**Owner chain matrix (2026-08-11, recorded for the follow-up tickets, not acceptance
criteria here):** attack→SH2 no chain; SH2→attack no chain; LH cast→SH2 **chains**;
SH2→LH cast no chain; attack→SH2-shout **chains**; SH2-shout→attack unclear (shout combo
window may be too strict). The no-chain rows are the ShoutMCO ticket-50 cast-intent API
and combo-position continuity — deliberately out of this ticket.

**Open acceptance cells, named per the evidence rules (review round 2026-08-11):**

- **2H (greatsword) drawn-idle entry is UNVERIFIED live.** The owner redirected acceptance
  from the T05 fixture to a copy of their latest save, whose loadout is a 1H rapier. The
  static case for 2H coverage is strong (2H attack/idle states live in the same
  `1hm_behavior` root SM entered from the same `1HM_Ready_State`, and no 2hm graph exists)
  but no greatsword cast has been driven on the new build. One `castSlot(0)` with a 2H
  drawn settles it next session.
- **"A real shout still plays SYHO" is unconfirmed on this build.** An injected Shout
  press produced no ShoutMCO arm line (likely no shout equipped in the save). The owner's
  matrix covered SH2-driven shouts, not the vanilla shout key.
- **The MCO swing check is indirect.** The owner drove attacks while building their chain
  matrix (attack→SH2 rows require working attacks) and reported no stuck animations, but
  no explicit before/after swing regression pass was run.

**ADR footprint note (standards review):** ADR-0006 and ADR-0001's deviation record
describe the shtb patch as magicbehavior-scoped; this slice grows it into a second graph.
Fold an ADR scope update into the next ADR-touching change.

**Session traps confirmed:** the wedged-Alt `A-` bar appeared after relaunch and did NOT
clear from an injected Alt tap or 0.3s hold (the documented fix presumes a different wedge
source); it proved cosmetic for `castSlot` — casts resolved the real spell throughout.
Owner fixture ruling recorded: test on a copy of the LATEST save, not the old Codex_T05
fixtures.
