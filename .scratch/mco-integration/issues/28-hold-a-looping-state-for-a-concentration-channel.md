# 28 — Hold a looping state for a concentration channel

**Type:** spike, then feature (Nemesis patch + driver)

**Blocked by:** None. Ticket 25 is the failure this starts from; read its "Phase 1 failed, and
why" section and `docs/adr/0013-a-channel-loops-through-the-idle-not-the-cast-state.md` before
touching anything.

**Status:** ready-for-agent — the spike is fully specified. **The build phase is gated on the
spike's answer and must not start before it.**

## What this is

A concentration cast held from a hotbar slot must play a looping clip for the whole hold. It
does not today: the cast plays one fire-and-forget throw clip and returns to a normal idle.

The animation assets for this already exist and are correct. What the fork lacks is a **state
that is held for the length of the hold**. This ticket adds one.

## The single unknown — spike this first

> **Does OAR replace the animation on a `MODE_LOOPING` `hkbClipGenerator` that the `shtb`
> Nemesis patch authors on a vanilla shout-inhale path?**

Everything else in the design follows from a yes. A no kills the approach and the ticket falls
back to the shout-graph alternative below.

Two sub-questions ride along, and the same spike answers both:

1. **The path.** The concentration submods carry files named `1hm_shout_inhale.hkx`,
   `mt_shout_inhale.hkx`, `mt_shout_exhale.hkx`, `sneak1hm_shout_inhale.hkx`,
   `sneak1hm_shout_exhale.hkx`. Confirm the exact `animationName` string a `shtb` generator must
   carry for OAR to match it — the existing generators use `Animations\MSCO_left1.hkx`, so the
   form is probably `Animations\1hm_shout_inhale.hkx`. Verify; do not assume.
2. **Registration.** `1hm_shout_inhale.hkx` should already be in `1hm_behavior`'s animation
   list, because vanilla shouts with a 1H weapon drawn use it. If it is, no new registration is
   needed. Confirm rather than assume — a generator on an unregistered path is one of the ways
   this patch has failed before (a prior `sh2c` build compiled a state with a null generator;
   see ADR-0006).

### How to run the spike

Smallest thing that answers it. Do **not** build the concentration path to run this.

1. Add one `MODE_LOOPING` `hkbClipGenerator` to the `shtb` patch on the shout-inhale path, and
   one state that plays it, entered by a new notify event of its own. Model it on the existing
   generator, `nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$0.txt`, whose shape is a
   `name` of `SH2_CastRight_Clip`, an `animationName` of `Animations\MSCO_left1.hkx`, and a
   `mode` of `MODE_SINGLE_PLAY`. Nine generators exist across `1hm_behavior` and
   `magicbehavior`; all nine are `MODE_SINGLE_PLAY`, and there is no `MODE_LOOPING` anywhere in
   `nemesis/` or `data/` yet.
2. Run Nemesis, deploy, launch.
3. Set `SpellHotbar_SpellAnimationType` (`SpellHotbar.esp` form `815`) to `1001` and
   `SpellHotbar_isCastingConcSpell` (form `834`) to `1` by hand — Papyrus
   `GlobalVariable.SetValue` through DevBench is enough; you do not need a real cast. Set
   `SpellHotbar_CastingSource` (form `835`) to `0` for the left-hand submod or `1` for the
   right-hand one.
4. Fire the notify, and **capture a frame**. A registered clip and a live state prove the state
   runs; they never prove which animation it played. The question is whether the pose is the
   concentration channel pose or the vanilla shout inhale.
5. Hold for at least five seconds and capture a second frame, to separate "looping" from
   "played once and froze on the last pose".

**Kill gate.** If OAR does not replace it, stop, record what was observed, and re-triage against
the shout-graph alternative below. Do not build the driver side against a generator whose
animation cannot be swapped.

## The build, if the spike says yes

A concentration cast enters a **held** `shtb` state instead of the single-play Driver Cast clip
set, and leaves it on release. The state is the fork's own, so the Driver Cast bookkeeping keeps
working; only the clip and the mode change.

- Entry: a new notify of its own, not `SH2_CastRight`. Keep the fire-and-forget entries and clip
  set untouched.
- Exit: `SH2_CastExit`. `MscoCastDriver::end_channel` already sends it and already arms the
  ADR-0005 combo write-back, so a channel already hands its combo position to the swing that
  follows.
- The per-family clip choice stays OAR's job through global `815`. The plugin already sets the
  right id per family; do not add clip selection to the driver.

**Commitment point is an open design question, not a discovery.** These clips very likely do not
carry `MLh_SpellFire_Event`, which is what commits a Driver Cast today (ADR-0004, ADR-0006).
Settle it before building: either annotate the fork's own state with a trigger, or fall back to
ADR-0006's authored-cast-time floor, which exists for exactly this case. Say which was chosen
and why.

## The alternative, if the spike says no

Re-enter the vanilla shout graph for concentration only, reaching the inhale loop the submods
are already authored against. It works — it is what upstream did — but concentration becomes a
second-class path again: no combo position, no plant, no chain-out. It also reopens ADR-0006.

One of ADR-0006's three reasons for retiring the voice path was that this load order's
shout-path clips T-posed. **Ticket 05 was closed on 2026-08-23 by owner ruling — "this isn't
relevant any more. no t-pose anywhere."** Re-argue the retirement on current evidence rather
than assuming it still holds; the other two reasons concerned hand casts and bear less on a
channel.

## Do not re-derive these

**The loop is vanilla's, not this mod's.** The vanilla shout graph loops the inhale — that is
how a shout charges through three words while the key is held. Upstream Spell Hotbar 2 drove
concentration through that graph, so the channel sustained for free. That is why the mod ships
no `MODE_LOOPING` generator: the loop was borrowed, not absent.

**What the concentration submods replace.** Counted across all 24 under
`meshes/actors/character/OpenAnimationReplacer/SpellHotbar2/`:

| submod | idle clips | shout clips |
|---|---|---|
| `cast_1h_left_conc` | 0 | 5 |
| `cast_1h_right_conc` | 0 | 5 |
| `cast_1h_left_conc_idle` | 9 | 0 |
| `cast_ritual_aimed_conc` | 12 | 5 |

22 of 24 replace shout clips. The `_idle` submods are a supporting layer so turning and standing
keep the casting pose; only 13 of 24 carry any. **The idle set is not the loop** — that was
ticket 25's failed premise.

**Coverage.** Every concentration id has submods: 1001 aimed, 1002 self, 1003 ward, 11001
ritual, 11003 dual, 11004 dual self, with left, right, staff and ward variants and a `_start`
partner gated on `834 == 0` against the loop's `834 > 0`.

**Conditions each submod tests:** `815` equals the family id, `835` equals 0 for left or 1 for
right, `834 > 0` for the loop and `== 0` for the `_start`, plus `IsPlayer`. Casting-source
values are `RE::MagicSystem::CastingSource` — left 0, right 1.

**OAR priority is settled.** Only Spell Hotbar 2 and its own overlay replace these files;
nothing else in the load order contests them. `Spell Hotbar 2 - OAR Priority Over SYHO` was
**enabled by the owner on 2026-08-23** and sits above both Spell Hotbar mods, lifting the
concentration submods from 99010010 to 101010010 — clear of SYHO's 99999990. If a clip fails to
swap, priority is not the reason; check the conditions and the path.

**Driver API already in place** (`skse_plugin/src/casts/`):

- `MscoCastDriver::begin(pc, hand, charge_time, CastShape)` — `CastShape::channel` already
  suppresses the cast-index walk and the follow-up-press window.
- `MscoCastDriver::end_channel(pc)` — arms the combo restore, sends `SH2_CastExit`.
- `CastingInstanceSpellConcentration::end_channel(pc)` and `is_streaming()`.
- `CastingController::is_channel_chainable()` and `cut_channel_for_attack(pc)` — an attack
  during a streaming channel ends it and travels on as an ordinary swing.
- Pure predicates and their tests: `combo_cache.h` and `combo_cache_test.cpp`.

**Live-verified 2026-08-23:** `commitment point (MLh_SpellFire_Event), shape=channel,
window=false` — the channel path is reached and the shape split works. The state exits at clip
end and the channel ends separately at release with `combo restore armed next=2`.

## Do not do this

- Do not create, retarget, import, or redistribute animation assets. Owner scope decision,
  2026-08-23, unchanged.
- Do not try to sustain the channel through the idle set. It was built, tested and failed.
- Do not touch the fire-and-forget path. It works and is verified.
- Do not restore `MscoCastDriver::replay`. It restarted the single-play throw clip every half
  second, which is a stutter, not a loop.

## Spike result — 2026-08-23

**The spike's question is answered YES: OAR replaces the animation on a `MODE_LOOPING`
generator the `shtb` patch authors on the shout-inhale path.** The held-state half is only
partly proven and carries one open defect; see below.

Frames live under `.scratch/mco-integration/evidence/28-channel-spike/` (gitignored, local).

### The path and the registration — both settled, and the ticket's phrasing was slightly off

`AnimationsHM_Shout_Inhale.HKX` is the right path, and it is better than "probably". It is the
**only** clip every non-idle concentration submod replaces — all 22 of them carry
`1hm_shout_inhale.hkx`, and nothing else is common to all. One generator therefore covers every
family, with no per-family clip selection in the patch.

Registration needed nothing new, but not for the reason the ticket gave. The animation list is
**per character, not per behavior**: `AnimationsHM_Shout_Inhale.HKX` lives in `defaultmale.hkx`
and `defaultfemale.hkx` `hkbCharacterStringData::animationNames`, and is played in vanilla by
`shout_behavior`, not by `1hm_behavior`. A clip generator in `1hm_behavior` or `magicbehavior`
resolves it fine because the list is shared across the project's graphs.

### What the patch builds

`SH2_CastChannel` enters `SH2_Channel_State` (id 746006) in both `1hm_behavior` and
`magicbehavior`. The clip is `MODE_LOOPING` with `triggers null`, so there is no end-of-clip
trigger to raise `SH2_CastExit` each cycle, and `variableBindingSet null` so a stale
`MSCO_attackspeed` from the previous cast cannot rescale the loop. Exit is `SH2_CastExit`, plus a
`shoutStart` escape. The fire-and-forget clip set and its entries are untouched.

**`eventNames` and `eventInfos` are parallel arrays.** Adding `SH2_CastChannel` to `#0085` /
`#0077` needs a matching entry in `#0087` / `#0079`. Without it Nemesis compiles the XML and then
fails both graphs at serialization with `ERROR(1003): Failed to output hkx file`, naming only
`magicbehavior.xml`. Worth knowing: the error names one graph and the real fault was in both.

### Evidence — OAR swap, proven by A/B/A on global `815`

Weapon drawn, `835 = 0`, `834 = 1`, entered by `player.sae SH2_CastChannel`:

| Frame | `815` | Pose |
|---|---|---|
| `t28-01-drawn` | — | drawn idle, before entry |
| `t28-02-channel-t0` | 1001 | out of idle, right arm extended |
| `t28-04-anim0-nomatch` | 0 | arms in, upright — the vanilla shout inhale |
| `t28-05-anim1001-again` | 1001 | arm extended forward again |

The control matters: with `815 = 0` no submod's conditions match, so the generator plays vanilla
`1HM_Shout_Inhale`. Flipping the global back reproduces the swapped pose. The clip on the fork's
own looping generator is therefore chosen by OAR at runtime, from the global, exactly as the
fire-and-forget path's clips are. That is the whole premise the build depends on.

### Open defect — the state does not hold for the whole hold

`t28-h3` shows the channel pose live at +3s. `t28-h6` shows the drawn idle at +6s, with no
`SH2_CastExit` sent by anything. So the state survives well past a single play of a ~1s clip, and
a `MODE_LOOPING` generator cannot end itself, yet something leaves the state between +3s and +6s.

Not diagnosed. Three candidates, cheapest first:

1. A **wildcard transition** on the root state machine. Wildcards fire from any state, so a
   vanilla entry — an idle-manager force, `IdleForceDefaultState`, a locomotion event — can pull
   the actor out of a state whose own transition array never lists it.
2. The **`shoutStart` escape this patch added** to the channel state's transitions. It is the one
   non-essential exit in the patch and the cheapest to remove and re-test.
3. An **annotation in the replacement clip**. The concentration clips are authored for vanilla's
   inhale state, so any event they carry now arrives in a graph that reads it differently.

The driver's own graph trace would name the event outright, and it did not run here: this test
fired the notify by hand, so `MscoCastDriver::is_active()` was false and
`should_trace_graph_events()` never returned true. Wiring `begin()` to send `SH2_CastChannel` for
a channel turns that trace on, which makes this defect cheap to diagnose during the build rather
than before it.

### Commitment point — decision

Use **ADR-0006's authored-cast-time floor**, not an annotation on the fork's state. The clip that
plays is chosen by OAR per family at runtime, so a trigger authored on this patch's generator
would fire at a time unrelated to whichever clip is actually playing. The floor is the charge time
the spell already declares, and `CastingInstanceSpellConcentration::update` already commits on it:
once `m_cast_timer` passes `m_release_anim_time` the charge finishes with no SpellFire needed.
`is_anim_ok()` stays true throughout because the state is live, so the charge loop will not cancel.

One driver change this implies, for the build: `observe_graph_event`'s `SH2_CastExit` branch warns
`press produced no payload` and resets the cast index when `clip_committed` is false. A channel
never sets it, so a channel would reset the fire-and-forget combo index on every release. Gate
that branch on `CastShape::fire_and_forget` — a pure predicate, so it belongs in `combo_cache.h`
with a test beside the existing ones.

## Build status — 2026-08-23, end of session

The held state works and the clip is correct. Combo hand-off is **not** finished; it has a
diagnosed, still-open defect. Do not close this ticket.

### Shipped and owner-confirmed

- `SH2_CastChannel` enters `SH2_Channel_State`; the state is held for the whole hold and ends on
  release. Log: `notified SH2_CastChannel (held channel) -> true`, then `SH2_CastExit -> true`
  4.6s later with no `state exiting` between them.
- **Flames looks right.** Owner 2026-08-23: *"Flames works perfect now!"*
- A fire-and-forget cast on the same bar is unchanged (`shape=fnf, window=true` in the same run).

### The bug that hid the whole thing, and the false claim that caused it

`Spell Hotbar 2 - MSCO Cast Animations` ships `SpellHotbar2_MCBO/cast_left_probe` — a 16-clip
diagnostic left over from the 2026-08-11 voice-driver work (see
`notes/05-progress-2026-08-11-voice-driver.md`; these are the clips that T-posed). It carries
**priority 101500000** against the concentration submods' 101010010, and its only condition is
`815 > 0` — a catch-all matching every animation type. It therefore won `1hm_shout_inhale.hkx`
and the channel looped an MCBO **cast** clip, which is what the owner saw as the fire-and-forget
animation.

This ticket's own claim — *"Only Spell Hotbar 2 and its own overlay replace these files; nothing
else in the load order contests them"* — was false, and it is why the spike passed while the real
cast failed: the spike's A/B/A compared two wrong clips and read the difference as success. The
real audit, by submod count on `1hm_shout_inhale.hkx`: Spell Hotbar 2 86, its priority overlay 86,
Dev - Thuum Reborn 18, SYHO 7, Goetia Conditional Shouts 1, the probe 1. The path is shared;
safety rests on conditions, not exclusivity.

**Fixed by** renaming the probe's `config.json` to `config.json.disabled` (clips left on disk;
revert by renaming back). This is a live-fixture change, not a repository one — a fresh install
of that local mod would reintroduce it.

### Open defect — the restored combo index is 1

`attack1 → hold Flames → attack` still yields attack1. Two separate causes, one fixed:

1. **Fixed.** `RollingMcoCombo::kMaxAgeMs` is 5000ms, sized for a one-to-two-second throw clip. A
   channel is unbounded and takes no samples while our state is live, so any hold past five
   seconds aged the pre-hold sample out and armed nothing at all. The hold is now credited back
   at release; a gap *before* the hold still ages out. Log line: `channel held NNNNms; discounted
   from the combo sample age`.
2. **Open.** The restore now always arms, but the sampled value is itself frequently `1`, so
   restoring it faithfully reproduces attack1. One cut in the owner's run armed `next=2`, so the
   value is timing-dependent. The sample is taken in `observe_graph_event` at
   `MCO_AttackInitiate` / `MCO_PowerAttackInitiate` / `HitFrame`, whichever lands last before the
   channel; the suspicion is that this reads `MCO_nextattack` before MCO advances it, so a swing
   cancelled early into a cast samples the current index rather than the next one.

**A debug line is already built and deployed for exactly this** and has never been read: `SH2
cast: sampled MCO next={} power={} at {tag}`. Next session: one clean `attack1 → hold → attack`,
then read which tag produced which value. That decides whether the fix is choosing a different
attack-time event, or deriving the successor rather than preserving the sample — the latter would
contradict `RollingMcoCombo`'s "preserve, never derive" rule, so prefer the former.

### Still untested

Self `1002`, ward `1003`, ritual `11001` including the fast-cast slot, dual `11003` and `11004`,
release-reverts-the-pose, and the t0/t+3s frames this ticket asks to be committed. All need the
owner's bindings.

## Acceptance

Live only. A build or a static check diagnoses; it never proves an animation.

- [ ] **Spike:** a `MODE_LOOPING` generator on the shout-inhale path is replaced by OAR, shown
      by a captured frame of the concentration pose, plus a second frame five seconds later
      proving it loops rather than freezing.
- [ ] Hold an aimed concentration (Flames, `1001`) with a weapon drawn; the clip loops for the
      whole hold. Frames at t0 and t+3s, committed and cited by path.
- [ ] Release ends the channel and the pose reverts.
- [ ] A self concentration (`1002`) loops its own clip.
- [ ] A ward (`1003`) loops its own clip.
- [ ] A ritual concentration (`11001`) loops its own clip, including the fast-cast slot.
- [ ] A dual-cast aimed (`11003`) and a dual-cast self (`11004`) each loop their own clip.
- [ ] A fire-and-forget on the same bar still plays its throw clip, unchanged.
- [ ] An attack during a hold ends the channel and swings, continuing the combo.

## Closed 2026-08-24 — owner's call

The animation half is done and owner-verified (channel state `f974fee`, per-family loops, the
acceptance rows above through "fire-and-forget unchanged"; `11004` and the t0/t+3s frames remain
untested and travel with ticket 29/30 verification sessions). The last cell — an attack during a
hold continuing the combo — now WORKS on 1H when the cast lands after the interrupted swing's
advance (owner OAR screenshot 2026-08-24 14:35: a1 → a2 → flames hold → attack3), and the rest
of that cell is carved out per the owner:

- **Ticket 29** — an interrupted swing counts toward the combo (pre-advance interrupts replay
  the swing; owner ruling: the user sees animations, not hit frames).
- **Ticket 30** — 2H attack entry ignores the restored index entirely, and 1H's success is
  unexplained; the owner's bar for resolution is a mechanism, not a passing run.

Diagnosis corpus: `notes/28-progress-2026-08-24-combo-fix.md`,
`notes/28-entry-seam-analysis-2026-08-24.md` (rounds 1–2). Handoff (b)'s ADR is deferred into
ticket 30.
