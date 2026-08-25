# Heart Strike

**Status:** Autonomous from-scratch completion regeneration passed at 128 px on 2026-08-24

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_31_heart_strike`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Heart Strike`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Heart Strike/AABL_Attack_A.HKX`;
  SHA-256 `4EEB9327281D25CE0BA6FABABEA5BCBC7DC5CDBD3068EFF69FF5D892EDAF9927`.
- Animation-proven: the 1.666667-second, 97-track, 1H-class clip advances about 390 units and opens
  three equal-damage weapon collisions. Every collision triples trail lifetime and brightness; the
  third hit at 0.966665 seconds carries `$AoW_Knockback`.
- Payload resolution: no active definitions were found for `$AoW_Knockback`, `$DummyPowerCost`, or
  `$DummyMagcost`. They prove no heart, blood, element, color, or exact VFX.
- Agent composition choice: one faceless Breton with one one-handed flanged mace and one defining
  final rising trail. Three collisions were intentionally distilled to one trail.
- Atlas intent: a tight lower-right to upper-left silver-blue crescent on burgundy-charcoal followed
  the horizontal Redguard thrust, vertical Argonian hammer, and perspective Nord furrow.

## Final generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for a Skyrim-style one-handed weapon art named "Heart Strike". This is an ability glyph, not a portrait, character key art, splash illustration, wallpaper, or scene.

PRIMARY GLYPH:
One—and only one—tight rising physical mace trail dominates 85-88% of the square. A compact flanged steel mace head leads from lower-right toward upper-left along one bold silver-blue crescent. The crescent is brightest immediately behind the moving mace head, tapers backward toward lower-right, and remains open rather than forming a ring. At the upper-left leading end, one compact ivory-white pressure nick and a few pale-gold steel sparks mark the final knockback contact.
No second crescent, trail echo, thrust line, heart shape, blood, ring, X, floating effect, or detached impact.

FIGURE AND WEAPON:
One tiny faceless Breton fighter occupies no more than 10-12% at the lower-right source end, subordinate to the mace head and single rising trail. Freeze the terminal cross-body upswing: body crouched low, front knee bent, torso rotating left, right arm driving the mace upward across the body, empty left hand tucked near the chest for balance.
Exactly ONE practical one-handed flanged steel mace in the right hand: one compact head, one short plausible haft, one hand on the grip. The mace head is visibly connected to the haft and exactly tangent to the one trail. No second weapon, shield, sword, axe, warhammer, staff, or detached head.

EVIDENCE AND SKYRIM VISUAL LANGUAGE:
The verified 1.6667-second 1H clip advances roughly 390 units, makes three equally damaging weapon contacts, triples trail lifetime and brightness on each, then applies unresolved knockback on the final hit. Distill the sequence to one defining final upswing with one intense trail, not three attacks or three trails. The ability name and payload prove no heart image, blood, element, or magic.
Breton identity through economical full-body silhouette/material cues only: small closed bascinet-like helm, simplified mail-and-plate masses, royal-blue cloth, burgundy accent, charcoal leather, restrained silver and muted-gold edges. Face fully hidden; no eyes, hair, skin close-up, portrait, or costume showcase.

STYLE AND COLOR:
Bold painterly graphical MMO ability icon optimized for 32x32 readability; compact graphic masses, strong negative space, high contrast, restrained texture. One luminous silver-blue physical crescent against a quiet burgundy-charcoal field; tiny source accents in royal blue, muted gold, and dark steel. Pale-gold physical sparks only at contact. No magical aura, fire, frost, lightning, poison, or rune. Dark vignette, safe crop, edge-to-edge art.

HARD CONSTRAINTS:
exactly one tiny faceless full-body Breton, exactly one one-handed mace, exactly one connected rising crescent, one contact. No portrait, face, large character, second trail, repeated mace, heart symbol, blood, gore, victim, spell, scenery, text, logo, watermark, border, or UI frame.
```

## Hard-gate failure

- Full-resolution result: fail. The mace head and partial haft float at the upper-left end of the
  trail while the Breton's extended hands hold no weapon. The trail is therefore disconnected from
  both figure and grip despite otherwise correct single-trail hierarchy.
- 32 px reduction: the crescent remains readable, but small-size clarity does not waive the
  detached weapon and broken causality.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_31_heart_strike_hard_failure.png` and
  `aow_31_heart_strike_hard_failure_32.png`.
- Failure image SHA-256: `DC114A71374EFA8F4D6269C4B73379C4FADD60BFAFFAE6EED6B5EC46DD02CDF6`.
- Per the one-shot goal, no regeneration was attempted and no master or atlas input was created.

## Completion regeneration

**Generation path:** New Codex built-in generation from scratch. The five approved keeper icons
were grammar-only references; the failed Heart Strike plate was excluded. Only the 128 px atlas
input is retained in the project.

```text
Use case: stylized-concept
Asset type: one original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, optimized for crisp 32x32 readability.
Primary request: Heart Strike, a driving one-handed mace upswing. The title does not authorize hearts or blood.
Reference roles: all five supplied images are approved grammar-only references for faceless near-black abstraction, limited palette, compact glow, painterly massing, and hotbar readability. Do not copy their pose, figure, weapon, facing, effect, palette, or composition. Do not use any prior Heart Strike image.
Animation evidence: a 1.6667-second 1H clip advances about 390 units and makes three equal weapon contacts; each triples trail lifetime and brightness, and the terminal hit carries unresolved knockback. No heart, blood, element, or exact VFX is proven.
Orientation: tight rising diagonal from lower-right toward upper-left, framed as a compact cross-body release.
Frozen action: one Breton fighter occupies about 32-36% at lower-right, crouched with torso rotating left, right arm driving a mace upward across the body, empty left hand tucked close. Exactly one practical one-handed flanged mace: compact connected steel head, one short continuous haft, one closed right-hand mass on its grip. Keep figure, hand, haft, head, and trail in one visibly uninterrupted causal chain.
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: closed bascinet mass, compact knight proportion, one
burgundy wedge. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is
the small anonymous verb; the effect and weapon path are the subject.
Causal effect: exactly one open silver-blue physical crescent begins behind the moving mace head and tapers toward lower-right; one ivory pressure nick and sparse pale-gold steel sparks sit at the upper-left leading contact. Distill three contacts into one persistent final upswing.
Palette: burgundy-charcoal atmospheric field, royal-blue wedge, dark steel, silver-blue wake, muted gold sparks. Abstract field only.
Hard constraints: one silhouette; exactly one connected held mace; exactly one head-connected rising trail; no floating mace, detached head, missing grip, second weapon, heart symbol, blood, second trail, ring, X, victim, magic, scenery, horizon, portrait, costume plate, text, logo, watermark, border, or UI frame.
```

### Final hard-gate result

- 128 px: pass. The held mace is continuous from hand through haft to head, and the single rising
  crescent begins behind that head.
- 32 px: pass. Mace, source figure, rising arc, and terminal contact remain legible.
- Atlas-input SHA-256: `D0A446843F93405388DC278A5716BEC2A108035CF1B234419CD29048D327CE98`.
