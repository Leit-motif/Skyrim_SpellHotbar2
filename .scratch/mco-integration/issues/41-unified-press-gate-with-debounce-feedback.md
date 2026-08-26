# 41 — Revert to the stock press gate; every refused press flashes red

To the player, hotbar slots are just buttons, and today the three skill types answer a spammed
button three different ways: weapon arts flash red (correct), shouts swallow the press with no
feedback and no cast, and spells absorb spam into an invisible buffer. The owner wants the model
stock SH2 already shipped: **press → lockout → visible refusal**. This ticket restores it.

**Blocked by:** nothing. Read tickets 36/37 (the latch as an input sink, and its caps) before
touching the queue paths — this ticket must not reopen what they closed.

**Status:** ready-for-agent

## Owner decision 2026-08-25

The exemption is dropped, not narrowed. Quotes, in order:

> I just want the global cooldown feel where you press a button, there is a lockout, and you are
> given feedback. That is the best design.

> Wouldn't it just be simpler and cleaner to follow and take what SH2 stock already shipped?

Confirmed before deciding: **cast-to-cast combos survive the revert** (see below). The owner
accepted the one real cost — losing the early-cancel cadence — and chose the revert.

## History

Pre-fork `process_input` (see `git show 01d68f1:skse_plugin/src/input/modes.cpp`) gated every
slot type behind one universal check — `allowed_to_instantcast(...) && can_start_new_cast()` —
and its else-branch painted the red flash. Commit `f465947` (ticket 10, the Cast Intent queue)
added a `queueable` exemption so a press during our own Driver Cast could defer instead of
dying. The exemption was cut far wider than that case: it took spells and shouts out of the
refusal path wholesale, and it unmasked a latent stock gap — the `try_cast_power` failure
branch never had a highlight call, invisible while the outer gate caught spam first.

## Why the revert is safe (verified against the code, not assumed)

- **Cast combos are cadence-independent.** The visual combo — successive casts walking clips
  1→2→3→4 — is `CastComboIndex` (`skse_plugin/src/casts/combo_cache.h:156`), which advances at
  each committed clip's SpellFire (`skse_plugin/src/casts/msco_cast_driver.cpp:492`), wraps at
  4, and is reset only by a session load or a dropped press. It advances per **cast**, not per
  chain-press. Sequential press → clip ends → press again still plays the next combo clip.
- **Queueing an art out of a weapon swing does not pass through the exemption.** During an MCO
  swing there is no live SH2 cast instance, so the stock gate passes the press;  the mid-swing
  deferral is ShoutMCO's `graph_refused` wait inside the start path
  (`skse_plugin/src/casts/casting_controller.cpp:883`). The fork's headline feature survives
  untouched.
- **No leftover GCD tail.** Ticket 18 already made FNF driver casts die at clip end. The
  lockout the player feels is the animation itself — which is also what the red flash reports.

## What is deliberately lost

Two behaviors, both accepted by the owner as the point of the change:

1. The `chain` outcome — a press inside the SpellFire→WinClose window cutting the clip tail to
   start the next combo cast early. Now a red refusal; the next cast starts on a fresh press
   after the clip ends.
2. The last-wins intent buffer for hotbar spell/shout presses during our own clip. A press
   slightly before the latch opens now flashes red instead of firing later.

## The change

1. **Remove the `queueable` exemption** in `InputModeCast::process_input`
   (`skse_plugin/src/input/modes.cpp:47-59`). Every slot type goes through the stock gate:
   `allowed_to_instantcast(...) && can_start_new_cast()`, with the red flash
   (`highlight_skill_slot(slot, 0.5, true)`) on refusal. The separate `is_live_concentration`
   refusal is subsumed — a live channel is a live `current_cast`.
2. **Add the missing red flash to the `try_cast_power` failure path** (`modes.cpp:88` — the
   branch where it returns false currently falls through with no highlight call).
3. **Add the red flash to the CastIntent ABANDON branch**
   (`skse_plugin/src/casts/cast_intent.cpp:217-219`), which today drops the payload with only a
   debug log. The intent machinery itself stays — it still carries the mid-swing art deferral.

Internal controller branches that the gate now shadows for hotbar spell/shout presses
(`classify_hotbar_cast_press`'s chain arm, `CastIntent::offer` from `try_start_cast`) are left
in place — they still serve the art paths, and pruning them is out of scope. Note in a comment
where the gate makes an arm unreachable from the hotbar, so a later reader doesn't hunt for a
caller that no longer exists.

## Acceptance

All cells are live-runtime, on the deployed DLL, driven via `castSlot` where possible:

- [ ] Spam a spell slot mid-cast: every press during the clip flashes red; no chained restart;
      no press silently absorbed.
- [ ] Sequential casts (press, wait for clip end, press) still walk the combo clips 1→2→3→4 —
      verify via the OAR Animation Log or the cast index graph variable, not by eye alone.
- [ ] Spam a shout slot mid-shout and during voice recovery: every refused press flashes red;
      nothing is silently swallowed.
- [ ] Spam a weapon art slot mid-animation: red flash on every refused press (already true —
      regression cell).
- [ ] Queue an art out of a live weapon swing: still fires via the ShoutMCO deferral
      (regression cell for the feature the revert must not touch).
- [ ] Powers and potions behave as before (regression cell).
- [ ] No new input path reopens ticket 36/37's latch caps.
- [ ] One owner hands-on pass for feel: press, lockout, red — identical across all slot types.
