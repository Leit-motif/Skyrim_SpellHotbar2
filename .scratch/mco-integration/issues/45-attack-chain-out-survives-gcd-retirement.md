# 45 — Attack chain-out must survive GCD retirement

Ticket 43 regressed the cast-to-attack chain. The input hook's chain-out block
(`input/input.cpp` ~:466-505, live-verified 2026-08-12) ends the shtb state on an attack or
left-hand-cast press so the clip's tail becomes the start of a swing — but its gate is
`is_committed_cast_holding_graph()` (`casting_controller.cpp:170`), which requires a live
`current_cast`. Since ticket 43 retires the instance at GCD expiry, the gate is true only
from SpellFire to retirement (~0.59→1.0 s at the owner's setting) and false for the whole
follow-through, so an attack press waits out the clip. Owner report 2026-08-25: "in order to
chain into an attack, I have to wait what seems like the entire clip."

**Status:** ready-for-agent

## The change

1. New predicate beside `is_committed_cast_holding_graph()`: the graph is in cuttable
   follow-through — `current_cast == nullptr && MscoCastDriver::is_active()` (post-retirement,
   pre-exit; a charging cast still has `current_cast` set and stays protected exactly as
   today). Prefer a pure-rule seam in `combo_cache.h` with a test, matching the house style
   of tickets 42/43.
2. The input hook's two chain-out branches (attack press, left-hand cast press) fire on
   `is_committed_cast_holding_graph() || <new predicate>`. Behavior inside the branch is
   unchanged: `MscoCastDriver::cancel(pc)`, press untouched (the fail-safe).
3. An armed, undelivered payload must land on this cut too: call `deliver_armed_payload`
   ("the cut (attack)") in the branch before `cancel`, same as the five existing cut seams.
   (The armed poll's clip-end exit would catch it a frame later anyway; the explicit call
   keeps one delivery story.)
4. Touch nothing else — the concentration channel chain-out and Ability latch blocks below
   it have their own gates and are correct.

## Acceptance

- Host tests pass; new pure-rule case(s) for the follow-through predicate.
- Live: cast Firebolt, attack-press at ~1.2 s (after GCD, mid follow-through) → state ends
  (`attack pressed on a committed cast; ending the state` or successor log line), swing
  starts, stamina spent. This is an OWNER cell by construction — injected input never
  reaches the PollInputDevices hook (verified 2026-08-12), so the owner's own press is the
  only driver.
- Live: same on clip 4 with the payload still owed → armed payload delivers at the cut, then
  the swing.
- Regression: attack press during the charge (pre-SpellFire) still refuses to cut (protected
  cast), and the hotbar-press cut seams from ticket 43 still work.
