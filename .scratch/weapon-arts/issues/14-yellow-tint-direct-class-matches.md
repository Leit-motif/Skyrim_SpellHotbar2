# 14 — Tint direct Ability Class matches yellow in the Binding Menu

Ticket 13 gives the player two states in the Abilities tab: live and dead. On a stance bar that
any-matches, live covers two different things. A Generic ash runs on every stance, so seeing it
live on Two-Handed says nothing about Two-Handed. A 2H ash runs on that stance and no other. The
player cannot tell the specific ash from the universal one while editing.

**Blocked by:** 13 (resolved)

**Status:** DONE — owner-accepted 2026-08-25. The Abilities list passed all three owner cells.
The bound-slot strip's behavior went to [ticket 17](17-tint-the-bound-slot-strip.md), which this
ticket explicitly left out of scope.

## Grill (2026-08-23)

- Yellow means the art's Ability Class **is** this bar's stance. White means live but not specific.
- Generic is never yellow. It is live everywhere, so it is white everywhere.
- Parent bars (Default, Melee) are all white. They union their children, so nothing is direct.
- Dual Wield: Dual is yellow, 1H stays white — a rapier opens that bar, so 1H is a side effect of
  the any-match, not a direct match.
- Gray still wins over yellow. A dead row is dead.
- Shield-only arts, keyed off the MCO left-hand-attack annotation, are **out of scope**. There is
  no Ability Class for them, so 1H Shield tints 1H yellow like the other one-hand bars.

## You test this

Profile `Nolvus Awakening`. Binding Menu, Abilities tab.

1. Select **Two-Handed**. Blood Flurry (2H) is yellow. Disengage (Generic) is white. Dual Flurry
   (Dual) is still gray.
2. Select **Dual Wield**. Dual Flurry is yellow. A 1H ash is white, not yellow. Blood Flurry is
   gray.
3. Select **Default** or **Melee**. Nothing is yellow — 1H, 2H, Dual and Generic all read white.

If a Generic ash goes yellow on any bar, or a gray row goes yellow, it fails.

## Agent tests the rest

4. `art_class_is_direct_on_bar` is true only for 2H on Two-Handed, Dual on Dual Wield, and 1H on
   the two one-hand bars. Generic is false everywhere. Default, Melee, Magic, Ranged, Vampire Lord
   and Werewolf are false for every class. Sneak ids (`parent+1`) match their parent.
5. Direct implies live: no bar/class pair is direct without `art_class_is_live_on_bar` also
   holding.
6. The Abilities tab list applies gray first, then yellow, then plain. Slot strip, spell and potion
   tabs, and `apply_bind_drop` are unchanged.

## What this is

A second tint tier on top of 13, driven by the same Ability Class and the same selected bar.

## What this is not

Not a new Ability Class. Not a shield-only class (see Grill). Not HUD tinting — the HUD knows the
live weapon, so it has no any-match to disambiguate. Not a sort order change.

## Comments

Owner 2026-08-23: filed from the wa13 acceptance pass. Gray works; the missing information is
which live art is *specific* to the selected stance.

2026-08-23: Agent cells 4–6. `art_class_is_direct_on_bar` sits beside `art_class_is_live_on_bar`
in `art_definition.h` and reuses `art_bar_stance_root` for sneak ids. The Abilities tab pushes gray
first, then yellow (`IM_COL32(255, 210, 74, 255)`), so a dead row can never read as direct.
`art_data_test` green, including a cell that asserts every direct pair is also live. DLL built and
deployed to `Dev - Spell Hotbar 2`; the new build was confirmed loading and rendering in game.

The yellow itself is **not** visually verified. Reaching the Abilities tab and the bar dropdown
needs a mouse, and DevBench's `input` tool injects button events only — it cannot position the
cursor. Owner cells 1–3 stand.

Owner acceptance 2026-08-25 — **cells 1–3 PASS.** Fixture: the ticket-38 DLL (11:33 build), the
ticket-16 stamped art pack, and the ticket-06 icon atlas at MO2 priority 4466; save `Save25`,
profile `Nolvus Awakening`.

- Two-Handed: Blood Flurry and Crushing Blow render yellow; Aimed Blow, Akatosh Charge, Crane
  Style, Cyclone Spin, Dash Slam, Disengage render white; Blood Seeker, Blood Spiller,
  Champion's End, Divided Strike render gray. No Generic art went yellow.
- Dual Wield: Ripping Hour renders yellow while the surrounding Dual-eligible rows stay white and
  Shadow Reave / Shadow Slash / Shoulder Slam stay gray.
- Default: no yellow anywhere. Owner: *"the left side is working perfectly... For default, there
  is no yellow."*

Owner screenshots (three, 15:24–15:31) are the evidence; they are 3440x1440 BMPs in the profile's
`STOCK GAME/Screenshots/` and were read directly rather than committed. Note for future passes:
Community Shaders' screenshot path produced black frames and the owner disabled it to capture
these.
