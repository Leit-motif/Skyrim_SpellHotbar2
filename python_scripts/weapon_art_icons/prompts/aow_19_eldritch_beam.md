# Eldritch Beam

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_19_eldritch_beam`

## Evidence and interpretation

- The active `SpellHotbar2Arts/Eldritch Beam/config.json` redirects through
  `overrideAnimationsFolder` to
  `Nolvus Ashes of War Stance Framework/Eldritch Beam` in the selected `Nolvus Awakening`
  profile.
- The enabled `Ashes of War - Weapon Art Via Additional Attack` and
  `Animations - Mercenary Greatsword` providers contain byte-identical
  `AABL_Attack_A.hkx` files. The selected provider path is
  `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Ashes of War - Weapon Art Via Additional Attack\meshes\actors\character\animations\OpenAnimationReplacer\Nolvus Ashes of War Stance Framework\Eldritch Beam\AABL_Attack_A.hkx`;
  SHA-256 `C4B37ED7D0883FA3F69D1FA795584F7E0D03910B2D74659A9C1EDC0851BB909A`.
- Animation-proven: the 2.833333-second clip begins a right-magic-node collision and stages
  `$EBeffect1`, `$EBeffect2`, and `$EBeffect3` at 0.0, 0.666640, and 1.166620 seconds. At
  1.666600 seconds it fires `$EBpro1` with a strong camera shake. The rig contains 97 transform
  tracks, so its precise limb pose was not claimed from the available 99-bone inspection skeleton.
- Payload-proven: `EldritchBlast.ini` resolves `$EBpro1` to spells `000807` and `000820` in
  `Eldritch Blast.esp`. The live record chain resolves a 2000-unit `Beam` projectile with green
  absorb-projectile light, impact force, laser/flare/cloud/strip geometry, endpoint sparkles, and
  a compact explosion. A secondary cone and explosion reuse fire records.
- Skyrim-reference-derived: a neutral weathered battlemage uses charcoal wool, worn iron, and
  leather rather than an invented racial or faction costume.
- Agent composition choice: one braced right-palm cast travels horizontally left-to-right. The
  beam is white-green and emerald from its explicit green light; the warm color is confined to a
  small terminal fringe supported by the secondary fire records.
- Atlas choice: the horizontal axis follows crossed Dual Flurry and vertical Earth Shatter and
  does not repeat their dominant composition.

## Reference roles

- No image reference was supplied to the generator.
- Local HKX, Payload Interpreter configuration, live plugin records, and winning NIF strings were
  semantic evidence only and were not image inputs.
- Approved prior icons informed only the recorded MMO hotbar grammar and 32 px acceptance standard;
  their figures, poses, palettes, effects, and compositions were not copied.

## Final generation prompt

```text
Use case: stylized-concept
Asset type: original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, designed first for crisp 32x32 readability
Primary request: Eldritch Beam — one concentrated supernatural beam erupting from a caster's right palm.
Payload evidence: three staged self effects build at 0.0, 0.6666, and 1.1666 seconds; at 1.6666 seconds a true 2000-unit beam projectile fires with strong camera shake. The beam record carries a green absorb-projectile light, laser/flare/cloud/strip geometry, endpoint sparkles, and a compact impact; a secondary short cone and explosion reuse fire records. The source node is NPC R MagicNode.
Exact action and camera: clean three-quarter side view, horizontal left-to-right action axis. Place a small generalized faceless Skyrim battlemage at the left third, torso recoiling and knees braced against the discharge, right shoulder and elbow driving one fully extended open palm toward the right. The beam begins visibly inside that palm and shoots to a bright endpoint near the right edge. The left arm stays tucked back for balance. No weapon.
Icon hierarchy: the beam and its endpoint occupy about 75 percent of the square; the caster is no more than 25 percent and exists only to explain the source. Preserve one bold line and one compact endpoint at 32 px.
Skyrim identity: neutral weathered battlemage rather than a named race or faction; dark charcoal wool robes under a few practical worn iron and leather guards, simple hood casting the face into shadow, no ornate high-fantasy costume.
Effect geometry and palette: one straight white-green core with a thick sickly emerald sheath and a short turbulent smoky wake that tightens toward the palm. Three restrained crescent-like charging wisps hug the forearm and shoulder without becoming rings. At the far-right endpoint, one compact green-white starless impact with a few sparks; a very small ember-orange fringe may appear only at that terminal flare to reflect the secondary fire record. The beam remains continuous, directional, and physically sourced from the palm.
Background and finish: deep aubergine-charcoal smoky field, painted graphical MMO ability-icon finish, bold value separation, restrained texture, no scenery.
Constraints: exactly one small faceless caster, one right hand as the source, one continuous beam, one endpoint. No weapon, second figure, victim, portrait, character key art, floating central orb, detached halo, black-hole ring, lightning branches, blue frost, decorative runes, readable symbol, text, logo, watermark, or border.
```

## Final hard-gate result

- Full-resolution result: pass. One coherent braced caster, one right-palm source, one continuous
  beam, and a compact terminal flare; no weapon, detached central effect, or portrait framing.
- 32 px LANCZOS reduction: pass. The caster, palm source, horizontal white-green core, and endpoint
  remain immediately readable as one hotbar action.
- Master SHA-256: `7B81033957B5A2F45A9304152680CE9BE5E0243A30A18FB95E95825F81B38FB6`.

