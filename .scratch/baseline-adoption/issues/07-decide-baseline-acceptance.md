# 07 — Decide Baseline Adoption acceptance

**What to build:** A reviewable decision stating whether the exact Installed Configuration is an Accepted Baseline for personal use in Nolvus Awakening, supported by a complete evidence matrix and explicit treatment of every suspected interaction.

**Blocked by:** 05 — Validate enabled features and integration seams; 06 — Validate the representative playthrough save.

**Status:** dropped 2026-08-03 — the Baseline Adoption effort was superseded by
`../../mco-integration/spec.md` and this ticket was never run. Do not implement it; no cell
here may be read as passing. The header still said `ready-for-agent` for 26 days after the
spec dropped it; corrected in the 2026-08-29 sweep.

- [ ] Review the complete acceptance matrix and leave every unexercised or inadequately evidenced cell open rather than implying it passed.
- [ ] Confirm that runtime evidence is tied to the exact fork commit, binary, Nolvus Awakening instance/profile and load order, Installed Configuration, save fixtures, and input paths.
- [ ] Review SpellHotbar2, crash, and relevant runtime logs together with screenshots and stability observations.
- [ ] For each suspected Material Interaction, record reproducible steps and the interacting mod or subsystem when evidence permits; create separate diagnosis or remediation work rather than changing behavior within this ticket.
- [ ] Route any later generally applicable native correction to the Core Fork and any Nolvus-specific records, presets, configuration, or adaptation to the separate Compatibility Package.
- [ ] Accept the baseline only if all required cells for the Installed Configuration pass and no unresolved Material Interaction remains.
- [ ] Record harmless warnings, cosmetic observations, and excluded unselected-feature cases separately from blocking findings.
- [ ] Checkpoint the acceptance decision before any customization begins and state which acceptance cells future upstream changes would require rerunning.
