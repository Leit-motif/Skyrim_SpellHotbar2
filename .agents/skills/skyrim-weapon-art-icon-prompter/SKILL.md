---
name: skyrim-weapon-art-icon-prompter
description: Author or revise prompts for Skyrim Weapon Art MMO hotbar icons, especially when animation evidence, race or faction coding, clothing, weapon silhouettes, VFX geometry, or 32 px readability must align before image generation.
---

# Skyrim Weapon Art Icon Prompter

Convert an ability and its animation evidence into a Skyrim-grounded visual brief and then a clean
image-generation prompt. This skill ends at a prompt unless the user also asks to generate.

## Load the project context

Before authoring a prompt, read:

- `python_scripts/weapon_art_icons/skyrim-visual-language.md` for Skyrim terms, archetypes,
  materials, and drift checks;
- `python_scripts/weapon_art_icons/figure-guidance.md` and `color-guidance.md` when a race-coded
  figure is requested or materially useful;
- `python_scripts/weapon_art_icons/composition-guidance.md` and the last three finalized manifest
  rows when choosing the action axis; read those rows' prompt files for their recorded axes and
  inspect approved masters only when the prose does not establish composition;
- the relevant animation annotations and payload configuration when the ability is backed by HKX
  or PIE evidence.

Resolve the animation from the active Ability/OAR definition rather than guessing from a similarly
named folder. Trace `overrideAnimationsFolder` when present, inspect enabled providers or the VFS
winner, and record the chosen HKX path and hash. If the active provider cannot be proven, label the
evidence unverified rather than calling one copy canonical.

For a named `$Payload`, search the relevant installed mod and Payload Interpreter configuration
files, then the project sources. If its definition remains unavailable, record it as unresolved and
do not invent an element, spell, color, or VFX from the payload name.

If a named Skyrim item, armor, faction, culture, or effect is unfamiliar or visually ambiguous,
research that exact term against first-party game media, locally installed game assets, or a
reliable game reference before describing it. Do not substitute generic fantasy memory.

## Resolve the visual brief

Fill `python_scripts/weapon_art_icons/prompt-brief-template.md` conceptually before writing prose.
Resolve these fields in this priority order:

1. **Owner direction:** literal subject, action, clothing, weapons, effect color, and requested
   correction. These are hard constraints.
2. **Frozen action:** one readable instant from the move. Describe the body's center of gravity,
   limb paths, weapon endpoints, and direction of travel.
3. **Skyrim identity:** choose a specific archetype, not a race label alone. `Nord barbarian`,
   `Nord housecarl`, and `Nord plate warrior` are different briefs.
4. **Causality:** every dominant trail starts from a moving weapon or body part. The pose must
   create the effect; the effect cannot decorate a static stance.
5. **Icon hierarchy:** choose the smallest figure that still communicates the action. Preserve one
   dominant silhouette and 32 px readability.
6. **Atlas variety:** choose an orientation and palette that fit the move without silently copying
   recent icons.

Label the provenance of material decisions in the brief: `animation-proven`, `payload-proven`,
`owner-directed staging`, `Skyrim-reference-derived`, or `agent composition choice`. Owner
direction wins, but do not misreport owner staging as something the annotations prove.

When information remains underdetermined, prefer a neutral Skyrim figure over inventing a faction
or costume. Ask one narrow question only when camera, archetype, or action timing would materially
change the image.

## Write the generation prompt

Use natural, positive art direction in this order:

1. asset and 32 px use;
2. exact frozen action and camera;
3. body mechanics and silhouette;
4. Skyrim archetype, clothing materials, and weapons;
5. causal VFX geometry and palette;
6. background and painted MMO-icon finish;
7. a short constraint list containing only likely high-cost failures.

Describe the desired image before mentioning exclusions. Do not bury the action beneath long
negative lists. When reference images are used, assign each one a narrow role such as `fur
construction`, `weapon silhouette`, or `approved icon grammar`; state that its pose and composition
are not references unless they truly are.

After a rejection that changes subject, action, costume, or composition, write a new prompt from
the canonical brief. Do not edit or reference the rejected candidate. Use an existing candidate as
an edit target only when the user explicitly wants to preserve substantial parts of it.

## Gate image generation

Before calling an image tool, verify all of the following:

- the figure is performing the requested action rather than posing;
- clothing and armor match the resolved Skyrim archetype;
- weapon type, count, grip, and visible geometry are correct;
- each dominant effect is physically connected to its source;
- the camera and action axis are explicit;
- the image remains an MMO ability glyph rather than portrait or key art;
- no rejected content reference is being carried forward accidentally.

If the owner has rejected the prior candidate for alignment, present the new plain-language prompt
for semantic approval before generating. The owner validates meaning; they are not responsible for
prompt engineering.

After generation, inspect both the full-resolution image and a 32 px preview. Self-reject an output
that violates any hard brief field; do not ask the owner to review an obviously misaligned result.

For a prompt-only request, run the same pre-generation checks against the written prompt, return the
resolved brief and prompt, and stop without calling an image tool.
