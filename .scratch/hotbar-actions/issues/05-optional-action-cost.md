# 05 — Optional Action Cost

**Type:** task

**What to build:** At the accepted down edge, an Action may charge stamina / magicka /
health and enter cooldown / GCD, using the Ability Cost helpers. Charging happens once per
press: mirrored held events and the up edge bypass admission, cost, cooldown, and GCD. Defaults are all zero (free).
Unaffordable or on cooldown: red flash, meter flash, `sound_MagFail`, no inject.
HUD shows cooldown overlay when a costed Action is on CD.

**Blocked by:** 04

**Status:** superseded — rolled into 02

**Status (superseded — rolled into 02):** ready-for-agent

## You test this

1. A default Action (zero costs). Press. Move plays. No meter drain. Superseded
   2026-09-05: this cell previously named a shipped Power Attack default.
2. Editor: set 25 stamina. Press with stamina. Move plays, stamina drops, slot shows CD
   if cooldown is set.
3. Drain stamina, press. Red flash, stamina meter flashes, no move.
4. Press again during cooldown. Red flash, no second inject.
4b. Hold a costed Action for several seconds. Cost, cooldown, and GCD are charged once.

## Agent tests the rest

5. Magicka-only and health-only costs flash the matching meter.
6. GCD: a costed Action with GCD > 0 refuses a second press inside that window even if
   cooldown_days is 0.
7. Zero remaining (all costs 0, no CD) never calls FlashMeter.
8. A refused down edge starts no mirror, so no target is left held.

All runtime cells here are unproven.

## Notes

Call the same unaffordable / deduct / `add_art_cooldown` shape as `try_start_art`, but do
not go through ArtDriver. Payload Interpreter costs stay unused.

A costless attack Action must still cut a committed cast (ticket 04 / ticket 10). Do not
put that cut behind "has a cost."
