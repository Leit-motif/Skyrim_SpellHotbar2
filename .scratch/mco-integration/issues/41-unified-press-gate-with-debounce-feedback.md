# 41 — One press gate: GCD lockout with visible refusal on every slot type

To the player, hotbar slots are just buttons, and today the three skill types answer a spammed
button three different ways: weapon arts flash red (correct), shouts swallow the press with no
feedback and no cast (the bug), and spells have no press gate at all. The owner wants one model:
**press → lockout → visible refusal**. The GCD feel, with debounce feedback. No Souls-style
buffer.

**Blocked by:** nothing. Read tickets 36/37 (the latch as an input sink, and its caps) before
touching the queue paths — this ticket must not reopen what they closed.

**Status:** ready-for-agent

## Owner report 2026-08-25

> If I spam the button with an Ash of War, I'm blocked out. I see red because the animation's
> playing. If I do it for a shout, I don't see the red blocking input on the hotbar icon, but
> nothing goes through. There's no input received. For spells, it seems there's no sort of mash
> guard. […] I want a unified model across all of the button presses because, essentially, to the
> player, these are just buttons. […] I just want the global cooldown feel where you press a
> button, there is a lockout, and you are given feedback. That is the best design.

## The rule this ticket exists to hold

Every press gets exactly one of three outcomes: it **executes**, it **queues** (the designed
combo/intent paths), or it is **refused with the red flash**. A silently swallowed press is a
bug wherever it happens. The combo window for weapon arts and the ShoutMCO deferred intent are
accepted presses, not refusals — they stay as they are.

## History: stock SH2 already had this design — ticket 10 carved the hole

Pre-fork `process_input` (see `git show 01d68f1:skse_plugin/src/input/modes.cpp`) gated **every**
slot type behind one universal check — `allowed_to_instantcast(...) &&
can_start_new_cast()` — and its else-branch painted the red flash. Press, lockout, feedback: the
exact model this ticket asks for, already shipped in stock.

Commit `f465947` (ticket 10, the Cast Intent queue) added the `queueable` exemption so a press
during our own Driver Cast could defer instead of dying. That was the right goal, but the
exemption removed the GCD refusal for spells and shouts wholesale, and it unmasked a latent
stock gap: the `try_cast_power` failure branch never had a highlight call, which didn't matter
while the outer gate caught spam first.

**So this is a restoration, not a design.** The stock gate is the reference implementation.
Re-narrow the exemption to the one case ticket 10 actually needed — a press that the intent or
combo machinery will genuinely accept — and let every other press fall through to the stock
refusal path.

## What the code already has, and where it leaks

The GCD machinery is fully built and even rendered — it just never refuses the types that matter:

- `BaseCastingInstance` carries `m_gcd` (1.5s default, 0.25s variants; set at
  `skse_plugin/src/casts/casting_controller.cpp:619/625/636/1380/1434`) with `is_gcd_expired()`
  and `get_current_gcd_progress()`, and the bars already draw the sweep (`gcd_prog`/`gcd_dur`
  through `skse_plugin/src/bar/hotbar.cpp`).
- **The exemption:** `InputModeCast::process_input`
  (`skse_plugin/src/input/modes.cpp:47-54`) refuses non-queueable types with red when
  `can_start_new_cast()` is false, but spell / shout / weapon_art are declared `queueable` and
  bypass that gate entirely. Weapon arts recover because their own start path paints red on
  failure (`modes.cpp:63`). The other two do not.
- **Shout silent drop, press side (the primary bug):** `modes.cpp:84-94` — when
  `try_cast_power` returns false, the failure falls through with **no highlight call at all**.
  Success injects the shout event; failure does nothing visible.
- **Shout silent drop, release side:** the ShoutMCO ABANDON branch
  (`skse_plugin/src/casts/cast_intent.cpp:217-219`) drops the payload with a debug log and no
  red. The neighboring refusals (`cast_intent.cpp:148/172/183`) all paint red — this one branch
  is the odd one out.
- **Spells:** `allowed_to_cast` (`skse_plugin/src/input/input.cpp:890`) checks `IsCasting`, but
  `start_cast` runs `cut_committed_cast_for_combo` (`casting_controller.cpp:889`), so a spam
  press during a live cast re-chains instead of being gated. Chaining is designed for combos;
  unbounded restart from raw spam is not.

## Shape of the fix

One gate at the dispatch layer in `process_input`, not three per-type guards:

1. While a live cast instance's GCD is unexpired, a press on any slot that the queue/combo
   machinery does not accept gets `highlight_skill_slot(slot, 0.5, true)` and returns. The
   existing `m_gcd` values are the lockout; no new timer.
2. Add the missing red flash to the `try_cast_power` failure path (`modes.cpp:88`) and to the
   ABANDON branch (`cast_intent.cpp:218`).
3. Decide per type whether a mid-GCD press may enter the combo/intent queue (arts: yes, that is
   ticket 10's feature; spells and shouts: only where the existing intent machinery already
   accepts it). Everything else is a visible refusal.

## Acceptance

All cells are live-runtime, on the deployed DLL, driven via `castSlot` where possible:

- [ ] Spam a weapon art slot mid-animation: red flash on every refused press (already true —
      regression cell).
- [ ] Spam a shout slot mid-shout and during voice recovery: every refused press flashes red;
      no press is silently swallowed. (DevBench: fire `castSlot`, re-fire immediately, capture
      the highlight state or log line per press.)
- [ ] Spam a spell slot mid-cast: presses during the GCD are refused with red, not chained into
      restarts; a legitimate combo chain still works.
- [ ] Powers and potions behave the same as the above (they already gate via
      `can_start_new_cast` — regression cell).
- [ ] No new input path bypasses ticket 36/37's caps on the local latch.
- [ ] One owner hands-on pass for feel: press, lockout, red — identical across all slot types.
