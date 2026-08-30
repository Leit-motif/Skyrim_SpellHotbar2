# 37 — Ticket 36's caps run on wall clock and are the only timers here that are

**Type:** defect (DLL), low severity, small scope. Residual of ticket 36.

**Status:** in progress — built and deployed 2026-08-29, awaiting the owner's live pause
cells. Code landed, `combo_cache_test`
and the other six suites pass unchanged, one acceptance box is still open — see `## Build` at the
bottom.

**Status at the time (historical):** ready-for-agent — triaged 2026-08-25 against the source; one
of the two filed claims was rejected outright, the other confirmed and narrowed. Read the triage
below before working it.

**Blocked by:** None.

## Triage, 2026-08-25

This ticket was filed straight off a second-model review of ticket 36's diff. Verifying both
claims against the source rejected the first and sharpened the second.

### Rejected: "the Ability driver has no watchdog"

It has one. It just does not live in `art_driver.cpp`, which is the only place the review looked.

`CastingInstanceWeaponArt::update()` (`casting_controller.cpp:1505`) caps a live art at eight
seconds — the same number ticket 36 chose for the cast state — and tears it down through the
driver's own path:

```cpp
if (ArtDriver::is_active()) {
    if (m_cast_timer > 8.0f) {
        ArtDriver::cancel(pc);
        return true;
    }
    return false;
}
```

with a sheathe escape immediately above it. `ArtDriver::cancel()` runs `on_exit`, which clears
`state_active` and `latch_is_open`, so `should_retain_now()` goes false and the latch drains.

The coupling holds because `ArtDriver::begin()` is only ever called with a `CastingInstanceWeaponArt`
freshly installed as `current_cast` (`casting_controller.cpp:1586`), and a refused begin resets both.
There is no path that leaves `ArtDriver::state_active` true with nothing polling it. The art driver
also has a second escape the cast driver lacks: with the latch open, `MCO_AttackInitiate` or
`MCO_AttackEnterNotify` exits the state (`art_driver.cpp:71`).

So the eat-loop the review predicted for Abilities cannot happen. **No code owed.** The one thing
worth keeping is a note: the art driver's bound lives in its casting instance rather than in the
driver, nothing says so at either site, and a future change that lets the driver outlive its
instance would silently remove the bound. A comment at `ArtDriver::begin()` would earn its place.

### Confirmed, and the real argument is diagnostic, not gameplay

`update_cast` is gated on `!ui->GameIsPaused()` (`gameloop_hook.cpp:18`); a paused game freezes
animation but not `steady_clock`. Both stamps ticket 36 added are `steady_clock`, so menu time
counts against them.

What makes this worth fixing is not mainly the gameplay edge. It is that **every other timer in
this subsystem accumulates `delta`** — `advance_time(float delta)`, the art's own `m_cast_timer`,
the GCDs — and `delta` only accrues on unpaused frames, so all of them are pause-correct by
construction. Ticket 36 introduced the only two wall-clock deadlines in the file. That is an
inconsistency with the house pattern, and it produces a log line that lies:

    SH2 cast: state watchdog expired after 12000ms; clearing wedged cast state

after a twelve-second inventory visit, with nothing wedged. Ticket 36's entire diagnosis was built
by counting these lines across a session. A cap that manufactures false wedge reports poisons the
next diagnosis, which is a worse cost than the animation it cuts.

The gameplay half is real but narrow: a healthy Driver Cast paused mid-clip for 8s+ is cancelled on
unpause instead of finishing. The clip is 1–2s, so the player has to open a menu inside that window
and stay a while.

### By design, not a defect: the latch cap under pause

A press retained, then a 4s+ menu visit, is dropped on unpause. That is the right outcome — firing a
several-seconds-stale hotbar press after the player has been in a menu is worse than dropping it,
and the drop is announced (warn line, red slot flash). Leave it, and let the fix below change it
only incidentally.

## The fix

Advance both deadlines on the same clock the polls run on. `update_cast` already receives `delta`
and only runs unpaused; accumulate it into one monotonic "unpaused milliseconds" counter, stamp
`retained_at_ms` and `state_entered_ms` from that counter instead of `now_ms()`, and compare against
it in the two predicates. The predicates in `combo_cache.h` take their times as parameters and do not
change at all.

This makes both caps pause-correct by construction rather than by a correction term, matches what
every neighbouring timer already does, and deletes the two `now_ms()` helpers ticket 36 added.

Do not reach for crediting paused time after the fact — the channel hold's `credit_held_time` exists
because a *sample age* had to survive a legitimately long hold, which is a different problem.

## Acceptance

- [ ] A cast paused mid-clip past the cap completes normally on unpause: no watchdog line, no cut.
- [ ] A press retained just before a pause behaves the same way it would have unpaused (the 4s cap
      measured in unpaused time only).
- [ ] No wall-clock deadline remains in `cast_intent.cpp` or `msco_cast_driver.cpp`; both read the
      unpaused counter.
- [ ] `combo_cache_test` still passes unchanged — the predicates are time-source agnostic and this
      fix must not need them edited.
- [ ] A comment at `ArtDriver::begin()` records that the art state's bound lives in
      `CastingInstanceWeaponArt::update()`, so a future change cannot remove it unknowingly.

## Provenance

Filed from a Cursor grok-4.6 review of ticket 36's diff, then triaged against the source. The
review's other three observations were judged not worth code at filing time and that still stands:
the `SH2_CastExit` branch not zeroing `state_entered_ms` (harmless while `send_entry()` is the only
raiser, and zeroing it there would add an animation-thread write to a main-thread field); the missing
zero-sentinel on `local_latch_hold_expired` (the call site always stamps first); and the recovery
frame's release being refused rather than fired while the wedged instance is still live (documented
at the call site).

## Build, 2026-08-29

`skse_plugin/src/casts/unpaused_clock.h` is the counter: `advance(delta)` is called once at the
top of `update_cast`, which the game-loop hook calls only while `!GameIsPaused()`, so the counter
is gameplay milliseconds. It is `std::atomic<double>` because `observe_graph_event` reads it from
the animation thread; relaxed ordering is enough, since nothing is published through it and a
reader one frame behind measures an age off by one frame. It starts at 1.0 rather than 0.0 so the
first frame of a session cannot mint a stamp that reads as the "nothing recorded" sentinel in
`cast_state_watchdog_expired`.

**One clock per file, not two.** The fix as filed only names the two ticket-36 deadlines, but
`msco_cast_driver.cpp`'s `now_ms()` predates them (it came in with ticket 11) and also stamps the
rolling combo samples and the channel hold. Splitting the file between two clocks would have left
`credit_held_time` discounting a wall-clock hold from a wall-clock age while the watchdog beside it
ran on gameplay time. So `now_ms()` in that file now *is* the unpaused clock, and every timestamp
in it moves together. That widens the fix slightly beyond the letter of the ticket, and in the
direction the ticket argues for: a 6s inventory visit no longer ages a combo sample past
`kMaxAgeMs` and resets the chain to attack1 either.

Predicates in `combo_cache.h` were not touched — they take their times as parameters, which is
what let the time source change under them.

### Acceptance

- [x] No wall-clock deadline remains in `cast_intent.cpp` or `msco_cast_driver.cpp`; both read the
      unpaused counter. `steady_clock` now appears in exactly one place in the DLL, `input.cpp`,
      which is real-time input debounce and not a gameplay deadline.
- [x] `combo_cache_test` passes unchanged — not one line of it was edited. All seven suites green
      (`combo_cache`, `clip_translation`, `art_data`, `art_pack_gen`, `cast_anim_ids`,
      `art_bind_record`, `bind_drop`).
- [x] A comment at `ArtDriver::begin()` records that the art state's bound lives in
      `CastingInstanceWeaponArt::update()` (casting_controller.cpp:1797), names the coupling that
      makes it hold, and says what a future change would have to add before breaking it.
- [ ] **Owner cell.** A cast paused mid-clip past the cap completes normally on unpause: no
      watchdog line in SpellHotbar2.log, no cut. Fire a Driver Cast, open the inventory inside the
      1–2s clip, stay past 8s, close it.
- [ ] **Owner cell.** A press retained just before a pause behaves as it would have unpaused (the
      4s cap measured in unpaused time only).

Both open cells need a pause held across a live cast, which is one action for a player at the
keyboard and a timing race for an agent driving injected input. The static half of the argument is
strong: the caps now read the same counter `advance_time`, `m_cast_timer` and the GCDs have always
used, all of which are already known pause-correct in play.
