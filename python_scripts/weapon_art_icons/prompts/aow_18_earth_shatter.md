# Earth Shatter

**Status:** Finalized by goal-mode hard gate on 2026-08-23

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_18_earth_shatter`

## Evidence and interpretation

- The active `SpellHotbar2Arts/Earth Shatter/config.json` redirects through
  `overrideAnimationsFolder` to
  `Nolvus Ashes of War Stance Framework/Earth Shatter` in the selected `Nolvus Awakening`
  profile.
- The enabled `Ashes of War - Weapon Art Via Additional Attack` and
  `Animations - Mercenary Greatsword` providers contain byte-identical
  `AABL_Attack_A.hkx` files. The selected provider path is
  `C:\Nolvus\Instances\Nolvus Awakening\MODS\mods\Ashes of War - Weapon Art Via Additional Attack\meshes\actors\character\animations\OpenAnimationReplacer\Nolvus Ashes of War Stance Framework\Earth Shatter\AABL_Attack_A.hkx`;
  SHA-256 `B8ED9613CC4D5ED12B3DD21DC93821C681348B320FA280AC950F040106922FF7`.
- Animation-proven: the 2.533333-second, 76-frame clip advances about 101 units, raises both arms
  into a high wind-up, steps into a wide forward brace, emits `weaponswing` at 0.85 seconds, and
  lands one `HitFrame` at 0.916666 seconds. There is no named elemental or spell payload.
- Agent composition choice: distill the generic weapon-compatible move into one practical
  two-handed warhammer and an Orsimer shock trooper. The physical overhead slam is preserved;
  weapon family and archetype are not claimed as animation-proven.
- Skyrim-reference-derived: weathered angular iron plates, worn leather and cloth, and a compact
  crushing head use the project's Orsimer and warhammer vocabulary.
- Agent composition choice: pale compression light, ochre-gray dust, and stone fragments express
  nonmagical impact without inferring an earth element from the ability name.
- Atlas choice: a near-vertical descending axis follows right-to-left Double Slash, frontal Dragon
  Strike, and crossed Dual Flurry without repeating their dominant compositions.

## Reference roles

- No image reference was supplied to the generator. Offline stick-figure renders of the verified
  HKX were evidence for body kinematics only and were not image-generation inputs.
- Approved prior icons informed only the recorded MMO hotbar grammar and 32 px acceptance standard;
  their figures, poses, palettes, effects, and compositions were not copied.

## Final generation prompt

```text
Use case: stylized-concept
Asset type: original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, designed first for crisp 32x32 readability
Primary request: Earth Shatter — one brutal overhead warhammer strike at the exact instant its head hits the ground.
Animation evidence: a 2.533-second clip advances about 101 units, raises both hands high, then drives them down while the fighter steps into a wide forward brace; one weapon swing lands one hit at 0.9167 seconds. No magical payload is proven.
Exact action and camera: three-quarter side view with a strong near-vertical top-to-bottom action axis. Freeze the impact: a compact faceless Orsimer shock-trooper silhouette has the rear knee bent, front leg planted wide, torso compressed forward, both hands gripping one long warhammer haft, and the hammer head contacting the lower-center ground. Shoulders, elbows, hands, haft, hammer head, and impact point must form one continuous descending force line.
Icon hierarchy: the hammer head and compact ground-impact burst are the primary read, about 65 percent of the glyph; the fighter is subordinate but fully explains the strike. Keep the whole action safely inside the square.
Skyrim identity: broad Orsimer shock trooper in weathered angular iron plates over dark worn leather and rough cloth, simplified into bold masses; one practical two-handed warhammer with a long wooden haft and compact iron crushing head, plausible Skyrim proportions.
Effect and causality: strictly nonmagical physical force. The hammer head is embedded at the source of one tight steel-white compression flare; ochre-gray dust and a few large stone shards fan outward along the ground from that exact contact point. No floating ring, no disconnected explosion, no elemental glow. The brightest value sits at the hammer-ground contact and fades outward.
Palette: grounded iron gray, charcoal, worn brown, and a restrained rust accent; pale steel-white impact and ochre-gray dust against a deep muted blue-black smoky field for atlas variety.
Style/medium: painted graphical MMO ability icon, chunky high-contrast silhouette, restrained texture, atmospheric edge glow, emblematic and action-first rather than character key art.
Constraints: exactly one faceless fighter, exactly one two-handed warhammer, two hands on the same haft, one ground impact physically caused by the hammer. No portrait, static pose, decorative broadside weapon, sword, axe, shield, second weapon, victim, fire, frost, lightning, green magic, magic rune, floating halo, detached ring, scenery, text, logo, watermark, or border.
```

## Final hard-gate result

- Full-resolution result: pass. One coherent fighter, one correctly gripped two-handed warhammer,
  plausible anatomy, and one impact sourced at the hammer-ground contact.
- 32 px LANCZOS reduction: pass. The central descending hammer, bright contact point, braced figure,
  and outward debris remain a single readable hotbar action.
- Master SHA-256: `D3ED07691FF300B33D98070FCDEB2F7314D9D665FDE26ABBAE41466EAA86559D`.
