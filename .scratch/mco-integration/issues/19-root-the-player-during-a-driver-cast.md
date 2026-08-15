# 19 — Root the player during a Driver Cast

**Type:** defect (driver / graph)

**What to build:** A Driver Cast roots the player for the duration of the cast. Currently the
player can move while the animation plays.

**Blocked by:** None — can start immediately.

**Status:** claimed

## How this showed up

Owner playtest, 2026-08-12, during ticket 11. Not combo membership. Not a child of ticket 11.
An SH2 cast is supposed to root; it does not.

- [ ] The player cannot translate during a Driver Cast, owner-verified in game.
- [ ] Rooting ends when the cast state ends, including a ticket-10 cut into an attack.
- [ ] Restore fixtures and close Skyrim after runtime work.

## Addon — ticket 18 leftover (2026-08-14)

`load_charge_curve()` now runs at each Driver Cast `begin()`, after flushing the Win32 INI
reader cache so a just-saved `MSCO.ini` is visible. DLL is in `Dev - Spell Hotbar 2`
(SHA-256 `217FE7210085647457CE339949A9C22286EA42882CFA654B7F7A34413A7B524F`).

- [ ] Save a changed curve in MSCO's SKSE Menu Framework page (Save, not just drag). The next
      hotbar Firebolt logs a new `MSCO charge curve` line and writes the matching
      `MSCO_attackspeed`. Unsaved drags must not.

## Comments

**2026-08-15 — agent: rooted for the clip, unrooted at CastExit; owner cells still open.**
Profile `Nolvus Awakening`, Save65 (Xaelle, Iron Rapier, Firebolt left). DLL SHA-256
`217FE7210085647457CE339949A9C22286EA42882CFA654B7F7A34413A7B524F`. `combo_cache_test` green.
Log `SpellHotbar2.log`. Game left running on Save65.

Two layers, both gated on the shtb state (`MscoCastDriver::is_active()`), not the cast
instance: WASD capture via `is_movement_blocking_cast()`, and `bAnimationDriven` written at
entry / cleared at `SH2_CastExit` (including `cancel()` for a ticket-10 cut). Consecutive
clips never send CastExit, so they stay rooted.

Papyrus `Actor.GetAnimationVariableBool("bAnimationDriven")` on `0x14` during `castSlot(0)`:

| When | Value |
|---|---|
| rest | false |
| immediately after begin | true |
| +400 ms / +800 ms | true |
| +2.8 s (after CastExit) | false |

Log for that clip: `bAnimationDriven=true wrote=true` with `SH2_CastRight (clip 1) -> true`,
then `bAnimationDriven=false wrote=true` on the graph's `SH2_CastExit`. Walk-during-cast and
the attack cut still need an owner press; injected input does not reach `DispatchInputEvent`.
