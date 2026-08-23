# 25 — Concentration spells need their own looping animation types

A concentration channel must play a looping clip for as long as the player holds it. Fire-and-forget
Driver Cast types (`MSCO_left1`–`left4`, `MODE_SINGLE_PLAY`, exit on clip end) cannot represent that.

**Blocked by:** 08 (resolved). Ticket 08 parked this: looping conc state + release-opened chain
window. The 2026-08-11 spec direction is the tier plan.

**Status:** needs-owner-verification — built 2026-08-23. Both phases and the dual-distinctness
slice are implemented and the unit suite passes; every acceptance cell is live evidence the owner
has to make. See "What shipped" below.

## Owner ask (2026-08-21)

> concentration spells need their own looping animation types

## You test this

(Unwritten until triage.) Hold Flames (or another known concentration) from a hotbar slot with a
weapon drawn. The clip loops for the hold. Release ends the channel. A fire-and-forget on the same
bar still uses its existing throw type, not the loop.

## What this is

Dedicated **animation types** for concentration so the Spell Editor / `SpellHotbar_SpellAnimationType`
picker and OAR can choose looping clips, instead of mapping conc onto the F&F throw set.

Today `chose_default_anim_for_spell` already assigns different ids for conc (aimed 1001, self 1002,
ritual 11001, ward 1003, plus dual 11003 / 11004). Those ids pick OAR submods, and **the submods
exist** — see the source audit below. The shtb Driver Cast clip generators are still single-play and
the state exits when the clip ends, so a concentration cast that goes down the Driver Cast path dies
with the first loop of a throw anim. Ticket 11’s `replay()` re-notifies `SH2_CastRight` and explicitly does
not walk the combo clip set; that is a restart, not a loop.

Related: ticket 05 (SYHO) — SYHO’s clip does not loop; conc in this load order stays broken until
this mod’s looping clips win.

## Source audit 2026-08-23 — the loop exists; the Driver Cast path bypasses it

Checked against the installed `Spell Hotbar 2` mod and the plugin source, to settle whether this is
a repair or a build. **It is a repair.**

`Spell Hotbar 2` ships **24 concentration OAR submods** under
`meshes/actors/character/OpenAnimationReplacer/SpellHotbar2/`, keyed on
`SpellHotbar_SpellAnimationType` (`SpellHotbar.esp` formID `815`) — 111 submods read that global in
total. Every concentration id is covered: 1001, 1002, 1003, 11001, 11003, 11004. Coverage spans
left / right / dual / ritual-aimed, each with self, staff and ward variants, and each with a
`_start` and an `_idle` partner.

**How the loop is built.** These submods do not add a looping clip generator. They replace the
**idle and locomotion set** — `1hm_idle.hkx`, `dw1hm1hmidle.hkx`, `staff_idle.hkx`, `h2h_idle.hkx`
and the whole `mt_turn*` / `magcast_turn*` family — with `shout_inhale` / `shout_exhale` taking the
`_start`. The channel loops because the *idle state* loops. That is why it works with no
`MODE_LOOPING` generator anywhere in the mod.

**Where it breaks.** Every clip generator in the shtb Nemesis patch is `MODE_SINGLE_PLAY` — all nine,
across `1hm_behavior` and `magicbehavior`, with no `MODE_LOOPING` in `nemesis/` or `data/`.
`start_conc_cast` builds a dedicated `CastingInstanceSpellConcentration` and then hands off to
`MscoCastDriver::begin`, the same driver fire-and-forget uses. So the cast bookkeeping is
concentration-aware while the animation is a single-play Driver Cast clip that ends and exits the
state, and the idle-state loop never gets its turn.

Fixing that is integration, selection or configuration work — which path a concentration cast takes,
and what the shtb state does when it gets one. It is not an asset problem, which is what the owner's
scope decision below already says.

**Dual is covered but not fully distinct.** `cast_dual_conc` (11003) and `cast_dual_conc_self`
(11004) exist and are complete. The gap is upstream, in `skse_plugin/src/game_data/spell_cast_data.cpp`
lines 89-92: the dual slot of `cast_anims_aimed_conc` and `cast_anims_ritual_conc` is the **same id,
11003**, so a dual-cast ritual concentration plays the plain dual concentration clip. Ward has no
dual id at all — the branch hardcodes 1003 with the comment "ward has no dual cast". This mirrors the
fire-and-forget convention, where aimed and ritual share 10016 and self and ritual-self share 10017,
so the dual slot is a shared animation by design rather than a per-family one. Owner 2026-08-23 wants
every cast type integrated, so that convention is now in scope to change.

## What to build (2026-08-23)

Two linked phases, per the owner's triage. Both are integration work inside the fork; no new or
imported animation assets, per the scope decision.

**Phase 1 — sustain the loop.** A concentration cast started from the hotbar must not die with
the single-play Driver Cast clip. The repair happens at the seam the source audit names: which
path a concentration cast takes, and what the shtb state does when it gets one. Two candidate
shapes, choose by evidence, not preference:

- (a) The conc cast plays its `_start` and then **exits the shtb state into the idle loop** the
  24 existing OAR submods already own (they loop by replacing the idle/locomotion set keyed on
  `SpellHotbar_SpellAnimationType`). The channel's liveness stays with
  `CastingInstanceSpellConcentration`; release restores the animation-type global and the idle
  reverts.
- (b) The shtb state is **held for the hold** (re-notify / replay against the same clip) and
  exits on release. `msco_cast_driver::replay` exists but is a restart, not a loop — measure
  whether the seam it plays is visible before choosing this.

Whichever shape wins: exit on button release, and the existing fire-and-forget path must be
byte-for-byte untouched.

**Phase 2 — chain window at release.** When the hold ends, open the cast-combo window the same
way a fire-and-forget clip does at SpellFire→WinClose (ticket 22's clock), so a channel can hand
off to an attack. Channels stay out of the cast-index walk (ticket 11's rule stands; the owner's
"don't redesign the runtime" confirms it).

**Dual distinctness (either phase, smallest diff wins).** In
`skse_plugin/src/game_data/spell_cast_data.cpp:89-92`: ritual-conc's dual slot currently reuses
11003 and ward hardcodes 1003 with no dual id. Extend the family distinctness through the dual
slot — new ids are configuration mapping onto **existing** submods, not new assets. Where no
submod exists for a combination (ward dual), record the mapping chosen and why rather than
inventing art.

**Evidence.** Live only: hold Flames (aimed conc) and a self conc with weapon drawn — the loop
sustains for the whole hold (captured frames at t0 and t+3s or a short recording), release ends
it, a fire-and-forget on the same bar still uses its throw type. Dual-cast a ritual conc and
confirm it no longer plays the plain dual conc clip. A build or static check diagnoses, never
proves.


## What shipped 2026-08-23

Shape **(a)**, on the evidence in the source audit above. All of it is plugin work; no animation
asset was created, retargeted, imported, or redistributed, and the fire-and-forget path is
untouched.

**Phase 1 — sustain the loop.** `MscoCastDriver::replay` is gone. It re-sent `SH2_CastRight`
every half second whenever the state had ended, which restarted the single-play start clip and
held the actor out of the drawn idle — the one state that owns the real loop. A channel now
enters the state for its start clip, the clip's own end-of-clip trigger ends the state, and the
24 concentration OAR submods sustain the hold by replacing the idle and locomotion set while
`SpellHotbar_isCastingConcSpell` is raised. Release clears that global through the existing
`reset_animation_vars()` and the idle reverts.

`MscoCastDriver::begin` now takes a `CastShape`. A channel's SpellFire commits the cast but does
not walk SH2's cast-combo index and does not open the follow-up-press window; a fire-and-forget
cast does both, exactly as before. That is ticket 11's rule, now enforced at the driver rather
than by a comment on a function that walked around it.

**Phase 2 — chain out at release.** Written as "open the cast-combo window at release". Under
shape (a) the literal window bit cannot be the mechanism: `combo_window` is only read together
with `MscoCastDriver::is_active()`, and a channel deliberately holds no state, so the flag would
be inert. What the phase asks for — a channel can hand off to an attack — needed two things
instead, and both shipped:

- `CastingController::cut_channel_for_attack`: an attack press during a streaming channel ends
  the channel and travels on to the game as an ordinary swing. Before this the press started a
  swing while the beam kept streaming. The press is not captured, the same fail-safe ticket 10
  uses.
- The channel arms the ADR-0005 combo write-back when it ends, so the swing after a hold
  continues the chain the hold interrupted instead of starting at hit 1. A hold longer than
  `RollingMcoCombo::kMaxAgeMs` has no fresh sample and hands nothing on, which is honest rather
  than invented.

The window's shape is therefore the hold: it opens at the commitment point and closes when the
player lets go or attacks (`channel_chain_window_open` in `combo_cache.h`).

**Dual distinctness.** The two-id family table moved out of `spell_cast_data.cpp` into
`skse_plugin/src/game_data/cast_anim_ids.h`, a pure header with its own test. Ritual
concentration's variant slot no longer borrows `11003`: it ships one submod
(`cast_ritual_aimed_conc`) and now keeps `11001` in both slots, so a fast ritual channel stops
playing the plain dual concentration loop. Ward concentration keeps `1003` in both slots, and the
reason is recorded in the header rather than solved with art: there is no dual ward submod and
vanilla has no dual ward pose to map onto. Aimed (`1001` / `11003`) and self (`1002` / `11004`)
were already distinct and are unchanged. Fire-and-forget ids are unchanged and pinned by test.

The shape decision is recorded as `docs/adr/0013-a-channel-loops-through-the-idle-not-the-cast-state.md`,
and `CONTEXT.md` gains the term **Cast Channel**.

### Files

- `skse_plugin/src/game_data/cast_anim_ids.h`, `cast_anim_ids_test.cpp` (new)
- `skse_plugin/src/game_data/spell_cast_data.cpp`
- `skse_plugin/src/casts/combo_cache.h`, `combo_cache_test.cpp`
- `skse_plugin/src/casts/msco_cast_driver.h`, `msco_cast_driver.cpp`
- `skse_plugin/src/casts/casting_controller.h`, `casting_controller.cpp`
- `skse_plugin/src/input/input.cpp`
- `skse_plugin/CMakeLists.txt`, `docs/adr/0013-…`, `CONTEXT.md`

### Evidence so far, and what it is not

Release build clean; all six unit test binaries pass (`cast_anim_ids_test`, `combo_cache_test`,
`clip_translation_test`, `art_data_test`, `art_bind_record_test`, `bind_drop_test`). The DLL is
deployed to `Dev - Spell Hotbar 2`.

That is a diagnosis, not proof. Every cell below is live evidence and none of it is taken:

- [ ] Hold Flames (aimed conc, `1001`) from a hotbar slot with a weapon drawn. The clip loops for
      the whole hold — frames at t0 and t+3s, or a short recording, committed and cited by path.
- [ ] Release ends the channel and the idle reverts.
- [ ] A self concentration behaves the same way (`1002`).
- [ ] A fire-and-forget on the same bar still uses its throw type.
- [ ] An attack pressed during a hold ends the channel and swings, continuing the combo.
- [ ] A fast ritual concentration no longer plays the plain dual concentration loop (`11001`, not
      `11003`).
- [ ] A dual-cast aimed and a dual-cast self concentration each loop their own clip
      (`11003` / `11004`).

## What this is not

Not Ability / Custom Ability Spell (weapon-arts 12). Concentration is excluded from that picker.
Not ticket 22’s F&F SpellFire→WinClose window. Not native left-hand MSCO (ticket 24).

## Triage

Answer these before `ready-for-agent`:

1. **Graph vs type.** Is this (a) new looping animation type ids + OAR clips on the existing shtb
   states, (b) a dedicated looping conc state in the shtb patch (spec tier 1), or (c) both?
2. **Clip source.** Author new loops, reuse vanilla / MSCO concentration loops, or retarget an
   existing pack? Do not ship someone else’s HKX without an answer.
3. **Release.** Spec tier 2 is a chain-out window opened at button release. Same ticket, or a
   follow-up after the loop is visible?
4. **Combo.** Ticket 11 left channels out of the cast-index walk. Keep that, or does a looping
   type also need combo-position rules?
5. **First person / ritual / ward.** Same looping type family, or separate ids as today
   (1001 / 1002 / 11001 / 1003)?

## Notes

Spec 2026-08-11: (1) looping conc state, exit on release; (2) chain window at release; (3)
combo-position continuity. This ticket is the owner’s animation-type slice of that path.

Owner 2026-08-23 triage: both a sustained loop and release-side chain behavior are necessary,
but they may land as two linked phases. Preserve distinct looping animation families for aimed,
self, ritual, and ward concentration casts; do not consolidate their existing categories.

Owner 2026-08-23, on dual: cover dual casts too — the assumption was that SH2 already handled them
natively. It half does (see the source audit): the dual submods exist, but two families share id
11003 and ward has no dual id. **All types of casts need to be integrated.** This answers Triage
Q5 — keep the families distinct, and extend that distinctness through the dual slot.

Owner 2026-08-23 scope decision: do not create, retarget, import, or redistribute animation
assets, and do not redesign Spell Hotbar's runtime. Treat the installed casting and looping
behavior as the source of truth; diagnose and correct only the existing integration, selection,
or configuration that prevents it from functioning as intended.

Owner 2026-08-23 symptom confirmation: this is the previously identified concentration-looping
failure, not a new regression or feature request. The expected existing loop is not sustained
while the channel is held; triage it as a repair.

`msco_cast_driver::replay` comment: “looping channel does not walk the clip set; ticket 11 leaves
channels out.”

## Comments

Owner 2026-08-21: filed from the post-09 punch list. Not specified enough to implement.
