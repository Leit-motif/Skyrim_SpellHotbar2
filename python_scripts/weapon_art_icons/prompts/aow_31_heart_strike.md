# Heart Strike

**Status:** Owner-directed red Molag Bal/Daedric edit finalized at 128 px on 2026-08-25

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

## Owner-directed red palette edit

Owner direction supersedes the prior silver-blue composition choice. The successful 128 px icon
was used as the sole edit target; geometry, pose, weapon connection, crop, and single-wake hierarchy
were locked.

```text
Use case: precise-object-edit
Asset type: square MMO/RPG hotbar ability icon, optimized for 32x32 readability.
Input image: sole edit target.
Primary request: recolor Heart Strike so the ability is unmistakably red-coded.
Change only color and glow: replace the blue-white mace crescent with a saturated crimson and claret physical wake, with a narrow red-white core and a compact white-red impact flare. Shift any blue rim light on the figure to restrained dark crimson. Keep the burgundy-charcoal field dark enough for separation.
Preserve exactly: square composition, crop, figure silhouette, helmet, arm pose, connected hand/haft/mace geometry, mace head shape, single open rising crescent, spark placement, vignette, painterly MMO-icon finish, and all object positions.
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: closed bascinet mass, compact knight proportion, one
burgundy wedge. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is
the small anonymous verb; the effect and weapon path are the subject.
Constraints: one held mace, one connected red crescent, one impact. No heart symbol, blood droplets, gore, fire, extra trail, detached weapon, new objects, scenery, text, logo, watermark, border, or UI frame. This is a narrow recolor, not a redesign.
```

- 128 px: pass. The connected mace and single crescent are unchanged; crimson, claret, and the
  red-white contact now make the requested red code immediate.
- 32 px: pass. Red upswing and source silhouette remain distinct.
- Atlas-input SHA-256: `A78FFD8FAD13CB55E7B99BC3B4F1C7A6D96187FE7878260A7048C8D357BD898E`.

## Owner-directed Skyrim artifact and armor edit

Owner direction: replace the generic mace with Skyrim's Mace of Molag Bal and the knight mass with
Skyrim Daedric armor while keeping the red Heart Strike treatment. Visual research used the UESP
Skyrim item render for the mace and the male Daedric armor screenshot, plus Bethesda-certified
Daedric armor concept-art provenance. These references supplied item silhouettes only.

- Mace reference: `https://images.uesp.net/3/34/SR-item-Mace_of_Molag_Bal.jpg`
- Armor reference: `https://images.uesp.net/thumb/e/ef/SR-item-Daedric_Armor_Male.jpg/600px-SR-item-Daedric_Armor_Male.jpg`
- Official concept-art provenance: `https://www.cookandbecker.com/en/artwork/2300/daedric-armor-skyrim-bethesda-softworks.html`

The first edit was rejected because the detailed armored figure expanded into costume-plate framing.
The successful pass restarted from the red source and locked the original compact envelope.

```text
Use case: precise-object-edit
Asset type: square MMO/RPG hotbar ability icon for Skyrim Spell Hotbar 2, optimized for 32x32 readability.
Input images: Image 1 is the sole edit target and fixes the exact composition, figure bounding box, action, crop, red crescent, and painterly abstraction. Image 2 supplies only Skyrim Mace of Molag Bal weapon silhouette. Image 3 supplies only Skyrim Daedric armor outer silhouette. Never copy reference backgrounds or static poses.

Primary request: make only two identity substitutions inside Image 1's existing occupied pixels: replace the generic mace with Skyrim's Mace of Molag Bal and replace the bascinet/knight outline with Skyrim Daedric armor. Do not enlarge, reposition, or lengthen the figure, arm, weapon reach, or occupied silhouette envelope.

Weapon: one short continuous one-handed haft remains in the exact existing hand and angle. At its existing upper-left endpoint, use a compressed but recognizable Mace of Molag Bal head: tall blackened cross-shaped Daedric mass, one crown spike, two broad angular side points, small horned-face base, sparse narrow sickly-green channels. Keep the entire mace within the original mace-and-arm envelope and tangent to the existing red impact. It is one connected one-handed mace—not a sword, axe, spear, or staff.

Armor: keep the body strictly within Image 1's original compact lower-right silhouette, occupying 32-38% of the square. Encode Daedric armor only through outer contour: small closed twin-horn helm mass, two jagged shoulder wedges, one compact angular torso wedge. Interior stays almost solid black with only restrained thin dark-red edge seams. No readable armor inventory, layered plate showcase, chest ornament, gauntlet detail, or added spikes outside the original figure boundary.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: small twin-horn Daedric helm mass, two jagged shoulder
wedges, compact one-handed striking proportion. No fur, hair, eyes, cloth folds, rivets, filigree,
or armor inventory. The figure is the small anonymous verb; the effect and weapon path are the
subject.

Preserve exactly from Image 1: lower-right-to-upper-left upswing, one open crimson/claret crescent with red-white core, white-red contact, dark burgundy-charcoal field, negative space, vignette, safe crop, and painterly brushwork. Effect remains the dominant first read.

Hard constraints: compact figure no more than 38%; exact original figure envelope; one connected Mace of Molag Bal; one red crescent; no large armored character, costume plate, shoulder filling the right edge, extra weapon, shield, detached head, two-handed haft, green trail, heart, blood, fire, scenery, text, logo, watermark, border, or UI frame.
```

- 128 px: pass. The tall cross/crown mace head, green channels, twin-horn helm, and jagged Daedric
  shoulder outline survive while the red crescent remains the first read.
- 32 px: pass. Mace silhouette, red upswing, and compact Daedric source remain separable.
- Atlas-input SHA-256: `3DDC2A0008203315137DEB17826F48FCAAFDA702B99AA0559500012454CC7FCF`.
