# 17 — Do not deliver clip 4 during its windup

**Type:** defect (driver timing)

**What to build:** A hotbar cast on clip 4 must not release the spell during the clip's windup.
Clips 1–3 releasing near the start of the animation is fine. Clip 4 has a windup, and the spell
currently fires in the middle of it, which looks wrong.

**Blocked by:** None — can start immediately.

**Status:** claimed

## How this showed up

Owner playtest, 2026-08-12, during ticket 11. Not combo membership. Not a child of ticket 11.
The session log already showed clip 4 hitting the 0.5 s authored-time floor before its SpellFire
annotation at ~0.92 s — same symptom, now owner-confirmed by eye.

ADR-0006 says the annotation leads and the authored cast time is the floor. Clip 4 is the case
where the floor is earlier than the animation's release pose.

- [ ] Clip 4 delivers at the release pose, not during the windup, owner-verified by eye.
- [ ] Clip 4 does not lock the player out of MSCO left-hand or hotbar casts (sheathe/unsheathe
      must not be required to continue).
- [x] Clips 1–3 still deliver at the start of the animation, unchanged.
- [x] Restore fixtures and close Skyrim after runtime work.

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

**2026-08-13 — agent: timer no longer fires clip 4 in the windup.** Profile `Nolvus Awakening`,
Save65 (Xaelle, Iron Rapier, Firebolt left). `castSlot(0)` walked clips 1→2→3→4. DLL SHA-256
`A4E23129FC003EE61E678584D882602DAB7D7E8FD2AF12D1A7CDADE00E258246` (later rebuilt after the
clips 1–3 SpellFire-leads correction; clip 4 path unchanged). Log `SpellHotbar2.log` from 18:02.

```
SH2_CastRight (clip 1) -> true   SpellFire +0.452s
SH2_Cast2 (clip 2) -> true       SpellFire +0.283s
SH2_Cast3 (clip 3) -> true       SpellFire +0.361s
SH2_Cast4 (clip 4) -> true       SpellFire +0.918s
SH2_CastExit -> true
SH2_CastRight (clip 1) -> true   follow-up after clip 4, no sheathe
```

No `delivering on the timer floor` line. The 0.5s authored time no longer releases clip 4;
delivery waits for SpellFire. Clips 1–3 still deliver at SpellFire (annotation leads), not at
0.5s. Follow-up `begin()` ran; `CastExit -> true`. MSCO left-hand lockout still needs an owner
press (`castSlot` does not reach `DispatchInputEvent`). Pose cell stays open for the owner.

`classify_cast_delivery` on `combo_cache.h`; `combo_cache_test` green. Save65 reloaded
(magicka 1000, health 500, Firebolt left, Iron Rapier); `qqq`; DevBench ping offline.

