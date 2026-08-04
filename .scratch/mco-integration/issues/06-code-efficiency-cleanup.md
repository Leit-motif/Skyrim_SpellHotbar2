# 06 — Clean up inefficient code

**Type:** task (Core Fork)

**What to build:** Targeted efficiency fixes in this fork's own code, argued from measurement
rather than from reading.

**Blocked by:** None, but explicitly **secondary**. It is not a prerequisite for any
integration ticket and must not be bundled into one.

**Status:** ready-for-agent

The owner named this as a secondary goal alongside MCO integration. Keeping it in its own
ticket keeps integration diffs reviewable — a chain-out fix and a refactor in the same commit
are indistinguishable to a reviewer, and one of them is load-bearing.

- [ ] Identify the actual hot paths rather than the ugly ones. The plugin hooks the main game
      loop and renders ImGui every frame; a per-frame cost is worth more attention than a
      per-cast one, however untidy the latter reads.
- [ ] Measure before changing. Cost is latency, not line count, and the heavy patterns hide
      in code that looks simple.
- [ ] Cover the Papyrus side (`papyrus/Scripts/Source/`) as well as the C++ — different rules
      apply, and Papyrus cost shows up as script lag and stack dumps rather than frame time.
- [ ] Keep each change independently revertible. An efficiency change that cannot be backed
      out separately from an integration change is a liability.
- [ ] Do not refactor for taste. Anything that does not show a measured improvement, or fix a
      genuine defect found on the way, belongs in a note rather than a diff.
