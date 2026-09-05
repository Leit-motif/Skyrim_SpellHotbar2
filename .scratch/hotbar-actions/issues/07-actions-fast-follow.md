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
   Resolved 2026-09-05: the `!Is3DLoaded()` guard now returns the capture decision, so a
   captured Action up or repeat is no longer forwarded raw.
2. **A key physically held across a load goes dead until re-pressed.** After `drop_live_cast`
   defers the release and `retry_action_releases` emits the up, the record is erased. The
   still-held source has nothing to mirror and its later up passes through unmatched. Decide:
   accept and state it in ticket 04, or re-arm on the next source repeat.
   Decided 2026-09-05: accepted, no code. Recorded in ticket 04 comments -- re-arming on a
   repeat would re-run admission and re-charge cost for a press made in the previous session.
3. **A pending release blocks re-press of the same slot or target.** `try_start_action`
   admission refuses against records still `release_pending`. Clears on the first loaded frame
   in practice; the queue-full case answers MagFail. After `kMaxReleaseAttempts` (300) the
   record is dropped with the target still down — the one path that leaves a phantom hold.
   Resolved 2026-09-05: a re-press against a `release_pending` record now retries that release
   first and admits when it succeeds. The 300-attempt cap no longer drops the record: it sets
   `retry_exhausted`, logs one warning, and stops only the per-frame retry. A source edge,
   re-press, or mode change refunds the budget and tries again, so no phantom hold is left.
4. **Empty `userEvent` is the normal case, not a fallback.** Owner 2026-09-05: "i assigned
   the Block action to keycode V, this is what my block mod is assigned to. that was the goal.
   i'm not trying to assign native skyrim functionality (such as native block which would be
   right mouse button)." Actions mirror mod hotkeys; native controls are out of scope. Live,
   every target (47, 48, 79, 81) resolved empty and every one worked. Fast-follow: stop
   treating the empty name as a degraded path in comments and logs, and decide whether
   `resolve_action_user_event` should stay at all or be removed as dead machinery.
   Resolved 2026-09-05: `resolve_action_user_event` stays -- an engine-mapped target key needs
   the name for PlayerControls. Comments, header docs, and log wording now treat the empty name
   as the ordinary mod-hotkey case rather than a fallback.
5. **Down and up are captured by different conditions.** Down capture sits behind
   `in_ingame_state()` and the bar-disable gate; up/repeat capture is unconditional in the
   pre-filter. Trace whether a forwarded down can still start an Action; if so the up is
   swallowed asymmetrically.
   Decided 2026-09-05: traced, no code. No forwarded down can start an Action; the reverse
   asymmetry (a refused down captured, its up forwarded) is real and harmless. In ticket 04.
6. **`castSlot` release marker is index-based.** `action_input_count()` then
   `release_action_for_slot(index, marker)`. Safe today (main-thread, synchronous) but fragile
   if an earlier record is erased between the two calls. A generation token is the robust form.
   Resolved 2026-09-05: records carry a monotonic `generation`. `action_input_count()` is now
   `next_action_generation()`, and `release_action_for_slot` matches on generation, not index.
7. **Writing `userEvent` into the embedded queue slot.** Reviewer HIGH, adjudicated LOW: this
   CommonLib's `AddEvent` fills `buttonEvents[buttonEventCount]` then advances, and the code
   checks the count moved before writing. Keep the guard; add a comment citing the header.
   Resolved 2026-09-05: guard kept, with a comment citing `RE/B/BSInputEventQueue.h` --
   `AddButtonEvent` fills `buttonEvents[buttonEventCount]` then increments, `MAX_BUTTON_EVENTS`
   is 10, which is why the count-moved check identifies the slot just written.
8. **`is_attack()` is `target == ocpa_power` only.** A captured B key is an identical target
   with `is_attack() == false`, so it does not cut a committed cast (ticket 04 cell 3).
   Resolved 2026-09-05: new pure `action_input_is_attack(action, resolved, live)` also reads a
   captured keyboard key equal to the live OCPA power or dual hotkey as an attack.
   `try_start_action` resolves both OCPA keys on every press and uses the helper for the
   admission gate, the post-acceptance cut, and the refusal log. Unit tested.
9. **Action overlays are a global JSON sidecar, not co-save state.** Names, icons, targets,
   and costs are shared across characters. State this in ticket 02 cell 6 or move them.
   Decided 2026-09-05: accepted as a global sidecar, no code. Recorded in ticket 02 cell 6 and
   in the CONTEXT.md Action entry.
10. **`apply_action_player_overlay` overwrites `icon` unconditionally but `display_name` only
    when non-empty.** Make both fall back or neither.
    Resolved 2026-09-05: the icon now falls back too -- an overlay with an empty `icon` AND
    `icon_form == 0` keeps the action's own icon, matching the name rule. Unit tested.
11. **While Action capture is armed, every button event is swallowed**, so the mouse cannot
    reach Cancel; only Escape works. Document or scope the capture to the next down only.
    Decided 2026-09-05: documented. `$ACTION_CAPTURE_ARMED` now tells the player the next key,
    mouse button, or pad button becomes the binding and that Escape cancels; a comment in
    `input.cpp` says why the mouse cannot reach Cancel (any click is a candidate binding).
12. **Dead code from the tap era**: `queue_action_tap`, `queue_keyboard_tap`,
    `get_dodge_hotkey`, `GameData::set_action`, `persist_user_action_overlays`,
    `BindCaptureState::any_armed`; `resolve_action_scancode` is test-only.
    Resolved 2026-09-05: removed `queue_action_tap`, `queue_keyboard_tap`, `get_dodge_hotkey`,
    `GameData::set_action`, `persist_user_action_overlays`, `BindCaptureState::any_armed`,
    `resolve_action_scancode`, and `keyboard_tap_phases` with its test.
    `kKeyboardTapReleaseHeldSecs` stays (castSlot and `release_held_duration`) with a comment
    explaining the bounded tap.
13. **One-entry `ActionKind` combo** in `action_editor.cpp`. Render a label.
    Resolved 2026-09-05: the one-entry kind combo is now a label; `kind_values` is gone and
    `draft_kind` still round-trips the value.
14. **CONTEXT.md still describes a single tap**; add the Mirror and Action Editor vocabulary.
    Resolved 2026-09-05: added **Mirror** and **Action Editor** terms, reworded the **Action**
    entry from "injects one authored input" to mirroring, and added the sidecar sentence.
15. **`read_action_uint` rejects non-`IsUint` silently** (`"captured_scancode": 48.0` loads
    as unbound with no error).
    Resolved 2026-09-05: the entry parser is now tri-state. A member present with the wrong
    JSON type logs a warning naming the member and skips the entry; an absent member stays
    optional.
16. **An out-of-catalogue Action id on a slot** renders `<INVALID>` with no load-time log line.
    Add the parallel `logger::info` the art path has.
    Resolved 2026-09-05: `hotbar.cpp` logs `Restored unknown Action {} on slot {}` after
    `update_action_assignment`, parallel to the art path.
17. **Tests**: `action_data_test.cpp` covers pure helpers only. Coverable seams:
    `apply_bind_drop` three-way exclusivity, `action_input_device_from_dx_scancode` boundaries
    (255/256/265/266), `is_action_record` for version 6 kind 2,
    `BindCaptureState::apply_action_down_edge` with `dx_scancode == 0`, overlay JSON rejection
    of out-of-range `kind`/`target`/`device`.
    Resolved 2026-09-05: added bind-drop form-onto-action and art-onto-action cases,
    `action_input_device_from_dx_scancode` boundaries, the finding-8 helper, the finding-10
    icon/name fallbacks, `is_action_record` for versions 6/7/8, a zero-DX
    `apply_action_down_edge`, and a new `action_overlay_json_test` over the extracted pure
    parser in `src/game_data/action_overlay_json.h`.
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

- [x] 1 and 3 fixed natively; 4 decided; build (full DLL, Release) and CTest (18/18) green
      2026-09-05. Redeploy and the owner's hold/release re-runs stay with the coordinator.
- [x] 2 and 5 decided and recorded in ticket 04.
- [x] 6–16 addressed with one line each above.
- [x] Save-then-load while holding an Action exercised once on a fresh save (the deferred
      `kPreLoadGame` release, expect `reason=retry`); if the save made mid-hold hangs post-thaw
      like Save6 did, that becomes finding 19. Exercised 14:45 on the Save5 session --
      see `runtime-acceptance-20260905.md`, "Save then load while holding (deferred release)".
- [x] 18: closed; Auto Input Switch 1.3.1 regression, owner reverted. No SH2 change.
- [x] 17: tests added for the seams that survive the above; CTest 18/18 passed.
