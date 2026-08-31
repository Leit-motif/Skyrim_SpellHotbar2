# 19 — The parent bars' ability tint says nothing, because they name no stance

Filed by the owner, 2026-08-31: *"currently our logic for weapon art availability coloring,
yellow, white, gray, works for specific sets, but the logic doesn't work for Default. The ideal is
that for Default, it checks your currently equipped weapon and then gives the permitted logic."*

**Blocked by:** nothing. Continues the tint lineage of tickets 13, 14, and 17.

**Status:** DONE — owner-accepted 2026-08-31, live in game on both Default and Melee.

## The defect

`$MAIN_BAR` renders as **"Default"** (`localization.cpp`). Its bar id is `'MAIN'`, and
`art_class_is_live_on_bar` treats `'MAIN'` as the union of every melee child plus fists — so every
ability class is live there. `art_class_is_direct_on_bar` returns false for `'MAIN'`, so no class
is ever direct. Both halves of `art_bar_tint` therefore collapse: **on Default every ability reads
white**, and the coloring carries no information at all.

The sharper half of the same bug: the runtime gate has never agreed with that. Both
[hotbar.cpp:753](../../../skse_plugin/src/bar/hotbar.cpp) and
[casting_controller.cpp:1835](../../../skse_plugin/src/casts/casting_controller.cpp) refuse a cast
on `art_class_is_live(art_class, equipped_type)` — the player's actual equipment. So on Default
with a bow, a crossbow, or a staff out, every ability stayed white in the menu while the game
refused all of them.

## The fix

Resolve Default against equipment instead of against a stance the bar does not pin. Two constexpr
additions in [art_definition.h](../../../skse_plugin/src/game_data/art_definition.h):

- `art_equipped_tint(ArtClass, EquippedType)` — dead when the runtime would refuse it, generic for
  a live Generic, direct otherwise. A non-Generic class is live only for the equipment that owns
  it, so live-and-not-Generic *is* the direct match; no second ownership table exists to drift.
- `art_bar_pins_no_stance(bar_id)` — true for Default and Melee, the only two bars that stand for
  a set of stances rather than one. Sneak twins share their parent's stance root and follow.
- `art_bar_tint_for_player(ArtClass, bar_id, EquippedType)` — a parent bar defers to equipment;
  every bar that names its own stance keeps the existing bar-based answer unchanged.

Both bind-menu call sites — the Abilities list and the bound-slot strip — now pass
`GameData::getPlayerEquipmentType()`.

Melee was added on the owner's call the same day: *"do 'mele' too. might as well go for full
coverage."* It has the identical defect — it unions its melee children and owns nothing, so nothing
was ever yellow there either.

## What the player sees on Default and Melee

| Equipped | Yellow | White | Gray |
|---|---|---|---|
| Two-handed | 2H | Generic | 1H, Dual |
| Dual wield | Dual | Generic | 1H, 2H |
| One-hand + shield/spell/empty | 1H | Generic | 2H, Dual |
| Unarmed | — | Generic | 1H, 2H, Dual |
| Bow, crossbow, spell, staff | — | — | everything |

## Acceptance

- [x] `art_data_test` covers the table above, the bow/staff all-gray case, the Sneak twin, and that
      no other bar's answer moved. Also asserts the tint's `dead` tier is exactly the negation of
      `art_class_is_live` across all 11 `EquippedType` values — the menu cannot color an ability
      the cast would refuse, nor gray one it would allow.
- [x] Full Release DLL builds at `/W4`; CTest 9/9.
- [x] **Owner cell — PASSED 2026-08-31.** Owner tested Default and Melee in the live bind menu
      and confirmed the tint works on both: *"I tested the default. Tested melee as well. It's
      working."* Save `Save4_…_20260831153204`, profile Nolvus Awakening, DLL `3e14465d`, session
      launched 11:47. The ImGui hotbar is not agent-capturable, so this cell was always the
      owner's.

## Consequence worth knowing

A parent bar now answers with the equipment, full stop. Hold a bow and *both* Default and Melee
gray every ability — truthful about what would fire right now, and it is the same rule in both
places. Neither bar is the active one while a bow is out, so this shows up only when browsing
those bars in the menu with ranged gear equipped. If it ever reads wrong, the alternative is one
line: fall back to `art_bar_tint` when the equipment opens no melee stance.
