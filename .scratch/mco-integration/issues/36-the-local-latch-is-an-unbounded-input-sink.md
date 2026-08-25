# 36 — The local cast latch is an unbounded input sink: one wedged driver eats every press forever

**Type:** defect (DLL) — affects ALL hotbar functionality, not shouts

**Status:** closed — both fixes landed and verified in-game (2026-08-25).

**Blocked by:** None.

## The report

Owner, 2026-08-25, mid-combat: *"everything was initially working, but after a while, due to more
frantic presses, trying to attack before the window was up, basically spamming the button--it seems
like....now all inputs are getting eatan"*, and then: *"this affects all sh2 functionality."*

Both halves are exactly right, and the log says so in one number.

## The measurement

One session, `SpellHotbar2.log`, game launched 00:23, log read at 00:47:

| line | count |
|---|---|
| `SH2 cast intent: slot N retained on local latch` | **101** |
| `SH2 cast intent: local latch releasing slot N` | **0** |
| `SH2 cast intent: slot N deferred to ShoutMCO` | 7 |
| `SH2 cast intent: fired slot N type T -> true` | 7 |

**The local latch released nothing, ever.** Every press that took the ShoutMCO deferral path fired,
7 for 7. Every press that took our own latch was eaten, 101 for 101. After 00:44:20 the log contains
nothing but retains — the driver is silent and the state never recovers. It was still stuck three
minutes later, and it survives combat ending.

## The mechanism, read out of our own source

`CastIntent::offer()` (`cast_intent.cpp:257`) branches on `should_retain_now()`:

```
should_retain_now() = should_retain_local_cast_intent(
    ArtDriver::is_active() || MscoCastDriver::is_active(),
    ArtDriver::is_active() ? ArtDriver::latch_open() : MscoCastDriver::combo_window_open(),
    CastingController::is_live_concentration());
```

which by `combo_cache.h:680` and its own tests is: **retain ⟺ a driver is active AND the combo
window is closed AND no live concentration.**

`MscoCastDriver::state_active` has exactly **one** clear path (`msco_cast_driver.cpp:~505`): the
graph *raising* `SH2_CastExit`. Nothing else clears it — not a timeout, not `attackStop`, not a
ready edge. `is_restore_edge` does consume `SBF_ReadyStart` / `MSCO_MagicReady` /
`MCO_AttackInitiate`, but only for the rolling combo restore; it never touches `state_active`.

And the driver's own bail-out is a `NotifyAnimationGraph` that the graph is free to refuse:

```
[00:42:44.531] SH2 cast: notified SH2_Cast3 (clip 3) -> false
[00:42:44.532] SH2 cast: notified SH2_CastExit    -> false
```

**`notified SH2_CastExit -> false` appears 7 times in this one session** (against 6 `-> true`). A
refused notify means the graph never raises `SH2_CastExit` back, so `state_active` stays true with
nothing left that can clear it. The last cast began at 00:42:46.004 (`notified SH2_Cast4 (clip 4)
-> true`); after it, the log shows `combo window closed (MCO_WinClose)` fifteen times over the next
minute and **not one** window open.

So the terminal state is `state_active = true`, `combo_window = false`, forever, which makes
`should_retain_now()` permanently true. From there:

- `offer()` takes the retain branch on every press, **returns `true`** (the press is reported
  handled), and overwrites the single-slot payload — so the press does not even fall through to the
  game.
- `poll_local_release()` runs every frame from `update_cast()` and bails at the same predicate on
  its first line.

The latch is a one-slot sink with the drain welded shut.

## Two fixes, independent, and the first one is the one that matters

**1. Bound the latch. Do this whatever else is done.** The local latch is the only waiter in this
stack with no cap. ShoutMCO bounds every one of its own — `pressCapMs = 4000`,
`shoutWaitCapMs = 4000`, `shoutCapMs = 8000`, plus `CastIntentApi::CheckWatchdog` — on the explicit
rule that a waiting state which cannot time out is a bug, not a feature. Give `payload_retained` a
deadline; on expiry, discard it and say so in the log. A wedged driver then costs the player one
press, not the rest of the session.

Consider also **not returning `true` from the retain branch**, or returning it only while the latch
is genuinely short-lived. Returning `true` is what converts "my cast did not play" into "my input
vanished": the press is consumed on our word that we will fire it later.

**2. Give `state_active` a clear path that does not depend on the graph agreeing.** The refused
notify is a measured, repeating event — 7 times in 25 minutes — so treating it as impossible is
what produced this. At minimum: clear `state_active` when
`NotifyAnimationGraph("SH2_CastExit")` returns false, since the graph has just told us it is not in
our state. A deadline on `state_active` itself is the belt-and-braces version.

## What this is NOT

**Not a regression from ShoutMCO ticket 66**, which landed the same evening. Checked rather than
assumed:

- ShoutMCO's deferral path fired 7/7 in this same session — the integration surface is healthy.
- The wedged state is entirely inside our `1hm_behavior` cast driver. Ticket 66 added a
  `BSIsActiveModifier` to `shout_behavior`'s `ShoutStanding` state only, active only during a
  player shout.
- Re-ticking `sbeef` that evening did start delivering `SBF_*` events we had not been receiving,
  which is a real change to our input — but `SBF_*` reaches only `is_restore_edge`, never
  `state_active` or `combo_window`.

## Acceptance

- [x] A press taken onto the local latch is either fired or discarded within the cap; it is never
      held indefinitely. — `kLocalLatchCapMs = 4000` (ShoutMCO's pressCapMs), checked every frame
      in `poll_local_release()` ahead of the retain bail; each fresh retain replaces the payload
      and restarts the clock, so no press outlives one cap. Unit-tested
      (`a_retained_press_is_dropped_once_its_cap_runs_out`).
- [x] With `state_active` forced true and the window closed, the Nth press still reaches the game
      (or is refused loudly) rather than vanishing. — verified live: during the 10:05:37 wedge the
      held press was released at watchdog expiry and *refused loudly* (`fired slot 3 type 3 ->
      false` + red slot highlight, `IsAttacking=1` at release), and the very next press deferred
      to ShoutMCO normally (handle 1).
- [x] A refused exit does not strand the state. — every `send_exit()` caller already ran
      `clear_state_flags()` unconditionally; what actually stranded the state was a cast whose
      graph never raised `SH2_CastExit` *at all* (entry `-> true` at 10:05:37, then straight into
      AttackState). The watchdog (`kCastStateCapMs = 8000`, channels exempt) is the clear path
      that needs no graph agreement: trace at 10:05:45.286 shows `state watchdog expired after
      8002ms` → `notified SH2_CastExit -> false` (refused, exactly as diagnosed) → held press
      released the same frame → later casts `-> true` with no relaunch. See
      `../evidence/36-watchdog-traces.log`.
- [x] The owner's reproduction leaves the bar working. — two spam rounds (25 + 20 reps of jammed
      `castSlot` presses interleaved with attack holds, greatsword, the second round against a
      live Bandit Warrior in combat at Riverwood edge — `../evidence/36-combat-repro.png`). Totals
      across the session: 133 retained, 82 released-and-fired, 2 wedges each recovered by the
      watchdog, and the bar cast normally afterward (`fired slot 3 -> true` at 10:09:21 and
      10:09:31). Compare the wedged session: 101 retained, 0 released, no recovery.
- [x] The log distinguishes "retained" from "retained and later dropped by the cap". —
      `SH2 cast intent: local latch dropped slot {} after {}ms cap` (warn) vs the debug "retained"
      line. Zero drops occurred live because spam kept refreshing the latch and the watchdog
      cleared both wedges first; the drop path is unit-tested.

## Comments

**Agent, 2026-08-25.** Implemented and verified. What landed (commit on `ticket-36-latch-bounds`):

- `combo_cache.h`: `kLocalLatchCapMs = 4000`, `kCastStateCapMs = 8000`,
  `local_latch_hold_expired()`, `cast_state_watchdog_expired()` — engine-free, unit-tested.
- `cast_intent.cpp`: `retained_at_ms` stamped on every local retain; expiry check first thing in
  `poll_local_release()`, dropping the payload with a warn line and a red slot highlight.
- `msco_cast_driver.{h,cpp}`: `state_entered_ms` stamped on an accepted entry notify;
  `poll_watchdog()` tears a wedged state down through the existing `cancel()`. Held channels are
  exempt.
- `casting_controller.cpp`: `poll_watchdog()` runs each frame immediately before
  `poll_local_release()`, so clearing a wedge releases a still-fresh press the same frame instead
  of dropping it.

One correction to the diagnosis: fix 2's "clear `state_active` when the exit notify returns
false" was already the code's behavior — every `send_exit()` caller runs `clear_state_flags()`
unconditionally. The terminal state came from a cast whose graph accepted the ENTRY and then never
raised `SH2_CastExit` back (it fell into MCO AttackState instead), a path with no notify to refuse
and, before this ticket, no clock. The watchdog is that clock.

The wedge reproduced twice under spam (once out of combat, once in combat) and recovered both
times inside 8 s with no player-visible residue beyond one refused press. Evidence in
`../evidence/`: `36-session.log` (full run), `36-watchdog-traces.log` (both recoveries),
`36-combat-repro.png` (live Bandit Warrior during round 2). The wedged baseline session is
`T36-local-latch-wedge-2026-08-25.log` in the same directory.

**Agent, 2026-08-25, post-review.** A second-model review (Cursor grok-4.6) of the diff confirmed
the fix and surfaced two residuals outside this ticket's scope — no ArtDriver watchdog (a wedged
Ability still eat-drop-loops, bounded but unrecovered) and both caps running on wall clock through
pauses. Filed as ticket 37 rather than widening this one. It also caught an overclaiming comment
at the `poll_watchdog` call site (the recovery frame's release is a loud refusal while the wedged
instance is still live, not a fire); the comment now says what the trace shows.
