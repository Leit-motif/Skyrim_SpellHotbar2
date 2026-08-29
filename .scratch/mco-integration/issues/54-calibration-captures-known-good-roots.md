# 54 — Calibration captures: telemetry signatures of the two known-good roots

**Type:** instrumentation (owner hands + telemetry, one live session, no authoring)

**Status:** ready-for-agent. **Blocked by:** None. Part of the ticket 53 umbrella.

## Why

Ticket 33 burned three owner feel-test cycles on mechanisms that telemetry had called green.
The root cause: no oracle was ever calibrated against behavior the owner certifies as CORRECT.
This ticket records that oracle. Every later prototype (tickets 55–58) must match these
signatures headlessly BEFORE it may cost an owner feel test.

## Protocol

Game up (CS-Test or owner save, read-only, never save). Owner performs, announcing each:

1. **Moving-entry MSCO fire-and-forget**: sprint, then cast an equipped-hand FF spell. The
   owner-certified-perfect hard root.
2. **Moving-entry hotbar channel**: sprint, then press a hotbar concentration slot
   (`SH2_Channel_State`, full-body, owner-certified).
3. (Control) **Moving-entry vanilla walk-cast**: sprint, then hold equipped Flames with no
   commitment mod active — the uncommitted baseline.

Per capture, record at ≤150 ms cadence for ~5 s spanning the entry: player X/Y, heading,
`bAnimationDriven`, `IsCastingRight/Left`, and `cliplog` filtered to the player (state the
active filter — the ring floods in ~seconds unfiltered; SCAR spam especially). Drive sampling
via `scenario` repeats; the owner supplies only the inputs (playbook: player movement is
owner-hands by construction).

## Deliverable

`.scratch/mco-integration/evidence/t54/` — one file per capture (raw sample series + a short
derived summary: frames from cast-begin to translation < 1 unit/sample; momentum tail length;
which variables flip and in what order). Cited by path from the ticket. Update the playbook's
oracle ladder with a pointer.

## Acceptance

- [ ] Three capture series on disk, cited by path, each labeled with the owner's verbal
      confirmation of what they did.
- [ ] A derived "correct root signature" summary a later ticket can diff a prototype against.
- [ ] Nothing saved in-game; no mods changed.
