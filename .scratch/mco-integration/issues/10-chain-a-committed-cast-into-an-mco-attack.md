# 10 — Chain a committed hotbar cast into an MCO attack

**Type:** feature (driver + input), the owner's 2026-08-12 ask: *"chain a spell → attack/pattack
cleanly."*

**What to build:** An attack press during a **committed** hotbar cast leaves the cast state early
and starts the swing, instead of being swallowed for the rest of the clip.

**Blocked by:** Nothing. Ticket 08 shipped the state this cuts; ticket 07 shipped the commitment
flag that makes cutting safe.

**Status:** claimed

## What the trial found (live, 2026-08-12, T-save, Iron Rapier drawn)

The handoff's §2 trial ran three ways, with **stamina as the discriminator**: an MCO swing spends
5 of the player's 115, and the reading is exact, immediate, and needs no line of sight. A magicka
reading does not discriminate (finding 12) and a frame only shows a pose.

| Press at | Stamina after | Meaning |
|---|---|---|
| no cast (control) | 115 → **110** | the injected press drives a real swing |
| 0.9 s into the cast (post-spellfire, inside `MSCO_WinOpen`) | 115 → 115 → 115 | **refused** |
| 1.4 s | 115 → 115 | refused |
| 1.8 s | 115 → **110** | allowed |
| 2.1 s | 115 → **110** | allowed |

**Outcome 3 of the handoff's three, as predicted: the state holds the graph.** The refusal ends
between 1.4 s and 1.8 s, which is the clip's own end (`SH2_CastExit` trigger at 1.617 s − 0.05 s),
not a cooldown: the cast instance's GCD expires at 1.5 s and the boundary sits after it.

The spell itself lands throughout — a 5000 HP Bandit Warrior fixture took the hit on a control
cast (5000 → 4992.5), so the tail being cut is dead time, not delivery time.

Frames corroborate the numbers rather than carrying them: `../shots/attack-control-swing.png` is
the control press mid-swing, `../shots/cast-then-attack-refused.png` is the same moment of the
same swing after a cast, standing idle with the blade down.

## The design

**Cut from the driver side; do not widen ShoutMCO's arm.** The three reasons are the handoff's §3
and stand unchanged: SH2 owns the state, ShoutMCO cannot see this cast (`isShoutStateEntry` is
exactly `tag == "SBF_ShoutStart"`), and `BeginShoutLocked`'s driver decline is load-bearing.

On an attack press while a committed cast holds the graph: send `SH2_CastExit`, capture the press,
and re-queue it so the game receives it one frame later.

- **The gate is commitment, not liveness.** `is_committed_cast_holding_graph()` is true only for a
  live cast instance whose graph state is active and whose `MLh_SpellFire_Event` has arrived.
  Before that instant a cut costs the player the spell — ticket 03's original hazard, real for the
  clip's first 0.483 s — so a press then keeps today's behaviour exactly.
- **The press is forwarded untouched.** It is not captured, and nothing is re-queued. The first
  revision did capture and replay it, meaning to buy the graph a frame; a Codex review showed that
  is not what happens, and the CommonLib source agrees. This hook runs inside `PollInputDevices`,
  and `PushOntoInputQueue` sets `queueTail->next` on the very chain being dispatched
  (`BSInputEventQueue.cpp`), so the copy arrives in the same frame regardless — paying for an
  allocation nobody frees, a second pass through this file's own modifier and keybind handling,
  and a second ImGui mouse event. Both events reach the graph's queue in the order they were sent,
  which is the only ordering available and the one the cut needs.

  Leaving the press alone is also what makes the chain fail-safe: if the graph refuses the cut, the
  player gets exactly today's behaviour instead of a swallowed attack.
- **Right attack only, keyboard and mouse only.** The left control is block, or a left-hand cast;
  neither is the "spell into a swing" being asked for. The device restriction is not fastidiousness:
  this file works in its own 0–15 gamepad ordinals while `GetMappedKey` answers in the engine's
  ids, so a gamepad comparison is between two alphabets and can only be wrong, and `kNone` is `-1`
  in an array index behind an assert a release build drops.
- **Concentration channels are excluded** (`is_concentration_channel()`). Cutting a channel's state
  does not end the channel — its own loop re-enters the state within half a second — so the cut
  would be undone while the swing was still starting. Ending a channel properly is its own ticket,
  as ticket 08 already scoped it.
- **Power attack is a separate cell.** This profile routes power attacks through One Click Power
  Attack on key 48, which reads raw input.

## Changed

- `skse_plugin/src/casts/casting_controller.{h,cpp}` — `is_committed_cast_holding_graph()`, and
  `is_concentration_channel()` on the instance hierarchy.
- `skse_plugin/src/input/input.cpp` — `is_attack_press` / `chain_out_of_committed_cast`, and the
  branch in `processAndFilter`.
- `skse_plugin/src/casts/msco_cast_driver.cpp` — `send_exit` logs its notify return.
- `skse_plugin/src/events/animationeventhook.cpp` — the graph-event trace (see ticket 08's
  correction), bounded to a live cast state because it runs on the animation thread and the logger
  flushes every line.
- `docs/adr/0006-own-nemesis-state-with-a-timer-floor.md` — amended with the measured annotation
  and the second graph, closing the footprint note ticket 08 left against it.

## Acceptance — the owner's hands are required, and here is why

**An agent cannot drive this branch.** DevBench's `input` tool injects into
`BSInputDeviceManager`'s event source, which reaches PlayerControls (an injected attack really does
swing, proven above) but **not** this mod's `DispatchInputEvent` trampoline. Verified 2026-08-12:
injecting hotbar slot 0's own bound key (DX scan code 2, read back from `SpellHotbar.GetKeyBind(0)`)
produced no cast and no log line at all, while `castSlot(0)` on the Papyrus seam worked in the same
session. Every cell below is on the input path, so every cell below is the owner's.

The branch traces itself so a failed press says why in one session: while a committed cast is live,
each press logs its device, key, and the resolved attack binding.

- [ ] Weapon drawn and idle, press "1", then press attack about 0.9 s in: the swing starts and the
      spell still lands (a damaged target or a visible effect — not a magicka reading).
- [ ] The same press **before** 0.483 s behaves exactly as it does today, and costs the player
      nothing.
- [ ] A press with no cast in flight is untouched — no capture, no replay, no dropped attack.
- [ ] A real MCO swing, a real shout, and an ordinary uninterrupted cast are all unchanged.
- [ ] Power attack (OCPA, key 48) — its own cell, and its own run. OCPA reads raw input on an
      unmapped keyboard attack control, so this branch does not fire for it; whether OCPA then
      synthesises a mapped attack event is unknown and is what the cell answers.
- [ ] Restore fixtures and close Skyrim after runtime work.

## The fixture, left standing 2026-08-12

Skyrim is running on the latest save, weapon drawn, facing a **20000 HP essential Bandit Warrior**
(`FF0026E0`, ~180 units ahead) that is in the line of fire: a control cast took it 20000 → 19992.5,
so every landed hit reads as another 7.5 and the pool outlasts the whole matrix. Cast is hotbar
slot 0 on key "1" (`SpellHotbar.GetKeyBind(0)` → 2).

To run a cell: press "1", then press attack about **0.9 s** later — after the spell leaves the hand,
inside `MSCO_WinOpen`. Then read `SpellHotbar2.log`:

- `attack pressed on a committed cast; cut the state` — the branch fired.
- `notified SH2_CastExit -> true` at that instant — the graph took the cut. `-> false` means it
  refused, and the next move is the Nemesis transition, not the C++.
- `press during a committed cast (device=…, key=…, attack key=…)` — a press the branch saw but did
  not match; the two key numbers disagreeing is the whole diagnosis.

**Fixtures to restore afterwards:** the spawned bandit (`FF0026E0 disable` / `markfordelete`) is the
only world change. Nothing else was mutated — no `tcai`, no INI edits, no save renames.

## Open findings carried, not fixed

- **The commitment flag has no cast generation.** `spellfire_seen` and `state_active` are
  process-wide; a stray accepted left SpellFire could in principle authorise a cut for a cast whose
  own annotation has not arrived. `arm_spellfire` clears the flag at every cast start, which bounds
  it, and the design is ticket 07's rather than this ticket's — but it is a real hole and belongs in
  whichever ticket next touches the commitment point.
- **CONTEXT.md still describes the retired shout-graph chain model.** Ticket 03 is corrected;
  CONTEXT.md is not, and the two now disagree.

## Known risk, named rather than assumed

The cut's mechanism is proven at 1.5 s and **unproven at 0.9 s**. `MscoCastDriver::finish`'s
`NotifyAnimationGraph("SH2_CastExit")` returned **true** at 1.504 s into a cast and the state left
immediately (`attackStop` / `SBF_ReadyStart` / `MSCO_MagicReady` on the next two log lines). A
Papyrus `Debug.SendAnimationEvent` of the same event at 0.2 s and 0.9 s did **not** stop the clip or
free the attack — but Papyrus is not this call path, so that is not evidence about it. If the
owner's press logs the cut and the swing still does not start, the next move is the transition's
`initiateInterval` in `nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$1.txt`, not the C++.
