# 20 — Chain a hotbar cast in during MCO recovery

**Type:** feature (input + ShoutMCO release), parent ticket 11 close-out feel

**What to build:** A hotbar press during an MCO attack's recovery window starts the next Driver
Cast the way a follow-up attack would — without waiting for the swing clip to finish. The delay
the owner already feels is the existing ShoutMCO buffer releasing too late, plus SH2 capturing
presses that did not start a cast.

**Blocked by:** None. Ticket 04 already ships defer→release; this is the recovery-tail cell that
ticket 04 left failing, now owner-specified as cadence not just "eventually casts."

**Status:** superseded 2026-08-13 — do not implement this file. The ask was an input
buffer / recovery window on SH2 casts, not inbound during MCO recovery. Ticket 14 already
lets a mash bypass GCD; ticket 18 is the remaining GCD tune.

## What this is not

Not clip-4 windup or the sheathe lockout (ticket 17). Not pacing SH2's own GCD between consecutive
hotbar clips (ticket 18). Not dual-fire (ticket 15, resolved).

## Behaviour

MCO exposes `MCO_InputBuffer` then `MCO_AllowRecovery` then `MCO_WinOpen` on the attack clip.
A hotbar press in that window must not die, and must not wait for `attackStop`.

- [ ] A hotbar press during `MCO_AllowRecovery` / `MCO_WinOpen` starts a Driver Cast (or
      ShoutMCO-releases into one) without waiting for the swing clip to end.
- [ ] A captured hotbar press that did not start a Driver Cast is not swallowed — vanilla or a
      later retry can still see it.
- [ ] Clips 1–3 and an idle Direct Cast are unchanged.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-13 — owner mixed-chain playtest.** `attack1 → attack2 → cast1 → attack3 → cast2`
works if you wait for the clip to finish. "the window is very strict… want a recovery window
to allow combo'ing so it feels the same as mco-attack cadence. should have a spell queuing,
input buffer type functionality. i also noticed a delay here, which indicates perhaps there
is an input buffer, there's just no recovery window."

Same session, Save65, `SpellHotbar2.log`. ShoutMCO *does* buffer: `slot 0 deferred to
ShoutMCO` handles 1–6, then `released cast on slot 0 -> true` often while `IsAttacking=1`
(ticket 04's hit-frame release). A press *during recovery* is a different miss:

```
17:27:32.527  attack cut from a committed cast; SH2_CastExit -> true
17:27:32.670  MCO_InputBuffer
17:27:32.838  MCO_AllowRecovery
17:27:33.517  captured hotbar press to prevent dual fire
17:27:33.518  SH2_CastExit -> false          <-- teardown, no begin()
17:27:33.520  MCO_WinOpen
17:27:33.946  attackStop
17:27:34.564  SH2_Cast3 -> true              <-- next press, after the clip ended
```

Capture from ticket 15 swallows key 1 whenever the bind is `handled`, including GCD / teardown
/ refuse. Combined with ticket 04 releasing at `preHitFrame` rather than `inRdy`, the owner
has to wait for the swing to end and press again.

**2026-08-13 — owner: this ticket misread the recovery ask.** The quote about a recovery
window and spell queuing was about SH2 casts matching MCO-attack cadence (queue the next
hotbar cast during the current clip), not about inserting a hotbar cast into an MCO swing's
recovery. That buffer only mattered while GCD felt hardwired at 1s. Ticket 14 already lets a
mash chain on SpellFire and skip `m_gcd`; ticket 18 is the remaining "keep a GCD, just
shorter" tune. This file is not work.
