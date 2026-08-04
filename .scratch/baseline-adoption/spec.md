# Baseline Adoption Compatibility Validation

Status: ready-for-agent

## Problem Statement

The user has installed Spell Hotbar 2 because Direct Cast is the desired way to play, but the base mod has not yet been independently validated in the active Nolvus environment. A successful build or an apparently functional hotbar is insufficient: the user needs confidence that the exact Installed Configuration behaves as the upstream product normally should, that keyboard and controller play both remain usable, and that the Nolvus load order does not introduce unintended interactions.

Without an Accepted Baseline, later customization would make failures difficult to attribute. Any problem discovered after behavior changes could come from the base mod, the Nolvus environment, the input mapping, or the customization itself.

## Solution

Establish an Accepted Baseline for the Installed Configuration before making behavior changes. Validate the mod at one primary seam: live, player-visible behavior in Skyrim through the actual input stack. Exercise physical keyboard input, native gamepad input, and representative reWASD mappings separately. Use a controlled disposable save first and a representative playthrough save second as fixtures at that seam.

Direct Cast receives the deepest coverage because it is the primary reason for the Nolvus Integration Fork. Every enabled FOMOD feature receives a smoke test, while unselected installer alternatives are excluded. SKSE and crash/runtime logs, screenshots, exact environment identity, and an acceptance matrix provide supporting Compatibility Evidence. Static inspection and a successful build may support diagnosis, but they cannot establish runtime acceptance.

If a suspected Material Interaction is found, reproduce and attribute it before changing anything. A generally applicable native correction belongs in the Core Fork; records, presets, configuration, or other Nolvus-specific adaptations belong in the separate Compatibility Package. The baseline is accepted only when the Installed Configuration matches normal upstream expectations and no unresolved Material Interaction remains.

## User Stories

1. As the player, I want the exact Installed Configuration recorded, so that the validation scope is unambiguous.
2. As the maintainer, I want the fork commit and tested binary identified, so that every result can be reproduced from a known source.
3. As the maintainer, I want the active MO2 instance and profile identified before runtime work, so that evidence is tied to the intended Nolvus environment.
4. As the maintainer, I want exclusive ownership of the MO2 profile and Skyrim runtime confirmed, so that another task cannot invalidate the test state.
5. As the player, I want Direct Cast validated as the primary casting mode, so that the mod fulfills the reason I installed it.
6. As the player, I want fire-and-forget spells validated through Direct Cast, so that ordinary instant-release spells behave normally.
7. As the player, I want concentration spells validated through Direct Cast, so that hold, sustain, release, cost, and animation behavior remain correct.
8. As the player, I want self-targeted spells validated, so that spells without an aimed target cast reliably.
9. As the player, I want aimed and projectile spells validated, so that targeting and release behavior remain usable.
10. As the player, I want Direct Cast validated in first person, so that the supported first-person path behaves normally.
11. As the player, I want Direct Cast validated in third person, so that the normal controller-oriented camera path behaves normally.
12. As the player, I want left-hand and right-hand choices validated, so that hand-specific bindings produce the intended cast.
13. As the player, I want dual casting validated where the Installed Configuration and character perks enable it, so that supported spells cast and restrict movement as expected.
14. As the player, I want spell resource costs validated, so that Direct Cast does not bypass or incorrectly multiply costs.
15. As the player, I want cooldown behavior validated where enabled, so that repeated activation follows the configured rules.
16. As the player, I want equipment and selected-power state restored where upstream behavior promises restoration, so that casting does not disrupt ordinary play.
17. As the player, I want native keyboard activation validated, so that keyboard play works without an input translation layer.
18. As the player, I want native gamepad activation validated, so that controller play works through the mod's own gamepad handling.
19. As the player, I want representative reWASD-to-keyboard mappings validated, so that my normal controller combinations can drive keyboard bindings reliably.
20. As the maintainer, I want native gamepad and reWASD paths tested separately, so that a mapping problem is not mistaken for a native input problem.
21. As the player, I want modifier and combination bindings exercised where configured, so that realistic controller combinations do not collide or stick.
22. As the player, I want input conflicts with Skyrim and installed mods checked, so that hotbar activation does not unintentionally trigger another action.
23. As the player, I want hotbar and binding menus checked, so that configuration and normal menu interaction remain usable.
24. As the player, I want OAR-driven casting animations checked, so that enabled casting behavior does not visibly conflict with the animation environment.
25. As the player, I want movement restrictions checked during applicable casts, so that inputs are blocked and restored only when expected.
26. As the player, I want every enabled FOMOD feature smoke-tested, so that acceptance represents the installation I actually use.
27. As the maintainer, I want unselected FOMOD alternatives excluded, so that the compatibility check does not grow into an irrelevant installer matrix.
28. As the maintainer, I want a controlled disposable save tested first, so that failures can be reproduced in a low-noise environment.
29. As the player, I want a representative playthrough save tested after the controlled save, so that compatibility is demonstrated in realistic load-order state.
30. As the player, I want bindings and hotbar state checked after closing menus, so that ordinary UI transitions do not lose state.
31. As the player, I want behavior checked after a cell transition, so that world transitions do not break the hotbar or casting state.
32. As the player, I want behavior checked after save and reload, so that serialized bindings and mode state persist.
33. As the player, I want behavior checked after a full game restart, so that success does not depend on transient process state.
34. As the player, I want behavior checked after death and reload where applicable, so that interrupted casting state does not poison the restored save.
35. As the maintainer, I want the SpellHotbar2 SKSE log inspected, so that visible success is not accompanied by material plugin errors.
36. As the maintainer, I want relevant crash and runtime logs inspected, so that stability failures can be attributed with evidence.
37. As the player, I want repeatable stalls or meaningful frame-time regressions investigated, so that compatibility includes practical playability without requiring a formal benchmark.
38. As the maintainer, I want screenshots for visible acceptance claims, so that the visual evidence is reviewable rather than anecdotal.
39. As the maintainer, I want each acceptance result recorded as passed, failed, or open, so that unsupported cells are never implied to have passed.
40. As the maintainer, I want suspected conflicts reproduced before a fix is attempted, so that changes address an observed interaction.
41. As the maintainer, I want the interacting mod or subsystem identified when practical, so that the correction is placed at the right boundary.
42. As the maintainer, I want generally applicable native fixes made only in the Core Fork, so that core behavior remains independent of one load order.
43. As the maintainer, I want Nolvus-specific records, presets, and configuration placed only in the Compatibility Package, so that the Core Fork does not absorb private load-order assumptions.
44. As the player, I want harmless warnings and cosmetic observations documented without automatically blocking acceptance, so that the decision focuses on Material Interactions.
45. As the maintainer, I want reWASD evidence to record only the mappings exercised unless a defect depends on the full profile, so that evidence remains proportionate and private.
46. As the maintainer, I want the baseline checkpointed before customization begins, so that future regressions can be compared with a known-good state.
47. As the maintainer, I want future upstream changes deliberately reviewed against the Accepted Baseline, so that abandoned or sporadic upstream activity cannot silently alter the integration.
48. As the player, I want a clear acceptance decision, so that customization begins only after the base mod is trustworthy in my load order.

## Scope decision: keyboard-first MVP (2026-08-03)

The owner scoped the first pass to **physical keyboard only**. Gamepad work is an enhancement to
be taken after MVP, not part of it.

- Deferred: ticket 03 (native gamepad) and ticket 04 (representative reWASD mappings).
- Ticket 06 no longer waits on 03 or 04, and repeats material coverage on the keyboard path only.
- The acceptance decision in ticket 07 therefore yields a **keyboard-only Accepted Baseline**. It
  is not the Dual-Input Compatibility baseline defined in this specification's language, and must
  not be described as one.
- The `GP-*`, `RW-*`, `PLAY-GP-1`, and `PLAY-RW-1` cells stay `open`. Deferred by scope is not
  passed, and nothing may close them but their own runtime evidence.

Context for the deferred work, from the owner: play is normally on gamepad, with Auto Input Switch
allowing both devices and reWASD binding keyboard keys to gamepad buttons. When gamepad work
resumes, the governing constraint is that **no physical button may be bound both natively and
through a reWASD keyboard mapping** — overlap is where the two paths collide. Worth checking then:
whether reWASD's keyboard emulation makes Auto Input Switch flip prompts to keyboard mid-controller
play. Unverified.

## Implementation Decisions

- Baseline Adoption is a compatibility-validation milestone, not a reduced proof of concept and not a customization milestone.
- The validation target is the exact Installed Configuration. Before execution, record the selected FOMOD components, relevant MCM options, enabled compatibility data, and binding configuration. Do not infer the selection from files when it can be observed directly.
- The primary and highest test seam is live, player-visible Skyrim behavior through the actual input stack.
- The input paths are physical keyboard, native gamepad, and representative reWASD mappings that emit keyboard keys or combinations. Treat these as distinct paths even when they activate the same hotbar slot.
- Direct Cast is the center of the acceptance model and receives comprehensive behavior coverage. Other enabled features receive smoke coverage sufficient to expose integration failures.
- Use a controlled disposable save first and a representative playthrough save second. They are fixtures at the same live-game seam, not substitutes for one another.
- Before deployment or runtime mutation, identify the active MO2 instance and profile, confirm exclusive ownership, and preview any deployment or rollback operation.
- Use the built artifact from a named Core Fork commit. Record a binary hash or equivalent immutable identity with the evidence.
- Do not modify the Core Fork or create a Compatibility Package merely because a theoretical conflict exists. Reproduce a Material Interaction first.
- Native behavior, fixes, and improvements that are generally applicable belong in the Core Fork.
- Records, presets, configuration, and adaptations specific to the Nolvus load order belong in the Compatibility Package.
- The Compatibility Package remains a separate runtime layer even when a finding is discovered while testing the Core Fork.
- The result is a Personal Integration. It is not a public compatibility claim, supported community release, or permission to redistribute modified binaries.
- Preserve the Accepted Baseline as a stable checkpoint before any customization work begins.
- Review future upstream changes deliberately rather than automatically merging them. Rerun all acceptance cells affected by a reviewed change.
- Restore runtime fixtures after validation and close Skyrim unless an immediate authorized follow-up requires it to remain open.

## Testing Decisions

- Good tests assert externally observable behavior at the live-game seam: the requested activation occurs, the intended spell or enabled feature behaves as expected, player state is restored correctly, and no Material Interaction appears. Avoid tests that pass solely because an internal function ran or a build succeeded.
- Create an acceptance matrix that records the exact spell or feature exercised, input path, camera perspective, hand mode, save fixture, persistence transition, expected result, actual result, evidence paths, and status.
- Validate Direct Cast with representative fire-and-forget, concentration, self-targeted, and aimed/projectile spells.
- Validate first-person and third-person behavior.
- Validate left-hand and right-hand behavior, plus dual cast only where the Installed Configuration and character state enable it.
- Observe cast initiation, release or sustained concentration, targeting, animations, movement restrictions, resource costs, cooldowns, cancellation, and restoration of equipment or selected powers where applicable.
- Exercise physical keyboard, native gamepad, and representative reWASD mappings separately. Record the exact reWASD mappings exercised; do not require the complete profile unless diagnosing a mapping-dependent defect.
- Smoke-test each enabled FOMOD feature. Build the feature list from the recorded Installed Configuration rather than from every capability described upstream.
- Check seams with installed input, UI, combat, animation, and equipment systems, including menu capture, OAR behavior, movement input, equipment restoration, spell costs, and cooldowns.
- Run first on a controlled disposable save, then repeat the material coverage on a representative playthrough save.
- Exercise persistence across menu close, cell transition, save/reload, full game restart, and death/reload where applicable.
- Inspect the SpellHotbar2 SKSE log after each coherent run and inspect relevant crash/runtime logs when a failure, warning, stall, or crash occurs.
- Note repeatable stalls or meaningful frame-time degradation. A formal performance benchmark is not required for baseline acceptance.
- Capture screenshots for visible behavior claims and retain logs or excerpts needed to support nonvisual claims.
- A cell passes only when its expected external behavior is directly observed and its evidence is tied to the exact fork commit, binary, MO2 instance/profile, Installed Configuration, save, and input path.
- A cell remains open when it was not exercised or lacks adequate evidence. Static validation must never close a runtime cell.
- A suspected Material Interaction must be reproduced before remediation. Record reproduction steps and identify the interacting mod or subsystem when the evidence permits.
- After any fix, rerun the failed cell and the smallest relevant regression set. A Core Fork change also requires a build/static verification checkpoint before runtime retest, but that checkpoint is not itself acceptance.
- Baseline Adoption passes when all required cells for the Installed Configuration pass and no unresolved Material Interaction remains. Harmless warnings and nonblocking cosmetic observations may be documented separately.
- Prior art in this repository consists of the upstream live behaviors described for Direct Cast, controller support, first-person and concentration support, dual casting, restoration, cooldowns, and FOMOD-selected integrations. There is no existing automated runtime suite that can replace live verification.

## Out of Scope

- Testing unselected FOMOD components or every installer permutation.
- Redesigning Direct Cast, input handling, animations, the UI, balance, or any other product behavior.
- Implementing speculative fixes before a Material Interaction is reproduced.
- Building a public release, public support matrix, or compatibility promise for other Nolvus installations.
- Publishing modified binaries while upstream modification and redistribution rights remain unclear.
- Archiving or distributing the user's complete reWASD profile unless a specific defect makes it necessary.
- Formal performance benchmarking when no repeatable performance problem is observed.
- Automatically merging future upstream changes.
- Beginning post-baseline customization.

## Further Notes

- Direct Cast is the principal acceptance path; breadth across enabled features must not dilute its depth.
- “Normal upstream expectations” means the behavior described by the upstream product and observed without a repeatable Nolvus-specific conflict. It does not require preserving an upstream defect if a generally applicable defect is independently demonstrated.
- A harmless warning, an unselected-feature issue, or a theoretical record conflict is not a Material Interaction.
- Runtime execution must follow repository ownership and deployment guardrails. This specification authorizes planning only; the agent executing validation must separately establish the live environment and mutation authority.
