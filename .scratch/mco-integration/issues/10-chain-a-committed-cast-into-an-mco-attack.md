# 10 — Chain a committed hotbar cast into an MCO attack

**Type:** feature (driver + input), the owner's 2026-08-12 ask: *"chain a spell → attack/pattack
cleanly."*

**What to build:** An attack press during a **committed** hotbar cast leaves the cast state early
and starts the swing, instead of being swallowed for the rest of the clip.

**Blocked by:** Nothing. Ticket 08 shipped the state this cuts; ticket 07 shipped the commitment
flag that makes cutting safe.

**Status:** resolved (power-attack cell open by decision, not by defect)

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

On an attack press while a committed cast holds the graph: send `SH2_CastExit`, and let the press
travel on untouched.

- **The gate is commitment, not liveness.** `is_committed_cast_holding_graph()` requires a live cast
  instance that owns a cuttable state, an active graph state, and an arrived `MLh_SpellFire_Event`.
  Before that instant a cut costs the player the spell — ticket 03's original hazard, real for the
  clip's first 0.483 s — so a press then keeps today's behaviour exactly.

  `has_cuttable_cast_state()` defaults to **false** on `BaseCastingInstance` so that only a kind of
  cast that opts in is ever cut. A power, a shout, and a potion never enter the state, and ending it
  on their behalf would be a send for a state they do not own; a concentration channel is excluded
  for the separate reason below. Only a plain spell cast opts in.
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
- **Concentration channels are excluded.** Cutting a channel's state does not end the channel — its
  own loop re-enters the state within half a second — so the cut would be undone while the swing
  was still starting. Ending a channel properly is its own ticket, as ticket 08 already scoped it.
- **Power attack is a separate cell**, and it does not chain — see acceptance.

## Changed

- `skse_plugin/src/casts/casting_controller.{h,cpp}` — `is_committed_cast_holding_graph()`, and
  `has_cuttable_cast_state()` on the instance hierarchy.
- `skse_plugin/src/input/input.cpp` — `get_attack_key()` and the branch in `processAndFilter`.
- `skse_plugin/src/casts/msco_cast_driver.cpp` — `send_exit` logs its notify return.
- `skse_plugin/src/casts/msco_cast_driver.{h,cpp}` — `should_trace_graph_events()`, the bounded
  post-cut trace window.
- `skse_plugin/src/events/animationeventhook.cpp` — the graph-event trace (see ticket 08's
  correction), bounded because it runs on the animation thread and the logger flushes every line.
- `docs/adr/0006-own-nemesis-state-with-a-timer-floor.md` — amended with the measured annotation
  and the second graph, closing the footprint note ticket 08 left against it.

## Acceptance — the owner's hands are required, and here is why

**An agent cannot drive this branch.** DevBench's `input` tool injects into
`BSInputDeviceManager`'s event source, which reaches PlayerControls (an injected attack really does
swing, proven above) but **not** this mod's `DispatchInputEvent` trampoline. Verified 2026-08-12:
injecting hotbar slot 0's own bound key (DX scan code 2, read back from `SpellHotbar.GetKeyBind(0)`)
produced no cast and no log line at all, while `castSlot(0)` on the Papyrus seam worked in the same
session. Every cell below is on the input path, so every cell below is the owner's.

Built and linked clean, 13/13, no new warnings; deployed byte-identical to `Dev - Spell Hotbar 2`.

The branch traces itself so a failed press says why in one session: while a committed cast is live,
**every** press logs its device, key, and the resolved attack binding — matching or not — and the
graph trace continues for a bounded burst of events after a cut, because the events that say whether
the hand-off worked all arrive once the state is already gone.

- [x] Weapon drawn and idle, press "1", then press attack about 0.9 s in: the swing starts.
      **Owner-run live 2026-08-12, 16:35–16:39. 32 cuts fired; all 32 were followed by
      `MCO_AttackInitiate` within 0.5 s — zero failures.** One run in full:

      ```
      41.516  MLh_SpellFire_Event                     the commitment point
      41.619  press during a committed cast (device=1, key=0, attack key=0)
      41.619  attack pressed on a committed cast; ending the state
      41.620  notified SH2_CastExit -> true           the graph took the cut
      41.626  attackStop, SBF_ReadyStart, MSCO_MagicReady        the state leaving
      41.627  SBF_NormalAttackStart, MCO_AttackInitiate, MCO_AttackEnterNotify
      41.764  MCO_InputBuffer
      41.913  MCO_AllowRecovery
      42.940  MCO_WinOpen, MCO_PowerWinOpen           a full ordinary MCO attack
      ```

      **The swing starts 7 ms after the cut, in the same frame batch** — which settles the design
      question the first revision got wrong. Same-frame ordering is not merely adequate, it is
      what happens: the cut's transition and MCO's attack event reach the graph's queue in the
      order they were sent, and the graph honours both in one update. The replay would have bought
      nothing.

      The spell landing is structural rather than separately observed here: `m_spell_started` is
      set the moment the commitment flag goes true, 103 ms before the press, so the magic is out
      before the cut exists. Worth one deliberate look at a target to close it by eye.
- [ ] The same press **before** 0.483 s behaves exactly as it does today, and costs the player
      nothing.
- [x] A press with no cast in flight is untouched, and no unrelated press is ever matched.
      **Owner-run 2026-08-12:** the branch saw and correctly declined every non-attack key pressed
      during a cast — "1" itself (×5), W (×1), A (×3), D (×1) — all `attack key=255`, because the
      right attack is bound to the mouse and nothing on the keyboard carries that control. Zero
      false positives across the session.
- [ ] A real MCO swing, a real shout, and an ordinary uninterrupted cast are all unchanged.
- [ ] **Power attack does NOT chain, and the reason is now measured, not guessed.** The owner
      pressed OCPA's key 48 three times during committed casts. Each time the branch *saw* the
      press and correctly declined it: `press during a committed cast (device=0, key=48, attack
      key=255)`. The right attack is bound to the mouse, so `GetMappedKey("Right Attack/Block",
      kKeyboard)` is `kInvalid` and no keyboard key can ever match — OCPA's power key least of all,
      since it is not an attack control at all but a mod's own hotkey.

      This is a scope decision, not a bug. **Owner's call, 2026-08-12: resolved on the profile
      side, not in code.** SH2 will not read `Data/MCM/Settings/OCPA.ini`; the owner moves the
      power attack onto a real attack control instead, and no cross-mod dependency is created.

      **Why that needs no code change.** The branch fires on the DOWN edge of whatever key carries
      `Right Attack/Block`. A vanilla hold-to-power-attack therefore already chains: the press
      cuts the cast state at ~0.9 s, the hold continues into `1HM_Ready_State`, and the power
      attack develops from it exactly as it would from an idle stance. The chain-out is indifferent
      to what the press later becomes — it only has to see the press.

      **What that constrains.** The power attack must originate from the *mapped attack control*,
      not from a separate hotkey. Binding a hotkey to the same physical key as the right attack is
      not the same thing and would misfire on every ordinary swing. In practice this means letting
      the vanilla hold produce the power attack rather than OCPA's instant key — worth confirming
      OCPA is not suppressing the hold path once the rebind is made.

      **Still open as a cell:** nobody has yet seen a power attack chain out of a cast. The
      mechanism above is reasoning from a verified branch, not an observation.
- [ ] Restore fixtures and close Skyrim after runtime work.

## The fixture, left standing 2026-08-12

Skyrim is running on the latest save, weapon drawn, facing a **20000 HP essential Bandit Warrior**
(`FF0026E4`, ~140 units ahead) that is in the line of fire: a control cast took it 20000 → 19992.5,
so every landed hit reads as another 7.5 and the pool outlasts the whole matrix. Cast is hotbar
slot 0 on key "1" (`SpellHotbar.GetKeyBind(0)` → 2).

To run a cell: press "1", then press attack about **0.9 s** later — after the spell leaves the hand,
inside `MSCO_WinOpen`. Then read `SpellHotbar2.log`:

- `press during a committed cast (device=…, key=…, attack key=…)` — every press the branch saw
  during a cast. Two key numbers that agree mean it matched; disagreeing is the whole diagnosis of a
  press that did nothing.
- `attack pressed on a committed cast; ending the state` — the branch fired.
- `notified SH2_CastExit -> true` at that instant — the graph took the cut. `-> false` means it
  refused, and the next move is the Nemesis transition, not the C++.
- The `SH2 graph event:` lines that follow the cut — `attackStart` or an `MCO_*` among them is the
  hand-off actually happening; their absence with a `-> true` cut is the interesting failure.

Expect a **second** `notified SH2_CastExit` roughly 0.6 s after the cut, when the cast instance's
GCD expires and `on_reset` calls `finish()`. It is harmless — the event's only listener is a state
that is already gone, so it reaches nothing and logs `-> false` — but it lands mid-swing and would
look alarming unexplained.

**Fixtures to restore afterwards:** the spawned bandit (`FF0026E4 disable` / `markfordelete`) is the
only world change. Nothing else was mutated — no `tcai`, no INI edits, no save renames.

**Regression cells that did close, on the shipped build:** three real MCO swings with no cast in
flight added **zero** log lines, so the animation-thread trace stays off during ordinary combat; and
an ordinary uninterrupted cast is unchanged end to end — entry `-> true`, `SH2_CastEnter` +0.18 s,
`MLh_SpellFire_Event` +0.46 s, `MSCO_WinOpen` +0.67 s, `SH2_CastExit -> true` +1.50 s.

## Open findings carried, not fixed

- **The commitment flag has no cast generation.** `spellfire_seen` and `state_active` are
  process-wide; a stray accepted left SpellFire could in principle authorise a cut for a cast whose
  own annotation has not arrived. `arm_spellfire` clears the flag at every cast start, which bounds
  it, and the design is ticket 07's rather than this ticket's — but it is a real hole and belongs in
  whichever ticket next touches the commitment point.
- **CONTEXT.md still describes the retired shout-graph chain model.** Ticket 03 is corrected;
  CONTEXT.md is not, and the two now disagree.

## Known risk — closed 2026-08-12

~~The cut's mechanism is proven at 1.5 s and **unproven at 0.9 s**.~~ **Closed by the owner's run:
32 of 32 cuts at ~0.6–0.9 s returned `-> true` and the state left on the next frame.** The Papyrus
`Debug.SendAnimationEvent` probe that appeared to show the opposite was measuring a different call
path, exactly as suspected — the Nemesis transition in `#shtb$1.txt` was never at fault.

The original text, kept because the reasoning is worth reusing: `MscoCastDriver::finish`'s
`NotifyAnimationGraph("SH2_CastExit")` returned **true** at 1.504 s into a cast and the state left
immediately (`attackStop` / `SBF_ReadyStart` / `MSCO_MagicReady` on the next two log lines). A
Papyrus `Debug.SendAnimationEvent` of the same event at 0.2 s and 0.9 s did **not** stop the clip or
free the attack — but Papyrus is not this call path, so that is not evidence about it. If the
owner's press logs the cut and the swing still does not start, the next move is the transition's
`initiateInterval` in `nemesis/Nemesis_Engine/mod/shtb/1hm_behavior/#shtb$1.txt`, not the C++.
