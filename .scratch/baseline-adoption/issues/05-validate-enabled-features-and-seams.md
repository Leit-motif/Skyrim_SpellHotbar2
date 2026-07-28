# 05 — Validate enabled features and integration seams

**What to build:** Evidence that every enabled part of the Installed Configuration survives ordinary Nolvus Awakening play transitions and does not materially interfere with the installed UI, animation, combat, equipment, or input systems.

**Blocked by:** 02 — Validate Direct Cast with physical keyboard; 03 — Validate Direct Cast with native gamepad input; 04 — Validate Direct Cast through representative reWASD mappings.

**Status:** ready-for-agent

- [ ] Derive the smoke-test list from the recorded Installed Configuration and exercise every enabled FOMOD feature without expanding scope to unselected alternatives.
- [ ] Check hotbar and binding menus, menu input capture, OAR-driven casting animations, movement restrictions, equipment or selected-power restoration, resource costs, cooldowns, and installed combat/UI seams.
- [ ] Exercise modifier and combination bindings across the relevant physical-keyboard, native-gamepad, and reWASD paths where those bindings are configured.
- [ ] Check state after menu close, cell transition, save/reload, full game restart, and death/reload where applicable.
- [ ] Record repeatable stalls or meaningful frame-time degradation without requiring a formal benchmark when no performance problem is observed.
- [ ] Record every exercised cell as passed, failed, or open, with screenshots for visible claims and logs for relevant nonvisual claims.
- [ ] Treat harmless warnings and cosmetic observations as nonblocking notes unless they meet the definition of a Material Interaction.
- [ ] Restore the controlled fixture and close Skyrim unless an immediate authorized follow-up requires it to remain open.
