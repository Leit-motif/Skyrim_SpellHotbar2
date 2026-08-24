# Weapon Art icon authoring handoff

Updated: 2026-08-23

## Resume here

- Worktree: `C:\Nolvus\Projects\spell-hotbar-2-ticket-06-icons`
- Branch: `ticket-06-weapon-art-icons`
- Integration target: `weapon-arts`, not `main`
- Active art: **18 — Earth Shatter** (`aow_18_earth_shatter`)
- Status: **not started**
- Inventory: **16 of 57 finalized; 41 remain**

Do not restart the set and do not regenerate Crane Style through Disengage. Their approved assets,
prompts, and notes are already recorded in `python_scripts/weapon_art_icons/manifest.tsv`.

## Divided Strike disposition

The owner directed that `divided_strike_cross_slash_v8.png` ship under time constraint on
2026-08-23. It is usable but explicitly not the visual result the owner wanted. The approved master,
128 px atlas input, prompt, and this compromise are recorded under `python_scripts/weapon_art_icons/`.

## Divine Smite disposition

The owner approved the attached original two-handed-grip Divine Smite render on 2026-08-23, not
the later one-handed correction. The exact attachment is the approved master. Its 128 px atlas
input, prompt, animation/effect evidence, and catalogue-class mismatch note are recorded under
`python_scripts/weapon_art_icons/`.

## Double Slash disposition

The owner approved `double_slash_breton_knight_candidate_v5.png` on 2026-08-23. The final
effect-first MMO glyph intentionally retains one dominant slash despite two swing events and the
ability name. Its source figure is a small French-medieval knight-coded Breton rather than a
portrait or character illustration. The approved master, 128 px atlas input, prompt, and evidence
are recorded under `python_scripts/weapon_art_icons/`.

## Dragon Strike disposition

The owner approved `dragon_strike_candidate_v1.png` on 2026-08-23. The effect-first glyph uses a
frontal depth-axis composition: one white-hot flaming sword and its foreground impact dominate the
square, with a small faceless warrior behind them. Symmetrical flame curls suggest dragon jaws or
swept horns without depicting a literal dragon. The approved master, 128 px atlas input, prompt,
animation evidence, and Flaming Strike payload interpretation are recorded under
`python_scripts/weapon_art_icons/`.

## Dual Flurry context-layer pivot

Final disposition: the owner supplied and approved an externally generated Gemini final cut on
2026-08-23 after the Codex generation attempts repeatedly failed to keep the axe heads, cutting
directions, and energy trails physically coherent. The exact uploaded PNG is canonical. It shows a
leaping blond Nord barbarian whose two inward-facing axes generate the crossed crimson trails along
their curved lines of motion. The approved master, 128 px atlas input, provenance record, and
animation evidence are stored under `python_scripts/weapon_art_icons/`.

The prompt-layer history below remains useful as a record of the failure and the context work, but
its draft is superseded and must not be used to regenerate or replace the approved final cut.

Several unapproved candidates exposed a prompt-authoring problem rather than an image-backend
problem: race labels were treated as costumes, prior rejected candidates carried visual content
forward, effect geometry overruled body action, and generic fantasy armor displaced the requested
Skyrim archetype. The owner paused generation on 2026-08-23 to build a durable context layer.

Canonical context now begins at:

- `python_scripts/weapon_art_icons/skyrim-visual-language.md`
- `python_scripts/weapon_art_icons/prompt-brief-template.md`
- `.agents/skills/skyrim-weapon-art-icon-prompter/SKILL.md`

The superseded prompt-only forward test is
`.scratch/weapon-arts/prompt-drafts/aow_17_dual_flurry-context-v1.md`. It resolves Dual Flurry as a
leaping Nord barbarian in rough fur and hide, using two compact one-handed axes to physically create
two red trails that cross once. Do not use it to replace the approved owner-supplied final.

## Earlier Divided Strike stopping point

The animation evidence supports a three-beat dual-weapon sequence: off-hand collision first,
main-hand collision second, then both weapon nodes collide in the terminal beat. The intended icon
is a figure visibly moving through that attack and ending in a cross-cutting X strike. The pose must
create the attack; luminous trails cannot be pasted over an idle or weapon-display stance.

The closest candidate before pausing is locally preserved at:

`python_scripts/weapon_art_icons/pilot/source/divided_strike_closest_v5.png`

Owner assessment: “better,” with the advancing motion accepted as progress, but the X still needed
to be **convex**, and the dagger might need to change to make the physical trajectory coherent.

The most recent candidate is locally preserved at:

`python_scripts/weapon_art_icons/pilot/source/divided_strike_rejected_convex_v6.png`

That candidate is explicitly **wrong**. It interpreted convexity as two large continuous opposing
C-curves, producing an hourglass/parenthesis shape rather than the owner's intended convex X. Do
not continue from its trail geometry and do not call it approved.

Before another generation, establish the intended convex-X geometry with the owner in plain shape
language or a quick owner-supplied sketch/reference. Preserve the moving lunge from the closest v5
candidate, then change the weapon and trail geometry together. Do not guess at “convex” again.

## Locked project direction

- Original generated art only; no third-party artwork or Bethesda assets are copied or shipped.
- MMO/RPG hotbar convention: generalized faceless figure, one dominant action, readable at 32 px.
- Elder Scrolls race and palette shorthand live in `figure-guidance.md` and `color-guidance.md`.
- Backgrounds should vary across the atlas without defaulting every icon to black or adding haze.
- Figure facing and action axes must rotate across the atlas; every new prompt names an explicit
  orientation and avoids repeating the recent upper-left-to-lower-right default unless the clip
  requires it. See `composition-guidance.md`.
- Catalogue wiring waits until the atlas contains the approved stable keys.
