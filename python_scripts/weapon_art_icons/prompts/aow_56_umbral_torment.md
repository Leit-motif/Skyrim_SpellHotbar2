# Umbral Torment

**Status:** Owner-directed purple palette edit finalized at 128 px on 2026-08-25

**Stable icon key:** `aow_56_umbral_torment`

## Evidence and interpretation

- Selected provider: `Ashes of War - Weapon Art Via Additional Attack/.../Umbral Torment/AABL_Attack_A.HKX`; SHA-256 `DB224AD82769236531CA87E398E682A84019D033EAAA1E598F9F71660B784DDA`.
- Animation-proven: the 3.0-second, 99-track 2H clip advances about 185 units and makes one hit; collision scale is 1.9, damage multiplier 3, and trail color is explicitly overridden to `(1, 0.35, 0.1, 1)`.
- Power-charge, paralysis, and hit-fog names are unresolved; no shadow, torment aura, darkness, or fog is supported.
- Composition choice: one Dunmer Redoran greatsword strike with one payload-proven ember-orange physical wake.

## Generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for an Elder Scrolls two-handed weapon art named "Umbral Torment". This is an effect-first action glyph, not a portrait, key art, wallpaper, or scene.

Make exactly ONE hot ember-orange physical weapon wake dominant: a thick compact diagonal arc upper-left to lower-right, brightest behind one greatsword edge, tapering backward, ending at blade tip in one orange-white impact. Color is explicit payload evidence, RGB approximately 1.0, 0.35, 0.1. Not fire magic, black shadow, multiple trails, ring, X, sphere, or detached crescent.

Show exactly one small faceless full-body Dunmer Redoran warrior completing one heavy two-handed diagonal strike. Front foot planted, rear leg braced, knees bent, hips and shoulders rotated. Both hands close around one continuous long hilt with spacing, lead behind guard and rear near pommel; wrists neutral, elbows bent. Hide face inside closed Redoran bonemold helm.

Exactly ONE connected two-handed Dunmer greatsword: broad dark ebony-steel blade with straight double edge, reddish-bronze fuller, compact chitin/bonemold guard, long ash-gray grip, one pommel. Both hands hold it. No second weapon, floating blade, detached hilt, axe, scythe, or one-handed grip. Lower edge leads orange wake.

The verified 3.0-second, 99-track 2H clip advances about 185 units and makes one hit. Collision scale is 1.9, damage 3, and trail color hot orange-red. Other names unresolved. Depict one enlarged high-force physical strike and proven orange trail; no shadow magic, torment aura, darkness, fog, or paralysis color.

Use Redoran construction: reddish-brown bonemold, dark mail, ash-gray wraps, brick-red sash, chitin shoulder, dark boots, bronze fittings. Palette: bonemold brown, ash gray, ebony, brick red, bronze, one hot ember-orange wake against cool slate-blue and clay. Crisp painterly graphical MMO hotbar icon optimized for 32x32.

HARD CONSTRAINTS: exactly one faceless Dunmer Redoran warrior, exactly one connected two-handed greatsword, exactly one blade-connected orange wake. Anatomically possible stance, grip, joints, leverage, balance, edge direction, and trail only. No portrait, second weapon, floating blade, detached hilt, one-handed grip, extra arm, impossible wrist, hand on blade, multiple trails, shadow, darkness, black aura, fog, paralysis glow, literal flames, blood, victim, scenery, text, logo, watermark, border, or UI frame.
```

## Hard-gate result

- Physics/anatomy: pass. Both hands, continuous hilt, shoulders, legs, sword edge, and one orange wake form a coherent heavy strike.
- Framing/effect: hard failure. Literal mountains, battlefield ground, and extensive fiery spectacle make scenic key art and read as fire magic despite the brief limiting the orange to a physical trail.
- 32 px remains legible but cannot rescue the prohibited framing/effect invention.
- Failure SHA-256: `DC46ABBB748F64A37FD982E2EEB9DF57C3CE8E221EE19EA407D7E5190FAE289C`.
- Evidence is preserved under ignored `pilot/`; no master or atlas input exists and no retry was attempted.

## Completion regeneration

**Generation path:** New Codex built-in generation from scratch. The five approved keeper icons
were grammar-only references; the failed Umbral Torment plate was excluded. Only the 128 px atlas
input is retained in the project.

```text
Use case: stylized-concept
Asset type: one original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, optimized for crisp 32x32 readability.
Primary request: Umbral Torment, one heavy Dunmer two-handed greatsword strike with a payload-proven ember-orange physical wake. Orange is trail color, not fire magic.
Reference roles: all five supplied images are approved grammar-only references for faceless near-black abstraction, limited palette, compact glow, painterly massing, and hotbar readability. Do not copy their pose, figure, weapon, facing, effect, palette, or composition. Do not use any prior Umbral Torment image.
Animation evidence: a 3.0-second 2H clip advances about 185 units and makes one hit; collision scale 1.9, damage multiplier 3, and trail color explicitly RGB about 1.0, 0.35, 0.1. Other payload names are unresolved; no shadow, darkness, fog, paralysis, or flame is supported.
Orientation: compact heavy diagonal from upper-left toward lower-right, seen from a tight side-on three-quarter crop.
Frozen action: one Dunmer Redoran warrior occupies 33-38% at upper-left/center, completing a braced two-handed descending strike. Both connected hand masses grip one continuous long hilt with spacing; wrists neutral, elbows bent, shoulders and torso rotate into the edge. Exactly one broad practical two-handed greatsword, one continuous straight blade and grip, visibly connected to both hands. Lower cutting edge leads.
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: closed Redoran helm mass, broad chitin shoulder wedge,
long two-hand grip. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure
is the small anonymous verb; the effect and weapon path are the subject.
Causal effect: exactly one thick compact ember-orange physical wake begins behind the moving lower edge and tapers backward; it ends at the blade tip in one orange-white pressure impact. The orange is smooth motion paint, not tongues of flame, sparks may be sparse.
Palette: cool slate-blue and clay abstract field, bonemold brown, ash gray, ebony, brick-red wedge, bronze, one hot ember-orange wake.
Hard constraints: one silhouette; exactly one connected two-handed greatsword; exactly one edge-connected orange wake; no mountains, battlefield, ground plane, horizon, panorama, flying boulders, literal fire, flames, smoke plume, shadow magic, darkness aura, fog, second weapon, detached blade, one-handed grip, multiple trails, portrait, costume plate, text, logo, watermark, border, or UI frame.
```

### Final hard-gate result

- 128 px: pass. Both hands connect to one greatsword, the lower edge owns one orange physical wake,
  and the field contains no scenic battlefield or literal flame spectacle.
- 32 px: pass. The heavy diagonal greatsword strike and orange trail remain distinct.
- Atlas-input SHA-256: `C53F69543142386205AB5BDF19DD6BF8072A0187E6414ACAC61F9C240FADBF74`.

## Owner-directed purple palette edit

Owner direction supersedes the animation-proven orange trail color for the atlas presentation.
The successful 128 px icon was the sole edit target; greatsword construction, grip, pose, crop,
impact, and single-wake geometry were locked.

```text
Use case: precise-object-edit
Asset type: square MMO/RPG hotbar ability icon, optimized for 32x32 readability.
Input image: sole edit target.
Primary request: recolor Umbral Torment purple.
Change only color and glow: replace the orange greatsword wake and impact with a saturated aubergine-to-violet physical wake, a narrow electric-magenta inner edge, and a pale lavender-white impact core. Shift orange edge lighting on the figure and sword to restrained violet. Change the abstract field to charcoal, deep aubergine, muted plum, and cool slate so the purple action remains the first read.
Preserve exactly: square composition, crop, near-black Redoran silhouette, helmet and shoulder outline, both connected hand masses, one continuous greatsword and grip, blade angle, one edge-connected wake, impact point, brushwork, and painterly MMO-icon finish.
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: closed Redoran helm mass, broad chitin shoulder wedge,
long two-hand grip. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure
is the small anonymous verb; the effect and weapon path are the subject.
Constraints: exactly one connected two-handed greatsword and one purple wake. No orange, literal flames, lightning branches, fog, extra trail, detached blade, new objects, scenery, text, logo, watermark, border, or UI frame. This is a narrow palette edit, not a redesign.
```

- 128 px: pass. The connected two-handed greatsword and one wake remain unchanged; aubergine,
  violet, magenta, and lavender now supply the requested umbral-purple read.
- 32 px: pass. Purple diagonal strike remains legible without orange or scenic drift.
- Atlas-input SHA-256: `D6A7286EA2D64B49704D8E7F129E83444DE37DC531ADFAD527ED20996C16355D`.
