# 19 — Root the player during a Driver Cast

**Type:** defect (driver / graph)

**What to build:** A Driver Cast roots the player for the duration of the cast. Currently the
player can move while the animation plays.

**Blocked by:** None — can start immediately.

**Status:** resolved

## How this showed up

Owner playtest, 2026-08-12, during ticket 11. Not combo membership. Not a child of ticket 11.
An SH2 cast is supposed to root; it does not.

- [x] The player cannot translate during a Driver Cast, owner-verified in game.
      Owner 2026-08-15: works for the left hand. Implementation is not hand-specific.
- [x] Rooting ends when the cast state ends, including a ticket-10 cut into an attack.
      Implemented (`cancel()` / `SH2_CastExit` clears `bAnimationDriven`); not a separate
      owner press. Owner close-out 2026-08-15 accepts it with the plant.
- [x] Restore fixtures and close Skyrim after runtime work. Owner review overrules teardown;
      game was left running on Save65 for the playtest that closed this.

The small MSCO clip step that this plant does **not** apply is
[ticket 21](21-apply-msco-animmotion-on-a-driver-cast.md), not a miss of this ticket.

## Addon — ticket 18 leftover (2026-08-14)

`load_charge_curve()` now runs at each Driver Cast `begin()`, after flushing the Win32 INI
reader cache so a just-saved `MSCO.ini` is visible. The in-game Save-vs-drag check was never
run here; it stays on [ticket 18](18-pace-the-hotbar-gcd-with-msco-clip-cadence.md).

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

**2026-08-15 — owner: rooting works for the left hand.** Same session, Save65 still live.
The walk-during-cast cell is closed for that case. The implementation is not hand-specific
(`is_active()` + `bAnimationDriven`), so a right-hand hotbar cast should match unless the
host graph disagrees.

**2026-08-15 — owner: close ticket 19.** Plant is accepted. The missing MSCO animmotion on
the same clips is [ticket 21](21-apply-msco-animmotion-on-a-driver-cast.md). The saved-curve
cell returns to [ticket 18](18-pace-the-hotbar-gcd-with-msco-clip-cadence.md).
