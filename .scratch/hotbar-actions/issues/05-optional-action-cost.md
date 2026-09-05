# 05 — Optional Action Cost

**Type:** task

**What to build:** Before inject, an Action may charge stamina / magicka / health and
enter cooldown / GCD, using the Ability Cost helpers. Defaults are all zero (free).
Unaffordable or on cooldown: red flash, meter flash, `sound_MagFail`, no inject.
HUD shows cooldown overlay when a costed Action is on CD.

**Blocked by:** 04

**Status:** ready-for-agent

## You test this

1. Default Power Attack (zero costs). Press. Move plays. No meter drain.
2. Editor: set 25 stamina. Press with stamina. Move plays, stamina drops, slot shows CD
   if cooldown is set.
3. Drain stamina, press. Red flash, stamina meter flashes, no move.
4. Press again during cooldown. Red flash, no second inject.

## Agent tests the rest

5. Magicka-only and health-only costs flash the matching meter.
6. GCD: a costed Action with GCD > 0 refuses a second press inside that window even if
   cooldown_days is 0.
7. Zero remaining (all costs 0, no CD) never calls FlashMeter.

## Notes

Call the same unaffordable / deduct / `add_art_cooldown` shape as `try_start_art`, but do
not go through ArtDriver. Payload Interpreter costs stay unused.

A costless attack Action must still cut a committed cast (ticket 04 / ticket 10). Do not
put that cut behind "has a cost."
