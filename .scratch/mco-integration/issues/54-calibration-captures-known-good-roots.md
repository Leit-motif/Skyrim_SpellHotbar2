# 54 — Calibration captures: telemetry signatures of the two known-good roots

**Type:** instrumentation (owner hands + telemetry, one live session, no authoring)

**Status:** CLOSED 2026-08-29, partial — the protocol was abandoned mid-run by owner ruling. One
capture landed and it overturned the project's cheapest oracle; the rest was cut as not worth
owner time. **Blocked by:** None. Part of the ticket 53 umbrella.

See the Outcome section at the bottom before planning any capture like this again.

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


## Outcome (2026-08-29)

**Delivered:** `tools/telemetry/root_signature.py`, and one capture —
`evidence/t54/01-msco-ff-moving-entry.json`, seven owner-performed MSCO fire-and-forget casts
from a moving entry — plus `evidence/t54/00-harness-check-standstill-channel.json`, a headless
`castSlot(0)` kept as the instrument's reading on a known root.

**What it bought, which is the whole justification for the ticket:** `bAnimationDriven` does not
rise on the owner-certified-correct MSCO root. Seven casts, no rise. The playbook called that
variable the direct read of the root plant and the cheapest proof available for commitment
mechanics; it is true for SH2's own channel and false for the target behavior. Enemy Magelock
never touches the flag either (see `notes/57-mechanism-comparison.md`). A prototype validated
against that flag would have been validated against nothing.

**Why the rest was cut, by owner ruling:** the reference mods answer the mechanism question
directly, and the calibration was buying an acceptance oracle at a price the instrument could
not justify. Two hard limits, both measured here:

1. **The two channels cannot run together.** `record`'s pose sampler and the graph-variable poll
   both need the game's main thread. Alone, pose holds 64 ms. Under the variable poll it starved
   to 8.2 s of a 60 s window and never left the start position. Split into two passes, the
   high-rate pose pass has no variable channel to anchor on. **The ticket's headline metric —
   time from cast-begin to translation stopping — has no pass that can measure it on this
   machine.** The summarizer now flags a truncated trajectory rather than reporting it as a
   player who never moved.
2. **A clock-triggered window is the wrong shape for owner-driven captures.** The owner cannot
   see the terminal, and chat latency sits between the instruction and the act. In-game
   `Debug.Notification` cues helped and did not fix it. If a capture like this is ever needed
   again: **run long and free, and slice the series on the cast edges afterward** — casts
   self-mark in the variable channel, which removes timing from the protocol entirely.

**Owner observations from the runs, recorded because they are facts about the build:**

- SH2 refuses to cast while sprinting. Moving forward at a run, a hotbar Flames channel roots.
- The vanilla equipped-hand walk-cast does not root and does not change — the uncommitted
  baseline behaves as expected.

**Not done:** the hotbar-channel and vanilla-control captures, and the derived signature summary
the ticket asked for. Tickets 55–58 do not inherit an oracle from this ticket; they inherit the
mechanism map in `notes/57-mechanism-comparison.md` and the negative result above.

**Fixture note:** the session granted Fireball, equipped it right-hand, and loaded a probe bar
(slot 0 = Flames). All in memory, nothing saved, no mod or file changed. A restart clears it.
