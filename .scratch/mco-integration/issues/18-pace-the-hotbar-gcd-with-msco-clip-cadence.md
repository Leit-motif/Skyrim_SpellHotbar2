# 18 — Pace the hotbar GCD with MSCO's clip cadence

**Type:** defect (driver timing)

**What to build:** The hotbar global cooldown follows the pace of the MSCO clips, rather than
feeling like its own timer. It may need to read MSCO's variable cooldown logic instead of keeping
an independent GCD.

**Blocked by:** None — can start immediately.

**Status:** ready-for-agent

## How this showed up

Owner playtest, 2026-08-12, during ticket 11. Not combo membership. Not a child of ticket 11.
Recorded so it is not forgotten; the exact MSCO variable and whether SH2 should adopt it or only
match its duration are for the implementing session to settle against ADR-0005 (do not grow a
second MCO timing policy for release).

- [ ] Consecutive hotbar casts follow MSCO clip cadence closely enough that the owner does not
      feel a separate GCD.
- [ ] The change does not become a second ShoutMCO release-timing cache (ADR-0005).
- [ ] Restore fixtures and close Skyrim after runtime work.
