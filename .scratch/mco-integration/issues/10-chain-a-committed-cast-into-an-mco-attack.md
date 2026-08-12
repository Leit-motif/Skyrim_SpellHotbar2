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
- **The press is replayed, not forwarded.** Sending the cut and forwarding the same press in the
  same frame is the case most likely to misbehave, because the graph may not have left the state
  when MCO reads the input. `BSInputEventQueue::PushOntoInputQueue` is the replay primitive the
  voice cast driver already uses for the shout key, and it costs one frame.
- **Right attack only.** The left control is block, or a left-hand cast; neither is the "spell into
  a swing" being asked for.
- **Power attack is a separate cell.** This profile routes power attacks through One Click Power
  Attack on key 48, which reads raw input.

## Changed

- `skse_plugin/src/casts/casting_controller.{h,cpp}` — `is_committed_cast_holding_graph()`.
- `skse_plugin/src/input/input.cpp` — `is_attack_press` / `chain_out_of_committed_cast`, and the
  branch in `processAndFilter`.
- `skse_plugin/src/casts/msco_cast_driver.cpp` — `send_exit` logs its notify return.
- `skse_plugin/src/events/animationeventhook.cpp` — the filtered graph-event trace (see ticket 08's
  correction).

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
- [ ] Power attack (OCPA, key 48) — its own cell, and its own run.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Known risk, named rather than assumed

The cut's mechanism is proven at 1.5 s and **unproven at 0.9 s**. `MscoCastDriver::finish`'s
`NotifyAnimationGraph("SH2_CastExit")` returned **true** at 1.504 s into a cast and the state left
immediately (`attackStop` / `SBF_ReadyStart` / `MSCO_MagicReady` on the next two log lines). A
Papyrus `Debug.SendAnimationEvent` of the same event at 0.2 s and 0.9 s did **not** stop the clip or
free the attack — but Papyrus is not this call path, so that is not evidence about it. If the
owner's press logs the cut and the swing still does not start, the next move is the transition's
`initiateInterval` in `nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$1.txt`, not the C++.
