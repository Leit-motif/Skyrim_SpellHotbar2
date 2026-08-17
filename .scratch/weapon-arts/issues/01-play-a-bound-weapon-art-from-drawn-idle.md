# 01 — Play a bound Weapon Art from drawn-weapon idle

Bind an art to a hotbar slot. Press that slot with a weapon drawn. The art plays. Nothing
equips or unequips. Ashes of War packs and the bind menu are later tickets.

**Status:** resolved — owner 1–3 passed; agent 3–7 passed

## You test this

Weapon drawn, 1h, standing still. Bind Test Art to a slot (agent can do the bind).

1. **Press the art slot.** You should see a special-attack animation, not idle and not a
   spell cast. Hands stay the same (same weapon, same spell).
2. **When it ends, left-click attack.** That swing should be the first hit of a combo, not
   hit 2 or 3.
3. **During the art, hold WASD.** You should not walk or strafe out of the clip. If the
   animation itself steps or lunges, you still move with it.

If 1 looks like idle or a twitch, it fails. If 2 chains from the wrong combo step, it fails.
If WASD walks you out of the clip, it fails.

## Agent tests the rest

3. **Sheathe, press the art slot.** Slot goes red. Log says sheathed. Hands unchanged.
4. **Drain stamina, press the art slot.** Slot goes red. Log says unaffordable. Hands
   unchanged.
5. **Press the art slot twice in a row.** First press plays. Second press (on cooldown)
   goes red. Log says cooldown. Hands unchanged.
6. **Bind an art, save, load that save.** The same slot still has the art. Pressing it
   still plays it.
7. **A spell on another slot.** Press it. The spell still casts. The art slot is untouched.

## Proven 2026-08-17 (Prisoner, Noble Rapier + Ice Spike)

Owner, live:

- [x] Case 1 — special-attack clip, not idle/twitch/spell; hands unchanged.
- [x] Case 2 — next left-click is combo hit 1.
- [x] Case 3 (WASD / Cast Plant) — owner after plant DLL + `SH2_Art_MG` wrap: WASD does not walk/strafe out of the clip.

Agent, SpellHotbar2.log:

- [x] Case 3 — sheathed press: `SH2_ArtStart not consumed (sheathed, mid-swing, or patch missing)` at 08:45:17. Reconfirmed 15:33:13 after persistence load.
- [x] Case 4 — `player.forceav stamina 0` then press: `SH2 art: unaffordable (need 25 stamina)` at 08:50:41. Hands unchanged. Stamina stayed 0.
- [x] Case 5 — play, wait for `SH2_ArtExit`, press again: `SH2 art: art 1 on cooldown` at 08:41:27. Hands unchanged. Owner also hit this live (the 8s Test Art CD).
- [x] Case 6 — `slotArt(0,1)` 15:31:39, save `SH2ArtBind03` 15:31:46 (`Saved 2 weapon art bind(s)`), load 15:31:51 (`WART: restored art 1`), `castSlot(0)` 15:32:17 skill type=8, `SH2_ArtStart` consumed, `SH2_ArtExit` 15:32:20. Hands still Noble Rapier + Ice Spike.
- [x] Case 7 — art bind does not overwrite other slots (`castSlot(1)`/`castSlot(2)` stayed empty type=0). Driver Cast SpellFire after the MSCO_left restore: 15:00:31 and 15:09:25 `SH2_CastEnter` / `MLh_SpellFire_Event` on this character.

Earlier agent proof (Save65) still stands: `SH2_ArtStart` consumed, selector 1 while live, MCO next-attack vars 1, stamina spent, selector 0 after exit.

## Notes

Test Art is 25 stamina and 8s cooldown (`GREATER_POWER` icon). That CD overlay is the intended “ability” read.

`AABL_Attack_A` was the ticket-01 inert placeholder. ADR-0009: the product clip is any
MCO-annotated HKX named by the catalogue, not that filename.

Art binds also persist through a dedicated SKSE `WART` record (bar id + slot + modifier + art id) so a 1h+spell bar does not fall back to an inherited spell after load.

## Answer

A bound Weapon Art plays from drawn idle without equipping, plants WASD like a Driver Cast, refuses sheathe/stamina/cooldown correctly, and survives save/load on `SH2ArtBind03`.
