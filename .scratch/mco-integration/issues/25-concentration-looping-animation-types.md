# 25 — Concentration spells need their own looping animation types

A concentration channel must play a looping clip for as long as the player holds it. Fire-and-forget
Driver Cast types (`MSCO_left1`–`left4`, `MODE_SINGLE_PLAY`, exit on clip end) cannot represent that.

**Blocked by:** 08 (resolved). Ticket 08 parked this: looping conc state + release-opened chain
window. The 2026-08-11 spec direction is the tier plan.

**Superseded for the loop by ticket 28** (`28-hold-a-looping-state-for-a-concentration-channel.md`),
which carries the corrected design and gates it behind a spike.

**Status:** DONE — owner-accepted 2026-08-25. The loop this ticket asked for exists and works; it
was built by [ticket 28](28-hold-a-looping-state-for-a-concentration-channel.md) after this
ticket's own phase 1 failed. See "Owner acceptance 2026-08-25" at the bottom. The sections below
are kept as the diagnostic record — **the "Status: needs-triage" framing and the failed-phase-1
narrative are historical, not current.**

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


## Phase 1 failed, and why — 2026-08-23

Shape (a) was chosen and built: let the start clip end the shtb state and let the OAR idle loop
sustain the hold. The owner tested it and saw the fire-and-forget throw animation with no
concentration loop after it. **The premise was wrong, and the file lists say so.**

Counted across all 24 concentration submods:

| submod | idle clips | shout clips |
|---|---|---|
| `cast_1h_left_conc` | 0 | 5 |
| `cast_1h_right_conc` | 0 | 5 |
| `cast_1h_left_conc_idle` | 9 | 0 |
| `cast_ritual_aimed_conc` | 12 | 5 |

22 of 24 replace **shout** clips. `cast_1h_left_conc` — the plain aimed concentration loop that
Flames uses — replaces five shout clips and **no** idle clips. It cannot loop through an idle it
does not touch. The `_idle` submods are a supporting layer so turning and standing keep the
casting pose; only 13 of 24 carry any.

**The loop is vanilla's, not this mod's.** The vanilla shout graph loops the inhale — that is how
a shout charges through three words while the key is held. Upstream drove concentration through
that graph, so the channel sustained for free and the mod never shipped a looping generator. That
is why there is no `MODE_LOOPING` anywhere in it: the loop was borrowed, not absent.

This corrects the "Source audit 2026-08-23" section above, which claimed the submods "loop by
replacing the idle/locomotion set" and that each has "a `_start` and an `_idle` partner". Both are
false. That claim was inherited and built on without checking the file lists, which takes seconds.

**Consequence for the design.** A channel needs a state that is *held for the hold*. OAR chooses
which file plays; it never changes how long a state lasts. Since ADR-0006 retired the voice path,
nothing in the fork holds a state open, so no clip can loop. Two shapes remain open, neither
chosen: re-enter the shout graph for concentration only, or give the `shtb` patch its own looping
generator on the shout-inhale path so the state is the fork's while the clip stays the one the
submods already replace.

Relevant to either: ticket 05 closed on 2026-08-23 on *"no t-pose anywhere"* — which contradicts
one of ADR-0006's three reasons for retiring the voice path. Re-argue that retirement rather than
assuming it.

**Current runtime state.** `MscoCastDriver::replay` is removed, so concentration now plays its
throw clip once and returns to a normal idle, instead of restarting the throw every half second.
Neither is the channel; the old shape stuttered and the new one is inert. Restoring `replay` is a
one-line revert if the stutter is preferred while the real fix is designed.

### What shipped and stands

Independent of the loop mechanism, on their own evidence:

- A channel does not walk the cast-combo index and does not open the follow-up-press window
  (`CastShape` in `combo_cache.h`, unit-tested). Confirmed live: `commitment point
  (MLh_SpellFire_Event), shape=channel, window=false`.
- An attack during a streaming channel ends the channel instead of swinging over a beam that
  keeps streaming.
- `skse_plugin/src/game_data/cast_anim_ids.h`: ritual concentration keeps `11001` in both slots
  instead of borrowing the plain dual concentration loop at `11003`; ward keeps `1003` in both,
  with the reason recorded rather than solved with art. Checked against the submods installed on
  disk.

### Acceptance

- [ ] Hold an aimed concentration with a weapon drawn; the clip loops for the whole hold.
      **FAILED 2026-08-23** — owner observed the fire-and-forget throw, no loop.
- [ ] Release ends the channel and the pose reverts.
- [ ] A self concentration behaves the same way.
- [ ] A fire-and-forget on the same bar still uses its throw type.
- [ ] An attack during a hold ends the channel and swings, continuing the combo.
- [ ] A fast ritual concentration plays `11001`, not `11003`.
- [ ] A dual-cast aimed and a dual-cast self concentration each loop their own clip.

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

## Owner acceptance 2026-08-25 — the channel works

Owner, holding Healing from a hotbar slot: *"it works. I used healing. Pose reverted. I was
rooted. It looped great. We're good."*

Runtime corroboration from `SpellHotbar2.log`, same session:

```
15:38:04.292  msco_cast_driver.cpp:193  SH2 cast: notified SH2_CastChannel (held channel) -> true
15:38:12.001  msco_cast_driver.cpp:560  SH2 cast: channel held 7710ms; discounted from the combo sample age
```

A 7.7-second held channel with one entry and one exit — no per-cycle re-notify, which is what
separates ticket 28's held state from the `replay()` stutter this ticket started with. The same
run shows 23 `shape=fnf, window=true` commitments from ordinary hotbar casts, so the
fire-and-forget path is untouched, which was this ticket's standing constraint.

Fixture: ticket-38 DLL (11:33 build), ticket-16 stamped art pack, ticket-06 icon atlas at MO2
priority 4466; save `Save25`, profile `Nolvus Awakening`.

### Acceptance, resolved

- [x] Hold a concentration with a weapon drawn; the clip loops for the whole hold. **PASSES** on
      Healing, 7710 ms, replacing the 2026-08-23 FAILED result above.
- [x] Release ends the channel and the pose reverts. Owner-confirmed.
- [x] A self concentration behaves the same way. Healing is the self family.
- [x] A fire-and-forget on the same bar still uses its throw type. 23 `shape=fnf` casts in the
      same run.
- [x] A fast ritual concentration plays `11001`, not `11003`. Shipped in `cast_anim_ids.h` and
      recorded above.
- [x] An attack during a hold ends the channel and swings, continuing the combo. Carried by
      tickets 29 and 30, both resolved.

**Not exercised, and deliberately not held against this ticket:** the dual-cast pair `11003` /
`11004`. Healing is a single-hand self cast, so the dual slot went untested here as it did in
ticket 28. The owner closed on the family working; if a dual channel ever misbehaves it is a new
report against the shipped behavior, not this ticket reopening.

Rooting during the channel is owner-confirmed ("I was rooted"), which is the ticket-38 behavior —
and the owner's separate finding about *how* that rooting is implemented is filed as
[mco-integration 39](39-rooting-should-block-input-not-lower-body-animation.md).
