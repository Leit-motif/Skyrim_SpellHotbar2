# 07 — Gray out a Weapon Art on the wrong Art Class

A bound art must sit on the bar even when the drawn weapon does not match. It must look dead and
refuse the press, not play a different clip. Story 30. WoW dead-ability, not Ashes of War
fall-through.

**Blocked by:** 03 (resolved)

**Status:** ready-for-agent

## You test this

Profile `Nolvus Awakening`. Rapier (1H) drawn, then a greatsword, then dual 1H.

1. Bind a **2H** art (Blood Flurry), a **Generic** art (Disengage), and a **Dual** art (Dual
   Flurry) to three slots. With the rapier: 2H and Dual are gray; Generic is live. Press 2H: fail
   sound, red highlight, Art Selector stays 0, stamina unchanged. Press Generic: the art plays.
2. Draw a greatsword: 2H goes live; Dual stays gray. Draw two melee 1H: Dual goes live; 2H grays.
3. Bind still works on a gray art — the slot keeps the art id. Swapping back to a matching weapon
   un-grays it.

If Blood Flurry plays on a rapier, or if a gray press writes the selector / spends stamina, it
fails.

## Agent tests the rest

4. `ArtDefinition` has an Art Class field. Catalogue TSV has a column. Values are only `1H`,
   `2H`, `Dual`, `Generic`.
5. `generate_art_pack.py` collapses the author’s OAR type ORs into those four. Mixed ashes
   (sword **or** greatsword) → Generic. Dual-both-hands gates → Dual. Keyword-only / unarmed-punch
   ashes → Generic. SH2 `config.json` conditions stay selector + player; **no** `IsEquippedType`
   copied onto our configs.
6. Custom / template rows default Generic. `try_start_art` refuses mismatch **before**
   `set_art_selector`. HUD gray uses the existing empty-charge / cooldown overlay (or icon tint),
   keyed off `getPlayerEquipmentType()` already fetched for bar switching. No extra per-frame
   record walk.
7. Bow / staff / magic: all four classes gray (no art state). Staff un-gray waits on a later
   SH2.mco staff effort.

## What this is

**Press and draw policy** for weapon class. Cheap: one enum compare on a path that already looks
up the art and already knows equipped type.

## What this is not

Not OAR fall-through to Sword Neutral. Not play-anyway. Not 14-way Nolvus types. Not Custom Art
Folders (08). Not icons (06).

## Notes

Usable map (melee):

| Tag | Live when `EquippedType` is |
|---|---|
| 1H | ONEHAND_EMPTY, ONEHAND_SHIELD, ONEHAND_SPELL |
| 2H | TWOHAND |
| Dual | DUAL_WIELD |
| Generic | those four plus FIST |

`try_start_art` already refuses CD and stamina the same way (MagFail, FlashMeter, error
highlight).

## Comments

Grill 2026-08-18: W chosen after costing it against fall-through. Dual is its own tag (MVP).
