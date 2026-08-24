# Flurry Strike

**Status:** Hard failure after the one permitted generation attempt on 2026-08-23; not finalized

**Generation path:** One Codex built-in image-generation call; no image references

**Stable icon key:** `aow_23_flurry_strike`

## Evidence and interpretation

- The active definition redirects to `Nolvus Ashes of War Stance Framework/Flurry Strike`.
  Selected enabled provider: `Ashes of War - Weapon Art Via Additional Attack/.../Flurry Strike/AABL_Attack_A.hkx`;
  SHA-256 `A0CFD01BB3D70EFCE83EED22E93923F8D0D16F3F43E8DB0750B456434EE94DFA`.
- Animation-proven: the 4.666667-second, 97-track, 2H-only clip advances about 401 units and
  delivers six weapon hits. Every collision uses 1.5x length, 3x trail lifetime, and 3x trail
  brightness; the sixth hit is stronger and carries `$AoW_Knockback`.
- Agent composition choice: an Orsimer shock trooper with one practical two-handed battleaxe and
  three steel-white trails as economical shorthand for six hits. No element was inferred.
- Atlas intent: layered right-to-left trails were chosen after rising Elegant Slash, rotational
  Enrage (F), and upward-frontal Enrage (M).

## Final generation prompt

```text
Use case: stylized-concept
Asset type: original square MMO/RPG hotbar ability glyph for Skyrim Spell Hotbar 2, designed first as a crisp 32x32 MMO ability icon
Primary request: Flurry Strike — a rapid two-handed battleaxe sequence distilled into layered weapon trails and one heavy finisher.
Animation evidence: a 4.6667-second 2H-only clip advances about 401 units, performs six weapon swings and six hits, lengthens the weapon collision and triples trail lifetime and brightness on every swing, then strengthens the sixth hit and adds knockback. No elemental or colored payload is proven.
Icon hierarchy: the repeated axe paths and terminal impact are the icon and occupy about 85 percent of the square. One tiny faceless Orsimer shock-trooper silhouette occupies no more than 15 percent near the center-left, only large enough to connect hands, axe, and trails. This must be an MMO hotbar ability glyph, never a portrait, character key art, armor showcase, or scene.
Exact action and camera: low three-quarter side view with the overall attack traveling right-to-left across the square. Freeze the final beat: the tiny broad fighter leans into a planted forward step, both hands spaced on one long wooden haft, while exactly one practical two-handed battleaxe finishes low at the left edge with its broad cutting edge leading the final cut.
Effect geometry and causality: show three bold staggered steel-white axe trails as economical shorthand for the six hits: one high, one middle, and one low, all parallel enough to read as a rapid advancing sequence rather than a cyclone. Every trail curves back toward the same moving axe path, is brightest immediately behind the cutting edge, and tapers toward the right. The low final trail terminates in one compact rust-white impact burst at the axe head with a few physical sparks and dust fragments. No independent rings or elemental glow.
Skyrim identity: tiny Orsimer shock trooper in simplified angular weathered iron layers over dark green rough cloth and worn black-brown leather. The equipment is reduced to bold masses; no visible face, tusks, or armor detailing at icon scale. The battleaxe has one dominant heavy iron head, plausible Skyrim proportions, never a double-headed fantasy emblem.
Palette and background: nonmagical ivory steel-white trails, iron gray, dark green, black, rust, and a restrained blood-red cloth accent against a quiet smoky sand-charcoal field.
Style/medium: painted graphical MMO ability icon, bold simple masses, strong silhouette, safe crop, restrained texture, immediate 32 px readability.
Constraints: exactly one tiny faceless fighter, one two-handed battleaxe, two hands on one haft, three trail bands as sequence shorthand, one terminal impact. No large character, portrait, armor showcase, sword, hammer, second weapon, detached ring, cyclone, fire, frost, lightning, magic rune, gore, victim, scenery, text, logo, watermark, or border.
```

## Hard-gate failure

- Full-resolution result: fail. The model made the Orsimer a large character-art subject rather
  than a subordinate icon marker, and the axe became a double-bitted fantasy head instead of one
  dominant practical head.
- 32 px reduction: the three trails remain readable, but small-size legibility does not waive the
  incorrect weapon construction or key-art hierarchy.
- Preserved ignored evidence:
  `python_scripts/weapon_art_icons/pilot/aow_23_flurry_strike_hard_failure.png` and
  `aow_23_flurry_strike_hard_failure_32.png`.
- Failure image SHA-256: `197759912368FAEF9628EB7809B5334A221D6CAB6DF238EC853C3AB311D0CC22`.
- Per the one-shot goal, no regeneration was attempted and no master or atlas input was created.
