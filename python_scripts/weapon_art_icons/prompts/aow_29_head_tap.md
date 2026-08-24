# Head Tap

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_29_head_tap`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Head Tap`. Selected
  enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Head Tap/AABL_Attack_A.HKX`;
  SHA-256 `B989B53C54D14856AD62A1C65B88A8A4B6B446F8A31C4C5352281C570D245ED2`.
- Animation-proven: the 3.333333-second, 99-track, 2H-class clip advances about 232 units and
  contains six weapon swings/hits from 0.166666 through 1.766666 seconds. It then triggers
  `$ES_HeavyStrikefly` at 1.866666; `$CostWarAshPoint1` occurs at the start.
- Payload resolution: no active definition was found for either named payload. Installed Argonian
  warhammer, battleaxe, and greatsword move-set annotations reuse `$ES_HeavyStrikefly`, supporting
  a heavy physical launch interpretation but proving no element, color, or exact VFX.
- Agent composition choice: one Argonian with one two-handed rectangular steel warhammer, one short
  vertical trail, and one final downward contact. Six hit events are not multiplied into trails.
- Atlas choice: a frontal vertical foreshortened hammer and swamp-green/teal/dark-brown palette
  follow the failed high axe arc, perspective furrow, and Imperial diagonal shield wedge.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style two-handed weapon art named "Head Tap". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
One—and only one—compact downward warhammer strike dominates 85-88% of the square. The practical rectangular steel hammer head is the largest crisp object near the lower-center, shown at the instant it delivers one precise downward contact into a tiny ivory-white compression point. A single short vertical steel-white motion trail extends directly upward behind that same hammer head, brightest immediately behind it and tapering toward the top. One restrained circular puff of physical dust and a few orange steel sparks spread from the exact contact point.
No second trail, no repeated hammer echoes, no rings, no floating effect, no target head, no victim, and no gore.

FIGURE, GRIP, AND CAMERA:
Low frontal three-quarter camera looking slightly upward along the hammer path. One tiny faceless Argonian two-handed fighter occupies no more than 10-12% near the upper-right source end, subordinate to the hammer head and vertical strike. Freeze the final compact follow-through: body crouched and leaning down, shoulders stacked over the haft, both hands clearly spaced on one long grip, elbows bent, hammer axis vertical into the contact.
Exactly ONE practical Skyrim-style two-handed warhammer: one simple rectangular iron/steel head, one plausible long wooden haft, two hands on the same haft. No axe blades, pick spike, sword, shield, second weapon, detached head, or oversized fantasy ornament.

EVIDENCE AND SKYRIM VISUAL LANGUAGE:
The verified 3.333-second 2H clip advances roughly 232 units and contains six timed swings/hits before an unresolved heavy-strike launch payload. Depict only the defining final downward hit and one motion trail, not six hits or six trails. The named payloads do not prove an element or color.
Argonian identity through economical full-body silhouette/material cues only: small closed hood/helm with a subtle backward crest outline, narrow tail balancing the crouch, layered dark-brown leather and swamp-green cloth, restrained teal stitching and one muted orange sash. Face fully hidden; no eyes, scales, snout detail, skin close-up, or costume showcase.

STYLE AND COLOR:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact graphic masses, high contrast, restrained texture. Dominant ivory-white/steel vertical strike against a quiet deep swamp-green and charcoal-black field; tiny source figure in dark brown, swamp green, restrained teal, and muted orange. No magical glow, fire, frost, lightning, poison, or rune. Dark vignette, safe crop, edge-to-edge art.

HARD CONSTRAINTS:
exactly one tiny faceless full-body Argonian, exactly one two-handed warhammer with two hands on one haft, exactly one short connected vertical trail, one physical contact. No portrait, face, large character, multiple trails, repeated hammer, giant ornate weapon, victim, head, blood, gore, spell, scenery, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full-resolution result: pass. One foreshortened rectangular hammer, one haft with two clear grips,
  one vertical trail, and one ground contact dominate; the Argonian is faceless and subordinate.
- 32 px LANCZOS reduction: pass. The hammer head, luminous vertical axis, and compact impact remain
  an immediate single-strike glyph.
- Master SHA-256: `F331BC6BFF317E7365CC85A245894FB79A6D13061DBA6A2D893D4590C6F53DC4`.
