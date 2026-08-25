# 13 — Gray out ineligible Abilities in the Binding Menu

The Binding Menu lists every Ability the same way on every bar. The player cannot tell, while
editing, which rows will actually play on the bar they have selected.

**Blocked by:** 02, 07 (both resolved)

**Status:** resolved

## Grill (2026-08-23)

- Dual Wield bar is **any-match**: 1H, Dual, and Generic look live (rapier and two melee 1H both
  open that bar). HUD still grays Dual Flurry on a rapier at press time.
- Bind a gray row anyway. Gray is information, not a lock.
- Tint the Abilities tab **list and** the in-menu slot strip. Same empty-charge overlay as the HUD.
- Main / Melee are **union of children**. Sneak = parent. Magic / Ranged / Vampire Lord / Werewolf
  are all dead.
- No why-gray tooltip. Do not sort gray rows to the bottom.

## You test this

Profile `Nolvus Awakening`. Binding Menu, Abilities tab.

1. Select **Two-Handed**. Blood Flurry (2H) and Disengage (Generic) look live. A 1H ash and Dual
   Flurry look dead (empty-charge overlay, dim name). Drag Dual Flurry onto a slot anyway: it
   binds. The slot icon on that strip is also dead.
2. Select **Dual Wield**. 1H, Dual, and Generic look live; 2H looks dead. Switch bars: rows stay in
   catalogue order and retint without closing the menu.
3. Select **Magic** (or Ranged): every Ability looks dead. Select **Melee** or **Default**: 1H, 2H,
   Dual, and Generic all look live.

If Dual Flurry is dead on Dual Wield, or if a gray drop is refused, it fails.

## Agent tests the rest

4. `art_class_is_live_on_bar` any-matches Dual Wield (`ONEHAND_EMPTY` + `DUAL_WIELD`). Two-Handed
   is 2H + Generic only. Melee/Default union children. Magic/Ranged/VL/WW none live. Sneak id
   (`parent+1`) matches parent.
5. List and slot strip use `draw_cd_overlay(0)` keyed off `Bars::menu_bar_id`, not the drawn
   weapon. Spell/potion tabs unchanged. `apply_bind_drop` unchanged.

## What this is

Bind-menu eligibility tint from the selected bar. Reuses Ability Class. Does not hide rows.

## What this is not

Not HUD gray-out / MagFail (07). Not a new Ability Class. Not staff/bow art states. Not a Dual
Wield bar split. Not a why-gray tooltip.

## Comments

Owner 2026-08-21: filed from the post-09 punch list. Distinct from 07 (HUD vs live weapon).

Grill 2026-08-23: owner confirmed Q1–Q6 (any-match, bind anyway, list+slots, union of children,
no tooltip, keep catalogue order).

2026-08-23: Agent cells 4–5. `art_class_is_live_on_bar` + `art_data_test` green. Binding Menu list
and slot strip use `draw_cd_overlay(0)` from `Bars::menu_bar_id`. Drop still allowed. Plugin copied
to Dev - Spell Hotbar 2. Owner cells 1–3 remain.

Owner 2026-08-23: cells 1-3 pass — "gray functionality works great". Resolved. The follow-up the
pass surfaced, a second yellow tier for arts whose class *is* the selected stance, is ticket 14.
