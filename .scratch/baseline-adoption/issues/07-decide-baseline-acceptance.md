# 07 — Decide Baseline Adoption acceptance

**What to build:** A reviewable decision stating whether the exact Installed Configuration is an Accepted Baseline for personal use in Nolvus Awakening, supported by a complete evidence matrix and explicit treatment of every suspected interaction.

**Blocked by:** 05 — Validate enabled features and integration seams; 06 — Validate the representative playthrough save.

**Status:** ready-for-agent

Scoped 2026-08-03 by the keyboard-first MVP decision (see `../spec.md`). What this ticket can
accept is a **keyboard-only Accepted Baseline**. It is not the Dual-Input Compatibility baseline
this project's language defines, and the decision must say so in those words. The `GP-*`, `RW-*`,
`PLAY-GP-1`, and `PLAY-RW-1` cells remain `open`; accept around them, never through them.

- [ ] Review the complete acceptance matrix and leave every unexercised or inadequately evidenced cell open rather than implying it passed.
- [ ] Confirm that runtime evidence is tied to the exact fork commit, binary, Nolvus Awakening instance/profile and load order, Installed Configuration, save fixtures, and input paths.
- [ ] Review SpellHotbar2, crash, and relevant runtime logs together with screenshots and stability observations.
- [ ] For each suspected Material Interaction, record reproducible steps and the interacting mod or subsystem when evidence permits; create separate diagnosis or remediation work rather than changing behavior within this ticket.
- [ ] Route any later generally applicable native correction to the Core Fork and any Nolvus-specific records, presets, configuration, or adaptation to the separate Compatibility Package.
- [ ] Accept the baseline only if all required cells for the Installed Configuration pass and no unresolved Material Interaction remains.
- [ ] Record harmless warnings, cosmetic observations, and excluded unselected-feature cases separately from blocking findings.
- [ ] Checkpoint the acceptance decision before any customization begins and state which acceptance cells future upstream changes would require rerunning.
