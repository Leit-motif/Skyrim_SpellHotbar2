# 06 — Validate the representative playthrough save

**What to build:** Evidence that the material Direct Cast, input, feature, and persistence behavior proven on the controlled fixture also works in the player's realistic Nolvus Awakening playthrough state.

**Blocked by:** 02 — Validate Direct Cast with physical keyboard; 03 — Validate Direct Cast with native gamepad input; 04 — Validate Direct Cast through representative reWASD mappings; 05 — Validate enabled features and integration seams.

**Status:** dropped 2026-08-03 — the Baseline Adoption effort was superseded by
`../../mco-integration/spec.md` and this ticket was never run. Do not implement it; no cell
here may be read as passing. The header still said `ready-for-agent` for 26 days after the
spec dropped it; corrected in the 2026-08-29 sweep.

- [ ] Load the recorded representative playthrough save through the same active Nolvus Awakening MO2 instance, profile, load order, Installed Configuration, commit, and binary used for controlled validation.
- [ ] Repeat the material Direct Cast coverage across physical keyboard, native gamepad, and representative reWASD paths.
- [ ] Repeat the enabled-feature, input-stack, UI, animation, combat, equipment, and persistence checks most likely to vary with realistic save or load-order state.
- [ ] Record any deviation from the controlled fixture as passed, failed, or open with reproducible steps and evidence.
- [ ] Inspect the SpellHotbar2 log and relevant crash/runtime logs and record repeatable stability or performance symptoms.
- [ ] Capture screenshots for visible claims and tie all evidence to the representative save identity.
- [ ] Restore the playthrough fixture and close Skyrim unless an immediate authorized follow-up requires it to remain open.
