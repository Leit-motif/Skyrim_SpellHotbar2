# 37 — Ticket 36's residuals: the Ability driver has no watchdog, and the caps run on wall clock through pauses

**Type:** defect (DLL), two residual gaps from ticket 36, found by second-model review of that
diff (2026-08-25). Neither was in ticket 36's scope; both are real.

**Status:** needs-triage

**Blocked by:** None.

## 1. A wedged Ability recreates the eat-loop ticket 36 fixed for casts

`poll_watchdog()` exists only on `MscoCastDriver`. `should_retain_now()` also retains behind
`ArtDriver::is_active()` with `ArtDriver::latch_open()` as the drain — and ArtDriver has no
watchdog. A wedged Ability (`SH2_ArtStart` accepted, `SH2_ArtExit` never raised, latch closed)
keeps `should_retain_now()` true forever. The 4s latch cap then drops each press loudly, but
`offer()` still returns true and restamps on every new press, so each press is eaten, held up to
4s, and dropped: an eat-drop loop instead of an eat-forever sink. Better than before ticket 36,
still not recovery.

Fix shape: the same watchdog, on ArtDriver's state, through its existing teardown path. The
default Ability latch is `artExit` (whole clip), so the cap must clear the longest legitimate art
clip.

## 2. Both caps are wall-clock and `update_cast` does not run while paused

`now_ms()` is steady_clock; the game loop hook skips `update_cast` while `GameIsPaused()`.
Animation freezes, the clocks do not. On the first unpaused frame:

- A press retained just before a 4s+ menu visit is dropped even though the latch would have
  opened on the next animation frames.
- A healthy Driver Cast paused mid-clip for 8s+ is torn down by the watchdog on unpause — a cut
  of a cast that never misbehaved.

ShoutMCO's own caps are wall-clock too, so this matches the neighbouring system, but it is a
visible (if rare) regression on the healthy path. Fix shape: credit paused time against both
stamps (the channel hold already does exactly this via `credit_held_time`), or stamp in
game-time.

## Noted, no action needed

From the same review, judged not worth code now:

- `SH2_CastExit`'s graph-event branch does not zero `state_entered_ms`; harmless while only
  `send_entry()` raises `state_active`, and zeroing it there would add an animation-thread write
  to a main-thread field. Revisit only if that branch ever grows a second raiser.
- `local_latch_hold_expired` has no zero-sentinel (the watchdog predicate does); the call site
  always stamps before it can be reached. Footgun, not a bug.
- The watchdog's recovery frame releases the held press into a still-live cast instance, so that
  press is refused loudly rather than fired; the next press works. Documented at the call site.

## Acceptance

- [ ] A wedged Ability recovers within a bounded time without the player relaunching, proven by
      a trace like ticket 36's: watchdog line, then a later successful art or cast.
- [ ] A press retained immediately before a pause, and a cast paused mid-clip past the cap, both
      survive the unpause (no drop line, no watchdog line, cast completes).
