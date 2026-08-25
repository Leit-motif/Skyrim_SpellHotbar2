# 38 — Root fire-and-forget casts instead of sliding them

**Type:** defect (presentation) / owner ruling

**Status:** resolved — all cells green 2026-08-25; owner verdict spawned follow-on ticket 39

**Blocked by:** None.

## How this showed up

Owner, 2026-08-25: the fire-and-forget cast animations slide the character forward with no foot
movement. Ruling: either the animations gain real foot movement (ideal), or the character roots
in place (fallback). The ideal path needs authored or sourced stepping animations — the same
new-asset wall that parked tickets 32 and 34 — so it is parked as ticket 40 (renumbered from
39) and this ticket builds the fallback.

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

- [x] Agent: Driver Cast of a fire-and-forget spell shows XY delta ≈ 0 across the clip
      (scene telemetry), where before the fix it stepped. 2026-08-25 15:44.
- [x] Agent: Disengage (`slotArt` + `castSlot`) still translates ~318 units. Measured ~315.
- [x] Agent: cast still commits (SpellFire fires, combo advances) — rooting changed nothing
      functional. `MLh_SpellFire_Event`, `shape=fnf, window=true`.
- [x] Owner: no forward glide on fire-and-forget hotbar casts. Owner 2026-08-25 via the
      acceptance pass: "Forward Glide is gone" — with the correction that spawned ticket 39.
- [x] Restore fixtures (`slotArt(slot, 0)` done) — Skyrim left RUNNING by agreement: the
      ticket-16 sweep re-run held the next fixture-queue slot.

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

**2026-08-25 15:45 — agent: all cells green. Resolved.**
Telemetry window taken right after the owner's pass (queue reordered: my window came before the
ticket-16 re-run). PID 25652, Save25's world, Embershard wilderness, weapon drawn, deployed
ticket-38 DLL (11:33 build).

| Cell | Result |
|---|---|
| FF driver cast, owner's slot 3 (skill type 3) | `SH2_CastRight -> true`, `MLh_SpellFire_Event` commit (`shape=fnf, window=true`), `SH2_CastExit`; Z bobbed with the clip; **XY frozen to the last digit** (9139.322265625, -56564.14453125) across pre/0.3/0.6/1.2/2.0s; **no** `SH2 motion: bound SH2_CastRight_Clip` line — the cast clip never bound |
| Disengage (`slotArt(0,12)` + `castSlot(0)`) | bound `SH2_Art_Clip` (140 animmotion keys, ticket-16 **stamped** copy — bytes differ from the author's file, data verified identical by that ticket), leapt (9139, -56564) → (8878, -56739) ≈ **315 units** with the known settle-back; art motion not regressed |
| Fixture restore | `slotArt(0,0)`; in-memory only, no save written |

Owner verdict (relayed from the acceptance pass): "Forward Glide is gone" — cell passed — with
the correction that rooting was only ever meant to block input, not freeze the lower body. The
leg freeze predates this ticket (the original report was "slides forward, no foot movement"),
and is now ticket 39, which this ticket's Disengage result already narrows away from the
animmotion layer toward the `bAnimationDriven` state modifier.

Earlier anomaly closed: art 15 playing with 160 keys and no visible XY during the contaminated
window was the art being near-stationary in XY, not a motion regression.
