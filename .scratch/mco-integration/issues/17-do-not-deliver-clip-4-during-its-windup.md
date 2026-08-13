# 17 — Do not deliver clip 4 during its windup

**Type:** defect (driver timing)

**What to build:** A hotbar cast on clip 4 must not release the spell during the clip's windup.
Clips 1–3 releasing near the start of the animation is fine. Clip 4 has a windup, and the spell
currently fires in the middle of it, which looks wrong.

**Blocked by:** None — can start immediately.

**Status:** ready-for-agent

## How this showed up

Owner playtest, 2026-08-12, during ticket 11. Not combo membership. Not a child of ticket 11.
The session log already showed clip 4 hitting the 0.5 s authored-time floor before its SpellFire
annotation at ~0.92 s — same symptom, now owner-confirmed by eye.

ADR-0006 says the annotation leads and the authored cast time is the floor. Clip 4 is the case
where the floor is earlier than the animation's release pose.

- [ ] Clip 4 delivers at the release pose, not during the windup, owner-verified by eye.
- [ ] Clips 1–3 still deliver at the start of the animation, unchanged.
- [ ] Restore fixtures and close Skyrim after runtime work.
