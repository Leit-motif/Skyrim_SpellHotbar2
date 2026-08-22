# 11 — Ability Editor fire-time override (enhancement)

Let the Ability Editor choose when `PIE.$custom_ability_N` fires in the clip, instead of only the
v1 rule (HitFrame, else 5% of duration).

**Blocked by:** 12

**Status:** needs-triage

## You test this

(Unwritten until 12 ships.) Open Custom Ability 3 with a dropped clip. Move a fire-time control;
the assigned spell releases at that point in the motion, not at HitFrame/5%.

## Agent tests the rest

Sidecar stores an optional fire time. Stamp uses that value when set. Pointer-pack ashes untouched.
No Havok timeline visualizer required for a number or simple 0–duration slider; a clip-preview
scrubber is extra and not this ticket unless explicitly added.

## What this is

An enhancement on ticket 12’s inject. 12 is parked (`needs-triage`); 09 no longer injects.
When 12 ships, v1 fire time is still HitFrame, else 5% of duration (tiny windup, not 50%).

## What this is not

Not clip authoring. Not a second PIE marker. Not cost/cooldown (09). Not Ashes of War.

## Notes

Grill 2026-08-21: slider wanted; parked because a visual/override control is scope on top of
inject. A sidecar float plus a numeric field is small; a waveform scrubber is not. Re-grill
which of those this ticket is before implementation.

Owner 2026-08-21: want this after 09.
