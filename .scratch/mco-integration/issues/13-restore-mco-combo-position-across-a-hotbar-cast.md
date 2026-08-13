# 13 — Restore MCO combo position across a hotbar cast

**Type:** feature (driver), parent ticket 11

**What to build:** A hotbar cast that interrupts a combo continues that combo. `attack1 → cast →
attack2`, not `attack1 → cast → attack1`. The same for power attacks. The owner has already seen
the power cell work (`attack1 → pattack2 → cast1 → pattack3`); this ticket makes the light cell
match and makes the restore safe to leave armed.

**Blocked by:** 12 — Amend ADR-0005: combo position is not release timing

**Status:** claimed

## What this is not

Not consecutive-cast clips (ticket 14). Not MSCO hand-cast chaining (ticket 15). Not a second
ShoutMCO. The ADR from ticket 12 is the interface: sample and restore combo position for a
Driver Cast this mod started; do not grow a release-timing cache.

The mixed chain `attack1 → attack2 → cast1 → attack3 → cast2` is parent 11's close-out once 13
and 14 are both green. This ticket owns the attack-3 half.

## Behaviour

The cut from ticket 10 still routes through ready, and ready resets MCO's index to 1. This ticket
puts the previous index back after that reset, once.

- Snapshot is a rolling sample taken while attacking, not a read at cast start (by then the
  previous swing has already reset the variable).
- One write after the ready-state reset payload, then the pending restore is consumed. A later
  ready or PIE event must not replay an old index.
- Preserve the value that was sampled. Do not add one, wrap, or clamp.
- If the sample is too old to trust, write nothing and leave MCO at 1.
- A real MCO swing, a real shout, and an ordinary uninterrupted cast are unchanged.

- [ ] `attack1 → cast → attack` continues at the next combo position, owner-verified by eye.
- [ ] Power attacks continue the same way, not only light attacks.
- [x] A restore is consumed after its ready pass; a later ready/PIE cannot rewrite the index.
- [x] Capture and restore do not race across the animation callback and the main loop.
- [ ] A real swing, a real shout, and an uninterrupted cast are unchanged.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-12 — unblocked.** Ticket 12 resolved. The contract is ADR-0005's 2026-08-12 scope
amendment: sample and restore for a Driver Cast this mod started; preserve never derive; do not
grow a release-timing cache. The blob's `RollingMcoCombo` / `observe_graph_event` path is the
start, not acceptance — Codex flagged races and a restore that stays armed.

**2026-08-12 — built on `ticket-11-combo-chain`.** Codex's two defects are closed at the
`RollingMcoCombo` seam: `arm` / `peek` / `consume` is one-shot, and every mutation takes a mutex
so capture (animation thread) and arm (main-thread cancel/finish) cannot tear. The driver writes
after `#0006`'s PIE reset (observer runs after the original handler), then consumes on the first
`SBF_ReadyStart` / `MSCO_MagicReady` so a later ready/PIE cannot replay the index. `SH2_CastExit`
arms only while a Driver Cast this mod started is live. Preserve, never derive; stale or empty
samples write nothing. `combo_cache_test` green; plugin links clean.

In-game cells stay open: the attack-cut path still cannot be driven from this hook (ticket 10).
Owner presses: `attack1 → cast → attack2`, and the same for power.
