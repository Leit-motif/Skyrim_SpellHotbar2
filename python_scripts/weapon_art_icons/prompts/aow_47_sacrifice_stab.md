# Sacrifice Stab

**Status:** One-shot hard failure recorded on 2026-08-24

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
