# 04 — Validate Direct Cast through representative reWASD mappings

**What to build:** Evidence that the player's representative controller combinations can activate Spell Hotbar 2 keyboard bindings reliably through reWASD without obscuring failures in the native gamepad path.

**Blocked by:** 01 — Establish the reproducible validation fixture.

**Status:** ready-for-agent

- [ ] Record and exercise representative reWASD mappings that emit the keyboard keys, modifiers, or combinations used by the Installed Configuration.
- [ ] Exercise enough fire-and-forget, concentration, targeting, camera, and hand cases to expose mapping-path-specific failures.
- [ ] Check for collisions, duplicated activation, stuck modifiers, premature release, menu capture, and interference from Gamepad++, Input Switcher, or other recorded input-affecting mods.
- [ ] Keep reWASD-emulated keyboard results separate from physical-keyboard and native-gamepad results in the acceptance matrix.
- [ ] Retain the complete reWASD profile as supplementary evidence when it is easy to export and does not expose unrelated sensitive configuration; lack of a full export does not block acceptance when the exercised mappings are recorded.
- [ ] Record pass, fail, or open status with mapping evidence, controlled-save identity, environment, commit, and binary.
- [ ] Inspect relevant logs, capture visible evidence, restore the fixture, and close Skyrim unless an immediate authorized follow-up requires it to remain open.
