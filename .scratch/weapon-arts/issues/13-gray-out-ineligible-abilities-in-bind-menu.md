# 13 — Gray out ineligible Abilities in the Binding Menu

The Binding Menu lists every Ability the same way on every bar. The player cannot tell, while
editing, which rows will actually play on the bar they have selected.

**Blocked by:** 02, 07 (both resolved)

**Status:** needs-triage

## Owner ask (2026-08-21)

> in the binding menu, based on the active/selected Bar, abilities that are ineligible should be
> greyed out so the player knows what works for each bar they are on.

## You test this

(Unwritten until triage.) Open the Binding Menu, Abilities tab. Select the Two-Handed bar: 1H and
Dual rows look dead; 2H and Generic look live. Select Dual Wield: Dual and Generic look live.
Select Magic or Ranged: every Ability looks dead (no shtb art state yet). Switch the bar combo:
the list retints without closing the menu.

## What this is

**Bind-menu eligibility tint** keyed off `Bars::menu_bar_id` (the combo on the right), not off the
weapon currently drawn. Ticket 07 already grays the **HUD slot** from live `getPlayerEquipmentType()`
and refuses the press. This ticket is the catalogue list (and, if triage says so, the in-menu slot
strip) so the player can see the same policy while binding.

Reuse `art_class_is_live` / Ability Class. Do not invent a second class enum.

## What this is not

Not HUD gray-out / MagFail (07, resolved). Not a new Ability Class. Not staff/bow art states
(spec: those bars stay dead until a later SH2.mco staff effort). Not hiding rows — gray, still
listed.

## Known mapping trap (must answer in triage)

`getCurrentHotbar_ingame` sends **both** `ONEHAND_EMPTY` (rapier) and `DUAL_WIELD` (two melee 1H)
to `DUAL_WIELD_BAR`. Live HUD gray is equipment-accurate; bar-level gray cannot be both “1H live”
and “Dual live” unless the policy is “live if *any* equipment that uses this bar would accept it.”

| Selected bar | Equipment types that actually open it |
|---|---|
| Dual Wield (`1HDW`) | ONEHAND_EMPTY **and** DUAL_WIELD |
| One-Hand Shield | ONEHAND_SHIELD |
| One-Hand Spell | ONEHAND_SPELL |
| Two-Handed | TWOHAND |
| Magic | SPELL, STAFF_SHIELD — all Ability Classes dead |
| Ranged | BOW, CROSSBOW — all dead |
| Main / sheathed / fists | MAIN_BAR; Generic live on FIST; 1H/2H/Dual dead on fists |
| Sneak variants | same parent bar + 1 |

## Triage

1. **Policy on Dual Wield bar.** Any-match (1H, Dual, and Generic all live) vs split the bar
   (out of this ticket) vs something else.
2. **Bind a gray row?** 07 allows bind anyway so the slot waits for a matching weapon. Same here,
   or refuse the drop?
3. **In-menu slots.** Tint already-bound wrong-class icons on the selected bar’s strip, or only
   the Abilities tab list?
4. **Parent bars.** Melee / Main are inheritance parents, not a single EquippedType. Gray
   everything except Generic, or skip tint on parent bars?

## Notes

List draw is `advanced_bind_menu.cpp` Abilities tab (`list_of_arts_filtered`). It never consults
`art_class_is_live` or `menu_bar_id`. Slot icons in that window call `draw_art_icon_in_editor`
without the HUD’s empty-charge overlay.

## Comments

Owner 2026-08-21: filed from the post-09 punch list. Distinct from 07 (HUD vs live weapon).
