# 03 — Validate Direct Cast with native gamepad input

**What to build:** Evidence that Spell Hotbar 2's native gamepad path remains usable inside the real Nolvus Awakening input stack, while remaining distinguishable from reWASD keyboard emulation.

**Blocked by:** 01 — Establish the reproducible validation fixture.

**Status:** deferred

Deferred 2026-08-03 by the owner's keyboard-first MVP scope decision (see `../spec.md`). Gamepad is
an enhancement after MVP. Do not start this ticket as part of the MVP pass, and do not let its
`GP-*` cells be closed by anything but their own runtime evidence — deferred is not passed.

When it resumes: no physical button may be bound both natively and through a reWASD keyboard
mapping. Overlap is where the two paths collide.

- [ ] Exercise representative Direct Cast behavior through bindings received as native gamepad input while the installed input stack remains enabled.
- [ ] Cover the relevant fire-and-forget, concentration, targeting, camera, hand, and dual-cast cases needed to expose input-path-specific failures.
- [ ] Verify coexistence with Gamepad++, Input Switcher, reWASD, and every other recorded input-affecting mod rather than treating the controller as an isolated vanilla device.
- [ ] Check for collisions, duplicated actions, stuck or leaked modifiers, lost input, unintended device switching, and menu capture or release failures.
- [ ] Keep native gamepad results separate from physical-keyboard and reWASD-emulated keyboard results in the acceptance matrix.
- [ ] Record pass, fail, or open status with exact bindings, evidence, controlled-save identity, environment, commit, and binary.
- [ ] Inspect the SpellHotbar2 log and relevant runtime logs, capture visible evidence, restore the fixture, and close Skyrim unless an immediate authorized follow-up requires it to remain open.
