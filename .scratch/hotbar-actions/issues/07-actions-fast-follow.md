# 07 — Actions fast-follow: review findings after the mirroring build

Status: open. Cut 2026-09-05 from the review of `429b937..255769c`. Owner: "capture these
observations and fixes as a fast-follow up. i want to address all of them in a separate
session." Nothing here blocks the current build; the owner tested `d7b0b3a` live the same
day and reported "everything seems to be functioning correctly".

Line: `ng/smf-next`. Overlay: `Dev - Spell Hotbar 2 SMF Next`. Native C++ only.

## Findings

Ordered by the coordinator's adjudicated severity. Reviewer severities that were changed are
noted.

1. **Source up is forwarded raw when the player is not 3D-loaded.** `input.cpp` ~221. The
   pre-filter releases the target and sets `action_event_handled`, then `return {}` at the
   `!pc->Is3DLoaded()` guard discards the capture. The engine or the source key's mod sees an
   up with no down. Fix: return the capture decision when an Action handled the event.
2. **A key physically held across a load goes dead until re-pressed.** After `drop_live_cast`
   defers the release and `retry_action_releases` emits the up, the record is erased. The
   still-held source has nothing to mirror and its later up passes through unmatched. Decide:
   accept and state it in ticket 04, or re-arm on the next source repeat.
3. **A pending release blocks re-press of the same slot or target.** `try_start_action`
   admission refuses against records still `release_pending`. Clears on the first loaded frame
   in practice; the queue-full case answers MagFail. After `kMaxReleaseAttempts` (300) the
   record is dropped with the target still down — the one path that leaves a phantom hold.
4. **Empty `userEvent` is the normal case, not a fallback.** Owner 2026-09-05: "i assigned
   the Block action to keycode V, this is what my block mod is assigned to. that was the goal.
   i'm not trying to assign native skyrim functionality (such as native block which would be
   right mouse button)." Actions mirror mod hotkeys; native controls are out of scope. Live,
   every target (47, 48, 79, 81) resolved empty and every one worked. Fast-follow: stop
   treating the empty name as a degraded path in comments and logs, and decide whether
   `resolve_action_user_event` should stay at all or be removed as dead machinery.
5. **Down and up are captured by different conditions.** Down capture sits behind
   `in_ingame_state()` and the bar-disable gate; up/repeat capture is unconditional in the
   pre-filter. Trace whether a forwarded down can still start an Action; if so the up is
   swallowed asymmetrically.
6. **`castSlot` release marker is index-based.** `action_input_count()` then
   `release_action_for_slot(index, marker)`. Safe today (main-thread, synchronous) but fragile
   if an earlier record is erased between the two calls. A generation token is the robust form.
7. **Writing `userEvent` into the embedded queue slot.** Reviewer HIGH, adjudicated LOW: this
   CommonLib's `AddEvent` fills `buttonEvents[buttonEventCount]` then advances, and the code
   checks the count moved before writing. Keep the guard; add a comment citing the header.
8. **`is_attack()` is `target == ocpa_power` only.** A captured B key is an identical target
   with `is_attack() == false`, so it does not cut a committed cast (ticket 04 cell 3).
9. **Action overlays are a global JSON sidecar, not co-save state.** Names, icons, targets,
   and costs are shared across characters. State this in ticket 02 cell 6 or move them.
10. **`apply_action_player_overlay` overwrites `icon` unconditionally but `display_name` only
    when non-empty.** Make both fall back or neither.
11. **While Action capture is armed, every button event is swallowed**, so the mouse cannot
    reach Cancel; only Escape works. Document or scope the capture to the next down only.
12. **Dead code from the tap era**: `queue_action_tap`, `queue_keyboard_tap`,
    `get_dodge_hotkey`, `GameData::set_action`, `persist_user_action_overlays`,
    `BindCaptureState::any_armed`; `resolve_action_scancode` is test-only.
13. **One-entry `ActionKind` combo** in `action_editor.cpp`. Render a label.
14. **CONTEXT.md still describes a single tap**; add the Mirror and Action Editor vocabulary.
15. **`read_action_uint` rejects non-`IsUint` silently** (`"captured_scancode": 48.0` loads
    as unbound with no error).
16. **An out-of-catalogue Action id on a slot** renders `<INVALID>` with no load-time log line.
    Add the parallel `logger::info` the art path has.
17. **Tests**: `action_data_test.cpp` covers pure helpers only. Coverable seams:
    `apply_bind_drop` three-way exclusivity, `action_input_device_from_dx_scancode` boundaries
    (255/256/265/266), `is_action_record` for version 6 kind 2,
    `BindCaptureState::apply_action_down_edge` with `dx_scancode == 0`, overlay JSON rejection
    of out-of-range `kind`/`target`/`device`.
18. **CLOSED 2026-09-05 — Auto Input Switch 1.3.1 was the cause.** Owner reverted to the
    previous version: "that was the problem all along. This mod has been functioning
    perfectly fine. I'm able to swap between mouse and keyboard easily." Kept for the record:
    the switch does not return to keyboard/mouse while the pad stays on — NOT this build's
    capture path. Live 2026-09-05 12:01–14:02, reproduced three times. Mechanism
    (source `Exit-9B/AutoInputSwitch`): the switch flips back when its `BSInputDeviceManager`
    event sink sees any non-repeating keyboard or mouse event; `Game.UsingGamepad()` reports
    the physical pad, not the switch's state, so Papyrus cannot observe it. Discriminator run at
    14:01:20 with the pad on and KBM dead: DevBench injected a keyboard Shift tap and a mouse
    right-button tap through `BSInputDeviceManager::SendEvent`, which reaches every sink and
    bypasses the SMF per-event hook and all of SH2. KBM stayed dead (owner: "nope"). Physical
    keys meanwhile reached SH2's hook and Timed Block throughout, and the mirror held nothing.
    Conclusion: the switch's sink is not acting on keyboard events while the pad is connected,
    independent of anything SH2 unlinks. Installed version is 1.3.1 (2026-08-26, "Updated for
    SkyrimSE 1.7.99 and Address Library 12"); if the owner's memory of clean switching predates
    that update, the update is the suspect. Owner preference: not an incompatibility to list.
    Recovery observed each time: power the pad off. Remaining action for a later session: retest
    with SH2 disabled once, or ask on the switch's page; no code change here.

## Acceptance

- [ ] 1 and 3 fixed natively; 4 decided; build; CTest; redeploy; owner re-runs the hold/release cells.
- [ ] 2 and 5 decided and recorded in ticket 04.
- [ ] 6–16 addressed or explicitly declined with one line each.
- [ ] Save-then-load while holding an Action exercised once on a fresh save (the deferred
      `kPreLoadGame` release, expect `reason=retry`); if the save made mid-hold hangs post-thaw
      like Save6 did, that becomes finding 19.
- [x] 18: closed; Auto Input Switch 1.3.1 regression, owner reverted. No SH2 change.
- [ ] 17: tests added for the seams that survive the above.
