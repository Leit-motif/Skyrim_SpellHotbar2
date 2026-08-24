# Furrow Strike

**Status:** Owner-approved silhouette regeneration, finalized 2026-08-24

**Generation path:** Three new Codex built-in generations from the canonical brief. The first revision made the humanoid primary but cropped away the emanating furrow; the second overcorrected to a distant figure. The final owner-approved medium-crop revision was generated from a new prompt. No rejected Furrow Strike image was used as a reference or edit target. Crane Style, Aimed Blow, Blood Flurry, Champion's End, and Dragon Strike were attached as grammar-only references.

**Stable icon key:** `aow_27_furrow_strike`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Furrow Strike`. Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Furrow Strike/AABL_Attack_A.HKX`; SHA-256 `248B72C7256AC0D2F2F009050230ADE9E60C829E9893CA76C9C9F928B53A6839`.
- Animation-proven: the 3.000000-second, 99-track, 2H-class clip advances about 208 units. A weaker 0.5-damage weapon collision begins at 1.333333 seconds; the defining 1.5-damage collision begins at 1.750000, triples trail lifetime, doubles trail brightness, triggers `$AoW_Dust`, then lands at 1.833333 with `$AoW_Knockback` and camera shake.
- Payload resolution: no active definitions were found for `$AoW_Dust` or `$AoW_Knockback`. Physical dust, displaced soil, stone, and impact force follow the animation names and collision; no element or magical color was inferred.
- Owner corrections: the humanoid attacker must remain the focal subject; the camera must leave room for a furrow to emanate from the strike; use a medium crop between the close and wide revisions; preserve the earth-tone direction.
- Owner-approved composition: a medium-crop Nord silhouette occupies the upper-left/center. One connected greatsword terminates at a contact just right of center, and one medium furrow originates there and travels into the lower-right.

## Reference roles

- `aow_08_crane_style.png`, `aow_02_aimed_blow.png`, `aow_04_blood_flurry.png`, `aow_07_champions_end.png`, and `aow_16_dragon_strike.png`: approved icon grammar only—faceless mass, abstraction, limited palette, glow, and 32 px readability.
- Their poses, figures, weapons, facing directions, palettes, and compositions were explicitly excluded.
- All rejected Furrow Strike images were excluded from generation inputs.

## Final generation prompt

```text
Use case: stylized-concept
Asset type: original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, designed first for crisp 32x32 readability
Primary request: Furrow Strike — use a medium crop balancing the prior close figure and the prior wide figure. The humanoid attacker is the clear subject, and a medium-length earth furrow visibly emanates from the weapon's ground strike.
Reference roles: the five supplied approved icons are grammar-only references. Match their faceless near-black abstraction, limited palette, compact glow, painterly massing, and hotbar readability. Do not copy any reference pose, figure, weapon, facing direction, palette, or composition. Do not use any rejected Furrow Strike generation as a visual reference.

Exact frozen action and camera: medium-distance side-on three-quarter action view. Place one broad Nord warrior in the upper-left/center, occupying about 37-40% of the square—larger and more immediate than a distant full-body figure, but with enough lower-right space to show the result of the strike. The warrior drives left-to-right in a low two-handed greatsword follow-through: torso pitched forward, front knee deeply bent, rear leg braced near the crop, shoulders rotating through the cut. Both connected hand masses sit on one connected grip. The practical straight blade descends from the hands to one ground-contact point slightly right and below center.
Orientation and hierarchy: the humanoid silhouette reads first, the connected blade second, and the emanating furrow third. The complete causal chain is unmistakable: humanoid pose -> connected greatsword -> bright ground contact -> outward earth furrow.

FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: broad shoulders, compact nasal-helm mass, powerful
forward-driving proportion. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory.
The figure is the small anonymous verb; the effect and weapon path are the subject.
Owner hierarchy clarification: set the silhouette around 37-40%, so it remains the dominant identifiable subject while leaving a purposeful lower-right channel for the furrow.

Weapon and physics: exactly one plausible Skyrim-style two-handed steel greatsword, one continuous blade and grip, visibly connected to both hand masses. The cutting edge terminates at the ground-contact point. Pose, supported center of gravity, shoulder rotation, and blade angle prove force.

Causal furrow: one compact ivory-white compression flash and restrained rust-orange sparks at the blade-ground contact. Starting EXACTLY at that contact, one continuous MEDIUM-LENGTH physical furrow travels diagonally toward the lower-right edge. It occupies about 22-28% of the square—longer and clearer than a tiny scrape, shorter and less dominant than a foreground trench. Show dark umber displaced-earth banks, ochre dust, a narrow ivory-gray compression seam, and a few small stones. The furrow must emerge forward from the strike point with no gap; it is not behind the blade, not a detached beam, and not a separate magical trail.
Color palette: deep umber, bark brown, ochre dust, charcoal, weathered steel gray, small ivory compression core and seam, tiny rust-orange sparks, and at most one restrained dark-red wedge. No elemental color.
Style/medium: polished painterly graphical MMO ability icon; bold compact masses, strong negative space, controlled edge light, dark warm-earth atmospheric field, safe crop, no scenic horizon.
Constraints: one humanoid, one greatsword, one contact, one connected medium furrow. No distant tiny figure, no extreme close-up, no full-square trench, no portrait, no face, no costume plate, no second weapon, no victim, no magic, fire, frost, lightning, runes, text, logo, watermark, border, or UI frame.
```

## Final hard-gate result

- Full generated intermediate: pass. The medium-crop faceless attacker is the dominant identifiable mass; both hands connect to one greatsword and the blade terminates at one ground contact.
- 128 px atlas input: pass. Figure, blade, contact, and the medium outward furrow form one readable causal chain without returning to the original trench-dominant composition.
- 32 px reduction: pass. The humanoid silhouette remains the first read and the bright earth seam visibly continues from the sword contact.
- Only the owner-approved 128x128 atlas input is retained in the project. Generated intermediates remain outside the project workspace.
- Rejected prior master and 128 px revisions are archived under `python_scripts/weapon_art_icons/pilot/silhouette-drift/`.
- Atlas-input SHA-256: `972E86E430B2DD96D7F5673203240D1528016A0DFF9106D5129F537A89E284CC`.
