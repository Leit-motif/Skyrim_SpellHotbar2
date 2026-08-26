# Sacrifice Stab

**Status:** Autonomous from-scratch completion regeneration passed at 128 px on 2026-08-24

**Stable icon key:** `aow_47_sacrifice_stab`

## Evidence and interpretation

- Selected provider: `Ashes of War - Weapon Art Via Additional Attack/.../Sacrifice Stab/AABL_Attack_A.HKX`; SHA-256 `701CBC01DBA04602E0FBD57FFEB1EA43460C9E39831E53297BC1BB6E46FAC117`.
- Animation-proven: the 1.833333-second, 97-track 1H clip lunges about 353 units and makes one hit at 0.899999; the repulse name is unresolved.
- No self-harm, blood, victim, altar, or ritual magic is supported.
- Composition choice: a Bosmer hunter physically overcommitting to one long-dagger thrust.

## Generation prompt

```text
Create ONE square 1:1 raster image: an original polished MMO/RPG hotbar ABILITY ICON for an Elder Scrolls one-handed weapon art named "Sacrifice Stab". This is an action glyph, not a portrait, character key art, wallpaper, or scene.

Show exactly one small faceless full-body Bosmer hunter making one dangerously committed straight dagger lunge from upper-right toward lower-left. The "sacrifice" is physical overcommitment, not self-harm: front knee deeply bent above a planted foot, rear leg stretched nearly straight, hips thrust forward, torso and right shoulder extending behind one stabbing arm, left arm backward to counterbalance. Right fist closes around one dagger grip; wrist neutral, elbow slightly bent, shoulder aligned behind point. Hide the face under a bark-brown hood and bone brow-mask.

Exactly ONE connected one-handed Bosmer hunting dagger: one long narrow dark-flint blade with reinforced pale-bone spine, simple antler crossguard, moss-wrapped grip, and small bone pommel. It is a practical long dagger, not a sword, spear, claw, or projectile.

Add exactly ONE narrow pale-amber pressure wake collinear behind the dagger point and forearm, beginning near the shoulder and ending at the point in one compact ivory spark. No cone, fan, slash arc, ring, or second weapon.

The verified 1.833333-second, 97-track 1H clip lunges about 353 units and makes one hit at 0.899999. The unresolved repulse name proves no force magic. Depict one risky full-commit stab only; the name proves no blood, self-sacrifice, altar, victim, death, or ritual.

Use Elder Scrolls Bosmer hunter construction: fitted moss-green leather and bark-brown scale, tan hide wraps, small pale-bone pieces, muted amber sash, soft dark boots. Palette: moss, bark brown, tan, bone ivory, muted amber, one pale-amber wake against dusky blue-green and brown. Crisp painterly graphical MMO hotbar icon optimized for 32x32.

HARD CONSTRAINTS: exactly one faceless Bosmer, exactly one connected held dagger, exactly one point-aligned wake. Anatomically possible lunge, grip, joints, balance, and path only. No portrait, second weapon, floating dagger, detached blade, extra arm, impossible wrist, hand on blade, blade entering self, multiple trails, blood, wound, altar, victim, corpse, force magic, element, scenery, text, logo, watermark, border, or UI frame.
```

## Hard-gate result

- Physics/anatomy: pass. The committed lunge, support, arm line, closed grip, and collinear wake are coherent.
- Weapon construction: hard failure. The requested forearm-scale long hunting dagger became an unmistakable full sword with a blade well over twice forearm length and an oversized ornate guard.
- 32 px result reads as a generic sword thrust, not the distinct dagger action requested.
- Failure SHA-256: `ED25316F88F82C5881A23352ED87E60D9EF63A462E64D385E7D3EF5490A6B123`.
- Evidence is preserved under ignored `pilot/`; no master or atlas input exists and no retry was attempted.

## Completion regeneration

**Generation path:** New Codex built-in generation from scratch. The five approved keeper icons
were grammar-only references; the failed Sacrifice Stab plate was excluded. Only the 128 px atlas
input is retained in the project.

```text
Use case: stylized-concept
Asset type: one original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, optimized for crisp 32x32 readability.
Primary request: Sacrifice Stab, a physically overcommitted Bosmer long-dagger lunge. “Sacrifice” means risky body commitment, never self-harm.
Reference roles: all five supplied images are approved grammar-only references for faceless near-black abstraction, limited palette, compact glow, painterly massing, and hotbar readability. Do not copy their pose, figure, weapon, facing, effect, palette, or composition. Do not use any prior Sacrifice Stab image.
Animation evidence: a 1.8333-second 1H clip lunges about 353 units and makes one hit at 0.9 seconds. The repulse name is unresolved. No self-harm, blood, victim, altar, or ritual magic is supported.
Orientation: strong straight thrust from upper-right toward lower-left, using mild frontal foreshortening.
Frozen action: one Bosmer hunter occupies 34-38% at upper-right, lunging down-left with torso and right shoulder extended behind the stabbing arm; the empty left arm counters backward. The right fist mass closes around exactly one short dagger grip. Shoulder, elbow, wrist, grip, narrow blade, wake, and point align in one uninterrupted thrust.
FIGURE MASS: one solid near-black silhouette, 25-45% of the square, cropped to torso, weapon, and
path. Face is a void. Race as outline only: small hooded hunter mass, light narrow weapon, one
bone-brow wedge. No fur, hair, eyes, cloth folds, rivets, filigree, or armor inventory. The figure is
the small anonymous verb; the effect and weapon path are the subject.
Weapon scale: exactly one practical Bosmer long dagger. The blade is narrow and only about forearm length from wrist to elbow, with a simple tiny antler crossguard, short moss-wrapped grip, and small pommel. It must unmistakably read as a dagger, not a sword.
Causal effect: exactly one narrow pale-amber pressure wake runs collinear behind the dagger point and forearm, ending at the point in one compact ivory spark. No slash arc or cone.
Palette: moss, bark brown, dusky blue-green, bone ivory, muted amber, one pale-amber wake. Abstract dark field only.
Hard constraints: one silhouette; exactly one connected forearm-length dagger; exactly one point-aligned wake; no sword-length blade, oversized guard, second weapon, floating blade, self-stab, blood, wound, altar, victim, corpse, magic, scenery, horizon, portrait, costume plate, text, logo, watermark, border, or UI frame.
```

### Final hard-gate result

- 128 px: pass. The connected narrow blade is forearm-scale and reads as a dagger rather than the
  full sword from the rejected plate.
- 32 px: pass. The risky lunge, dagger point, and collinear wake remain one clear glyph.
- Atlas-input SHA-256: `C417B0C83938AB1C5E9EE2D2CC20F6661F8B4F5F4879D81B1593F0CD9C47D795`.
