# 48 — A dual cast slower than 1.51s presents as a single-hand cast

Bug against ADR-0018's contract, found by the ticket 44 inventory (§2.2).

**Status:** CLOSED 2026-08-26, owner-accepted with ticket 46's dual cells. TWO fixes were
needed, not one: `35cfa98` (the time-based slot pick — this ticket's filed defect) and
`bf366ae` (the deeper one found live: upstream's per-spell `Animation2` column is its
variant/first-person anim id, NOT the dual id — Firebolt carries 10001 — so both dual paths
now ask the family for 10016/10017/11003/11004 and never read the column for dual). Owner
confirmed dual aimed and dual self present dual art in play; the log shows 10016/10017
written. Honest residue: the literal >1.51s slow-dual cell was never exercised live (the test
spells are fast); it is covered by the unit case `slow dual 1H is still the dual id` and by
the fix making cast time structurally irrelevant to the dual id. `ritual_cast_slot()` in `cast_anim_ids.h` now
gives a dual 1H cast the dual id at any cast time; unit cases cover all five cells. Two debug
lines landed with it: the dual→auto perk downgrade names itself, and `set_animtype_global`
logs the id it writes.

**Caveat from the same day's diagnosis:** the CS-Test fixture (level 3) lacks dual-casting
perks, so `player_can_dualcast_spell` fails and every scripted "dual" cast silently downgraded
to auto→left — the 46 session's "dual never selects" telemetry was this, not the leak. Dual
cells can only be accepted by a character with the school's dual-cast perk (the owner's), or
after granting the perk to the fixture in-memory.

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
