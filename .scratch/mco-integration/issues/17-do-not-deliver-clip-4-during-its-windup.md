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
- [ ] Clip 4 does not lock the player out of MSCO left-hand or hotbar casts (sheathe/unsheathe
      must not be required to continue).
- [ ] Clips 1–3 still deliver at the start of the animation, unchanged.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Comments

**2026-08-13 — owner: clip 4 still fires in the windup, and then locks casting.** Same
session as ticket 15's closing playtest (`SpellHotbar2.log` from 17:07). Every clip 4:

```
notified SH2_Cast4 (clip 4) -> true
no SpellFire event by the authored cast time; delivering on the timer floor   (~0.50s)
isolated left-hand caster before vanilla SpellFire                            (~0.92s, the real throw)
```

After that, `SH2_CastExit -> false` (no listener). Hotbar presses are still `captured` but
`begin()` does not run — 17:23:03.761–05.063 is a 1.3 s mash of captured key 1 with no
`SH2_CastRight`. Owner: MSCO left-hand and SH2 both dead until sheathe/unsheathe. Live
inspect at 17:29 was already idle (Firebolt left, magicka 1000) — lockout had been cleared.
Do not treat sheathe as the fix; the graph has left a state that listens for `SH2_Cast*`.

