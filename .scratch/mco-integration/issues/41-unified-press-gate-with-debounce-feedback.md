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
- **Queueing an art out of a weapon swing does not pass through the exemption.** An MCO swing on
  its own holds no live SH2 cast instance, so the stock gate passes the press; the mid-swing
  deferral is ShoutMCO's `graph_refused` wait inside the start path
  (`skse_plugin/src/casts/casting_controller.cpp:883`). The fork's headline feature survives
  untouched. **Scope correction (review, 2026-08-25):** this holds for a swing with no SH2
  instance live, which is the ordinary case — not universally. A swing can overlap one: a potion
  inside its GCD, or a cast or channel cut for the attack but not yet retired. An art pressed in
  that overlap is now refused rather than deferred. That is the universal lockout the owner
  chose, so it is accepted, not a defect — but the promise is narrower than first written.
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

- [x] Spam a spell slot mid-cast: every press during the clip flashes red; no chained restart;
      no press silently absorbed.
- [~] Sequential casts (press, wait for clip end, press) still walk the combo clips 1→2→3→4 —
      verify via the OAR Animation Log or the cast index graph variable, not by eye alone.
- [x] Spam a shout slot mid-shout and during voice recovery: every refused press flashes red;
      nothing is silently swallowed.
- [x] Spam a weapon art slot mid-animation: red flash on every refused press (already true —
      regression cell).
- [x] Queue an art out of a live weapon swing **with no other SH2 instance live**: still fires
      via the ShoutMCO deferral (regression cell for the feature the revert must not touch).
      The overlap case — a swing during a potion GCD or a cut cast — refuses by design; see the
      scope correction above.
- [ ] Powers and potions behave as before (regression cell).
- [x] No new input path reopens ticket 36/37's latch caps.
- [ ] One owner hands-on pass for feel: press, lockout, red — identical across all slot types.

## Acceptance run 2026-08-25 (agent, live)

Deployed DLL, Nolvus Awakening, owner's latest save (`Save27_…`, read-only), driven through the
`castSlot` Papyrus seam. Log: `Documents/My Games/Skyrim Special Edition/SKSE/SpellHotbar2.log`.
The gate's refusal is readable as `modes.cpp:110 SH2: slot N refused by the press gate` — a debug
line added by this ticket, since the stock else-branch paints the flash and logs nothing.

- **Spell spam — PASS.** Slot 0 (Firebolt), five presses. 20:35:12.123 started the cast; the
  four follow-ups at .394 / .616 / .839 / 13.068 each logged `refused by the press gate (type=3,
  cast live=true)`. The press at 13.068 lands *after* `MSCO_WinOpen` (12.954) — inside the window
  that used to produce the `chain` outcome — and is refused. No restart, nothing absorbed.
- **Weapon-art spam — PASS.** Slot 1 (art 15). 20:39:47.106 `SH2_ArtStart -> true`; the three
  follow-ups at .445 / .728 / 48.009 all refused by the gate.
- **Shout spam — PASS.** Slot 8. 20:40:55.820 fired (`queued input event=true`); presses at
  56.146 / .415 / .697 refused with the flash. Second burst at 20:40:59.719 fired, follow-up at
  20:41:00.244 refused. Before this ticket these presses were swallowed silently.
- **Art out of a live weapon swing — PASS, the cell that mattered.** Mouse swing (`Right
  Attack/Block`, 0.2s hold), art slot 1 pressed 200ms in with `IsAttacking=1`. The gate *passed*
  the press (no refusal line); `SH2_ArtStart not consumed (mid-swing)` → `slot 1 deferred to
  ShoutMCO (handle 1)` at 20:40:07.569 → `RELEASE for handle 1 (ready)` at .809 → `fired slot 1
  type 8 -> true`. The fork's headline feature is untouched by the revert.
- **Combo walk — PARTIAL.** Sequential casts advanced 1→2 and 2→3, so the index still advances
  per cast. The 3→4 step could not be observed: clip 3 (`Animations\MSCO_left3.hkx`, an owner
  custom animation) raised `SH2_CastExit` before SpellFire, which by design resets the index to 1
  and delivered the spell late (`casting_controller.cpp:524`). Owner confirmed the animation is
  at fault, not the code. Re-run this cell once those clips carry their SpellFire annotation.
- **Powers and potions — NOT RUN.** No slot of type `power`, `lesser_power`, or `potion` is bound
  on any bar reachable in this save, so there was nothing to press. Static position: the diff
  cannot change them. They were never in the `queueable` set, so they were already gated on
  `can_start_new_cast()` before this change and are gated on the same predicate after it; powers
  additionally share the exact branch the shout cell exercised. Still, that is an argument, not a
  frame — leave the cell open.
- **Tickets 36/37 — PASS (static).** No input path was added. `is_live_concentration()` remains
  live at `cast_intent.cpp:267`, and the cap and watchdog paths are untouched by the diff.
- **The red flash itself — NOT PROVEN, and cannot be headlessly.** DevBench has no registered
  capture provider on this instance, and the native fallback (`MenuControls::QueueScreenshot`)
  does not composite SH2's ImGui overlay: the captured frame
  (`t41-spell-refusal-flash.png`, 3440×1440) contains no hotbar at all. Every cell above proves
  the refusal *call* — `highlight_skill_slot(slot, 0.5, true)` — reached, by its adjacent log
  line. That the pixels are red is the owner's hands-on cell.

## Review 2026-08-25 — Codex (gpt-5.6-sol, high)

Verdict **LAND WITH FIXES**; four findings, all verified against the code and all addressed.
None touched the behavior of the revert — three were comments of mine that were simply wrong,
one was this ticket overclaiming.

1. The gate comment (and this ticket) claimed an MCO swing never holds an SH2 cast instance.
   Not universal — see the scope correction above. Comment and ticket both narrowed.
2. `try_start_cast`'s new comment called the latch-closed arm unreachable from the hotbar.
   `InputModeVampireLord::process_input` sends potions there with no gate of its own (stock did
   not gate that mode either, so it stays out of scope). The comment also wrongly said an art
   release calls in there; `fire_payload` dispatches arts to `try_start_art`. Both corrected.
3. `try_cast_power`'s new comment claimed the arm is live for the release path. It is not:
   `fire_payload` runs under `attempting_release`, so `is_firing()` is true and the arm's own
   guard skips it. Its real remaining caller is Vampire Lord mode. Corrected.
4. `can_accept_hotbar_cast()` has zero callers and still advertises the removed chain admission.
   Pruning was out of scope, so it is marked instead, with a do-not-rewire note.
