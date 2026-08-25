# 35 — Retire the DLL movement capture: rooting is behavior-only

**Type:** cleanup (DLL), decided by [ADR-0015](../../../docs/adr/0015-commitment-is-a-property-of-the-behavior-state.md)

**Status:** claimed — implementation dispatched 2026-08-24; in-game cell open below.

**Blocked by:** None.

## The ruling

Owner, 2026-08-24, closing the day's scope pass: rooting goes through behavior only, "and that
also means removing all the extra cruft around what we had wired up earlier to solve the
movement thing."

## What is being removed, and why each is safe

The survey that gates this ran first, because the obvious hazard was real until checked: if any
cast type were rooted by the DLL capture *alone*, deleting it would un-root that type the same
day the owner ruled everything roots. It is not:

- **Every spell cast type enters a shtb state.** `start_cast`, `start_conc_cast`,
  `start_ritual_conc_cast`, and `start_ritual_cast` all route through `MscoCastDriver::begin()`
  (`casting_controller.cpp:895-1010`) — there is no cast path that bypasses the driver.
- **Every shtb state carries the root.** Six `BSIsActiveModifier` pairs in `1hm_behavior`
  (cast1-4, channel `#shtb$34/$35`, art `#shtb$28/$29`), five in `magicbehavior` (cast1-4,
  channel `#shtb$29/$30`), each binding `bAnimationDriven` and wrapping the state's clip in a
  modifier generator. Verified in the patch source, not assumed.
- Powers, shouts, and potions never had a DLL root here (`BaseCastingInstance::blocks_movement()`
  was already false); shouts are ShoutMCO's, thuum ticket 66.

Removed, end to end:

- The WASD capture in the input hook (`input.cpp:468` block reading
  `is_movement_blocking_cast()` and swallowing Forward/Back/Strafe presses).
- `CastingController::is_movement_blocking_cast()` itself.
- The `blocks_movement()` virtual chain: the `BaseCastingInstance` virtual, the ritual,
  concentration (plus its `m_blocks_movement` member and `blocksMovement` ctor parameter — the
  construction site passed the dual-cast flag), and ritual-concentration overrides.
- `shtb_state_blocks_movement` / `driver_cast_blocks_movement` in `combo_cache.h` and their
  tests.

Kept: `cast_intent.cpp`'s diagnostic graph-variable list naming `bAnimationDriven` —
instrumentation, not rooting.

## The behavioral deltas this accepts, on purpose

The DLL layer's timing was instance-scoped; the modifier is state-scoped. Three windows change,
and each lands on the MCO-consistent side:

- `CastingInstanceRitual` blocked into recovery (`m_cast_timer >= -0.5f`) — a tail beyond the
  state. Gone: the root now ends when the state ends, which is the rule everywhere else.
- Dual-cast concentration blocked only pre-commit (`m_blocks_movement && !m_casted`). Now the
  whole held state roots, matching the everything-roots ruling.
- The capture ate the key *press*; the modifier ignores the input and stops the translation.
  Movement keys during a cast now behave as they do during an MCO attack — buffered/steering
  rather than swallowed — which is the seamless behavior, not a regression.

## Acceptance

- [x] DLL builds; `combo_cache_test`, `clip_translation_test`, `art_data_test` pass with the
      helpers and their tests removed. (Static — see comment when implementation lands.)
- [ ] **In-game, the one cell that matters:** with the new DLL deployed, a fire-and-forget cast,
      a held channel, a ritual, and a weapon art each show zero displacement under a held
      movement key — displacement measured, not footfalls (thuum 66's lesson). A movement press
      mid-cast must not slide the player now that the key reaches the game.
- [ ] Rooting ends on state exit for all four, including a ticket-10 chain-out cut.
- [ ] Evidence names commit, DLL hash, save, profile.
