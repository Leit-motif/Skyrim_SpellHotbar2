# 48 — A dual cast slower than 1.51s presents as a single-hand cast

Bug against ADR-0018's contract, found by the ticket 44 inventory (§2.2).

**Status:** ready-for-agent

**Blocked by:** nothing (independent of tickets 46/47, but its acceptance is easiest to see
once 46's dual set is live).

## The defect

A dual-cast one-handed spell goes through `start_ritual_cast`, which picks the animation slot
by cast time: `variant` (the dual id 10016/10017) only when `casttime <= 1.51f`
(`casting_controller.cpp:1140-1146`, `spell_cast_data.cpp:96-108`, threshold
`casting_controller.h:12`). A dual cast SLOWER than 1.51s therefore writes the plain
single-hand family id with `SpellHotbar_CastingSource == 0` — OAR presents it as a plain
left cast while delivery dual-casts.

Under the matrix's D1 (dual is a first-class hand value), a dual cast presents dual at any
cast time. The 1.51s fast/slow split exists for genuinely two-handed ritual spells (slow
ritual = the ritual id, fast ritual = the dual id) — untangle the dual-1H case from that
logic without disturbing the ritual behavior.

## Acceptance

- A dual-assigned 1H spell with cast time > 1.51s writes the dual animation-type id and
  selects the dual set (Animation Log), commits at its authored event, and delivers once
  with dual mechanics.
- Fast dual (≤ 1.51s), slow ritual, and fast ritual keep today's ids — cite the log for each.
- The existing unit suites stay green; add a case to the anim-id selection tests if the
  decision logic moves into a testable seam.
