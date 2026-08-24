# 31 — Root the player through a hotbar-fired shout

**Type:** verification (may be zero-code)

**Status:** ready-for-agent

**Blocked by:** None.

## The rule, restated by the owner 2026-08-24

> "shouts should block movement input during the cast (committed) like mco attacks."

This is the same rule the owner set on 2026-08-06 and it is already built — **in ShoutMCO, not
here.** [thuum ticket 07](../../../../thuum-fully-animated-shouts-mco/.scratch/shout-mco-engine/issues/07-root-the-inhale.md)
resolved 2026-08-07: the engine writes `SHOUT_lock=1` and raises vanilla `moveStop` at
`SBF_ShoutStart`, holds it for the whole shout, and clears it at `shoutStop`. Route 1 gates the
`moveStart` transition on `SHOUT_lock == 0`, so a movement key held across the shout never
re-enters locomotion. Driven from a running start: 993 ms with zero footfalls against a
196–677 ms cadence, and the next footfall 9 ms after the unroot. The root is a compile-time
constant (`kRootDuringChain` in `ShoutChainEngine.cpp`), not an ini knob.

**So nothing new is designed here.** What is unproven is the fork's own path into it.

## Why this ticket exists anyway

A hotbar shout does not shout by itself: `CastingInstanceShout` swaps `selectedPower`
(`casting_controller.cpp:1461`, `CastingInstancePower` ctor) and `VoiceCastDriver` presses the
native voice button. That produces a real vanilla shout, so it should raise `SBF_ShoutStart` and
should be rooted by the same lock. **Should. Every cell in thuum ticket 07 and ticket 43 was
driven with a physical shout key**, never through the hotbar, and this fork's own
`CastingInstanceShout` inherits `blocks_movement() == false` from `BaseCastingInstance`
(`casting_controller.cpp:480`) — the fork contributes no root of its own and is relying entirely
on the other DLL noticing.

Two ways it could quietly not hold, both worth a single drive rather than an argument:

1. The proxy-power press path enters the voice state by a route that skips `SBF_ShoutStart`.
2. SH2's own WASD capture (`is_movement_blocking_cast()`, `input.cpp:468`) and ShoutMCO's
   control-map toggle (`RestoreMovement`) disagree about who hands movement back, so the player
   is freed early or left stuck.

## Acceptance

- [ ] A shout fired from a hotbar slot, with a movement key **held across the whole shout**,
      produces no locomotion — same instrument as thuum ticket 07: `FootLeft`/`FootRight`
      cadence either side of the shout, from the ShoutMCO trace with `bTrace = 1`.
- [ ] `SHOUT_lock` goes 1 at `SBF_ShoutStart` and 0 at `shoutStop` for that shout, read back
      (an undeclared variable reads 0 silently — thuum A45.9).
- [ ] Movement returns on the first frame after the unroot, and is not still blocked by SH2's
      own capture.
- [ ] Evidence names both repositories' commits, both deployed DLL hashes, the save, and the
      profile.

If all four hold, close this as met by thuum ticket 07 and record the hotbar path as covered.
If any fails, the fix belongs in ShoutMCO's engine, not in this fork — SH2 should not grow a
second root that fights the first.

## Scope note: this is a player-only rule

Neither plant reaches an NPC. ShoutMCO hooks `RE::VTABLE_PlayerCharacter[2]` and says so
(`ShoutChainEngine.cpp:3455`, "Player only for MVP"); extending it is a registration change, not
a rewrite. SH2's capture reads `ControlMap` bindings out of the player's input dispatch, which
an NPC has none of. **NPCs keep vanilla shout behavior — they walk while shouting.** If the
owner wants NPC commitment too, that is new work in ShoutMCO and gets its own ticket there.
