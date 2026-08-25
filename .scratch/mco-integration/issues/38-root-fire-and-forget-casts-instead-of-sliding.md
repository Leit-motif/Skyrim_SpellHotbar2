# 38 — Root fire-and-forget casts instead of sliding them

**Type:** defect (presentation) / owner ruling

**Status:** claimed

**Blocked by:** None.

## How this showed up

Owner, 2026-08-25: the fire-and-forget cast animations slide the character forward with no foot
movement. Ruling: either the animations gain real foot movement (ideal), or the character roots
in place (fallback). The ideal path needs authored or sourced stepping animations — the same
new-asset wall that parked tickets 32 and 34 — so it is parked as ticket 39 and this ticket
builds the fallback.

## What is already true (do not re-derive)

The slide is our own DLL push. Weapon-arts ticket 05 made `ClipTranslationDriver` consume a
clip's `animmotion` keys and `SetPosition` the player, because Disengage's ~3 m leap **is** the
art. The four cast clips (`MSCO_left1`–`4` via `SH2_CastRight_Clip` / `SH2_Cast2/3/4_Clip`) got
the same consume so a Driver Cast would match a native MSCO cast's small lunge (ticket 21's
acceptance). Those clips animate no stepping, so the translation reads as a glide.

Rooting needs no new mechanism: the `bAnimationDriven` graph wrap (ticket 21, kept by ticket 35)
plants the character at `max_xy = 0` on its own — measured repeatedly on Save65/72 before ticket
05 landed. Removing the DLL push for cast clips restores exactly that state.

## The work

In `skse_plugin/src/casts/clip_translation_driver.cpp`, bind motion only for `SH2_Art_Clip`.
The cast clips (`SH2_CastRight_Clip`, `SH2_Cast2_Clip`, `SH2_Cast3_Clip`, `SH2_Cast4_Clip`)
no longer bind, so `apply()` never translates a Driver Cast. Arts keep their motion untouched.
The `MscoCastDriver::is_active()` arm of the `apply()` gate goes with it — with no cast clip
ever bound, it can never fire with keys present.

## Trade-off accepted

A **native** MSCO left-hand cast (casting without the hotbar) still takes its lunge from AMR;
SH2 does not own that path. Hotbar casts and native casts will look different again — the exact
asymmetry ticket 21 chased, now inverted by the owner's preference. Recorded here so nobody
reopens 21 to "fix" it.

## What this is not

- Not un-planting WASD (ticket 19/21 wrap unchanged).
- Not touching art motion — Disengage must keep leaping.
- Not editing hkx files or OAR configs, and not a new animation (that is ticket 39).

## Acceptance

- [ ] Agent: Driver Cast of a fire-and-forget spell shows XY delta ≈ 0 across the clip
      (scene telemetry), where before the fix it stepped.
- [ ] Agent: Disengage (`slotArt` + `castSlot`) still translates ~318 units.
- [ ] Agent: cast still commits (SpellFire fires, combo advances) — rooting changed nothing
      functional.
- [ ] Owner: no forward glide on fire-and-forget hotbar casts. Visual, owner's eyes.
- [ ] Restore fixtures (`slotArt(slot, 0)`) and close Skyrim after agent-only telemetry.

## Comments

**2026-08-25 — agent: built, deployed, runtime cells pending a fixture window.**
Commit `0da06b7`: cast clips no longer bind in `ClipTranslationDriver`, `MscoCastDriver` gate
arm removed, unit tests green. DLL deployed to `Dev - Spell Hotbar 2` 11:33 and is the running
build. First telemetry attempt was contaminated: the ticket-16 art sweep was driving the same
instance (slotArt/castSlot interleaved with my samples), and a later `castSpell` probe was a
silent no-op with the player parked in the river at Z −342 — no log line, no movement, casts
refused. Moved to QASmoke; usage ran out before the cells landed.

Open observations for the runtime session:
- An art with 160 animmotion keys (art 15) played during the contaminated window with no
  visible XY. Either that art is near-stationary or the gate edit regressed art motion — the
  Disengage cell (~318 units, known) decides.
- Disengage's clip is now ticket-16's stamped byte-unique copy (backup at
  `C:\Nolvus\_backups\art-pack-prestamp-20260825`); animation data verified identical, but
  cite the stamped file in evidence.
- Ticket-16's sweep saw 30/57 presses dropped by the ticket-36 4s latch cap. If a driver
  state outlives its clip in my cells, tell spell-hotbar-2-f1 — may be the same defect.

Fixture queue (agreed cross-session): owner manual pass (tickets 25/06/14) → ticket-16 sweep
re-run → my ~5-min window (slotArt/castSlot slot 0, cleared after; no relaunch, no deploy).
Owner-eyes cell may close during the owner's pass: any hotbar FF cast, weapon drawn — glide
gone?
