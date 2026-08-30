# 07 — Gallery screenshots

**Type:** task
**Status:** ready-for-agent
**Blocked by:** 03

Eight to nine images after the key art, one per thing the page has a section about. The model page
uses exactly this count and the images are plain: 2560x1440, no annotations, no arrows, no burned
captions. UI panel on one side, character on the other, background thrown out of focus. The
restraint is the polish.

## Shot list

One per page section, so the gallery and the description are the same document read two ways.

1. The hotbar in combat, weapon drawn, a cooldown running.
2. The bind menu open on a slot, showing a spell being bound.
3. The same menu binding an **Ability** — the thing base SH2 cannot hold in a slot.
4. Settings, whichever surface ticket 01 rules ships (Mod Control Panel pages, or the MCM).
5. The bar editor / drag layout.
6. Per-hand casting mid-frame: weapon right, spell left.
7. The slot strip showing its tint tiers — bound, on cooldown, unavailable — in one frame.
8. A weapon art mid-swing with its slot lit.
9. Optional: the Oblivion-mode bar, if it ships and looks distinct.

## Rules

- **Depth of field on the background.** The model page's shots are legible because the world behind
  the panel is thrown out of focus; a busy Nolvus interior behind a dense panel is unreadable.
- **Consistent time of day and weather across the set.** Nine screenshots at nine different
  lightings read as nine sessions.
- **Same resolution for all of them.** Mixed aspect ratios in a gallery look careless at thumbnail
  size, which is where they are first seen.
- **No text on the images.** If a shot needs a caption to make sense, the shot is wrong. Nexus has
  a caption field per image; use that.
- The ImGui surfaces will not appear if captured through the engine's own screenshot path — see
  ticket 03. Use the same desktop-level path the video uses.

## Acceptance

- [ ] Eight or nine stills, all at one resolution, committed under `.scratch/release/media/`.
- [ ] Every description section has at least one image, and every image belongs to a section.
- [ ] Each one checked at Nexus thumbnail size, not only at full size.
