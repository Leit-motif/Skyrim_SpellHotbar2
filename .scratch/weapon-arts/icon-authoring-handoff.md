# Weapon Art icon authoring handoff

Updated: 2026-08-23

## Resume here

- Worktree: `C:\Nolvus\Projects\spell-hotbar-2-ticket-06-icons`
- Branch: `ticket-06-weapon-art-icons`
- Integration target: `weapon-arts`, not `main`
- Active art: **13 — Divided Strike** (`aow_13_divided_strike`)
- Status: **in progress; no Divided Strike candidate is approved**
- Inventory: **11 of 57 finalized; 46 remain**

Do not restart the set and do not regenerate Crane Style through Disengage. Their approved assets,
prompts, and notes are already recorded in `python_scripts/weapon_art_icons/manifest.tsv`.

## Exact Divided Strike stopping point

The animation evidence supports a three-beat dual-weapon sequence: off-hand collision first,
main-hand collision second, then both weapon nodes collide in the terminal beat. The intended icon
is a figure visibly moving through that attack and ending in a cross-cutting X strike. The pose must
create the attack; luminous trails cannot be pasted over an idle or weapon-display stance.

The closest candidate before pausing is locally preserved at:

`python_scripts/weapon_art_icons/pilot/source/divided_strike_closest_v5.png`

Owner assessment: “better,” with the advancing motion accepted as progress, but the X still needed
to be **convex**, and the dagger might need to change to make the physical trajectory coherent.

The most recent candidate is locally preserved at:

`python_scripts/weapon_art_icons/pilot/source/divided_strike_rejected_convex_v6.png`

That candidate is explicitly **wrong**. It interpreted convexity as two large continuous opposing
C-curves, producing an hourglass/parenthesis shape rather than the owner's intended convex X. Do
not continue from its trail geometry and do not call it approved.

Before another generation, establish the intended convex-X geometry with the owner in plain shape
language or a quick owner-supplied sketch/reference. Preserve the moving lunge from the closest v5
candidate, then change the weapon and trail geometry together. Do not guess at “convex” again.

## Locked project direction

- Original generated art only; no third-party artwork or Bethesda assets are copied or shipped.
- MMO/RPG hotbar convention: generalized faceless figure, one dominant action, readable at 32 px.
- Elder Scrolls race and palette shorthand live in `figure-guidance.md` and `color-guidance.md`.
- Backgrounds should vary across the atlas without defaulting every icon to black or adding haze.
- Catalogue wiring waits until the atlas contains the approved stable keys.

