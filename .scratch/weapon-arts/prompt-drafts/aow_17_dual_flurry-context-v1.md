# Dual Flurry context-layer forward test

> **Superseded 2026-08-23.** The owner approved an externally generated final cut. This document is
> retained as prompt-workflow evidence only and must not be used to regenerate or replace
> `python_scripts/weapon_art_icons/source/aow_17_dual_flurry.png`.

Status: prompt-only validation draft; generation paused

## Resolved brief

- **Ability:** Dual Flurry (`aow_17_dual_flurry`)
- **Animation evidence:** 1.666667 seconds; roughly 284 units of advance; both equipped weapons hit
  together at 0.583333; both hit together again at 0.883333 with knockback.
- **Active animation resolution:** the installed Spell Hotbar OAR overlay points to
  `Nolvus Ashes of War Stance Framework/Dual Flurry/AABL_Attack_A.hkx`; enabled duplicate providers
  share SHA-256 `6BEC732875773F40770EC7AC70F0795BDF641947FF2B189D4431B875FE75CBD2`.
- **Owner direction:** a leaping Nord barbarian clad in furs, dual-wielding axes, physically
  performing a red cross strike.
- **Frozen moment:** the apex of the forward leap as both axes pass through the center of the
  cross-cut and the second paired hit releases its knockback.
- **Skyrim identity:** Nord barbarian/fur warrior, not Stormcloak, housecarl, plate champion, or
  horned-helmet mascot.
- **Equipment:** rough layered animal fur and hide, coarse leather straps and wraps, exposed arms,
  small weathered iron pieces, two compact one-handed bearded iron war axes.
- **Camera:** dynamic three-quarter side view; travel across and slightly toward the viewer; both
  feet visibly airborne.
- **Effect:** exactly two crimson-red weapon trails cross once immediately in front of the body;
  each axe head remains at the moving edge of its own trail; one compact red-white knockback flash.
- **Hierarchy:** moving cross-strike first, airborne fur-clad silhouette second; readable at 32 px.
- **Reference policy:** no rejected Dual Flurry candidate is a content reference. Approved project
  icons may supply painted MMO grammar only.
- **Provenance:** paired hits, knockback, duration, and travel are animation-proven; the leap, Nord
  barbarian identity, furs, axes, and red cross are owner-directed staging; fur/hide construction
  and compact iron-war-axe vocabulary are Skyrim-reference-derived; the three-quarter camera and
  charcoal field are agent composition choices.

## Generation prompt

```text
Create an original square painted MMO ability icon for Skyrim Spell Hotbar 2, composed for immediate readability at 32 × 32 pixels.

Freeze a Nord barbarian at the apex of a violent forward leap while he performs a simultaneous dual-axe cross-cut. Show him in a dynamic three-quarter side view, moving across and slightly toward the viewer. Both feet are clearly off the ground, his knees trail behind him, his torso twists through the strike, and the rough fur and leather at his shoulders and waist stream backward with the jump.

The barbarian has a powerful but generalized faceless silhouette. He wears rough layered gray-brown animal fur and hide, coarse dark-leather straps and wraps, exposed muscular forearms, and only a few small pieces of weathered iron protection. His head and face remain shadowed beneath wind-tossed hair. He is a Skyrim fur warrior, not a plated soldier or ceremonial champion.

He grips exactly two compact one-handed bearded iron war axes: short wooden hafts, practical dark-iron heads, broad cutting edges. His arms actively sweep across one another in front of his torso. The screen-left axe travels from upper-right toward lower-left; the screen-right axe travels from upper-left toward lower-right. Each moving axe head sits at the leading edge of its own luminous crimson slash trail. The two red trails cross exactly once immediately in front of him, forming one clean ordinary X because the body and axes physically create it. A compact white-red impact flash and tight dark-crimson pressure ring at the crossing point communicate the second paired hit's knockback.

The red cross-strike is the primary read and occupies about sixty percent of the square; the airborne fur-clad barbarian occupies about forty percent. Use a quiet charcoal, soot-black, weathered-brown, and muted iron background so the crimson trails dominate. Keep the painted texture rugged and material-first, with restrained sparks and no scenery.

One airborne figure, two one-handed axes, two red trails, one crossing impact. Preserve the leaping action, fur-and-hide construction, physical weapon-to-trail continuity, safe crop, and bold MMO glyph silhouette. Exclude a standing guard, crossed-weapon emblem, heavy plate cuirass, Stormcloak uniform, horned mascot helmet, elemental ice or fire, extra major slashes, portrait framing, text, logo, border, and watermark.
```

## Forward-test result

The prompt resolves the failures that reached the owner:

- `Nord` is separated from `Nord barbarian` and from plated or faction armor.
- the leap is defined through feet, knees, torso, and trailing material rather than the word
  `dynamic` alone;
- both axes have class, scale, hand ownership, path, and endpoint;
- the X is caused by the moving axes rather than placed behind a static pose;
- red is owner-directed physical attack language, not an inferred elemental palette;
- prior rejected candidates carry no content into the next generation.
