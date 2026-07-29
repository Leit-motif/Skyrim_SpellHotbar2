# 01 — Establish the reproducible validation fixture

**What to build:** A reproducible Baseline Adoption fixture for the exact Installed Configuration in Nolvus Awakening, with every later runtime result attributable to a known source, binary, load order, save, and input configuration.

**Blocked by:** None — can start immediately.

**Status:** claimed

- [ ] Record the exact selected FOMOD components, relevant MCM options, enabled compatibility data, hotbar bindings, and enabled input stack, including Gamepad++, Input Switcher, reWASD, and other input-affecting mods present.
- [ ] Record the Core Fork commit and immutable identity of the tested DLL.
- [ ] Inspect and record the active Nolvus Awakening MO2 instance, profile, mod list/load order, and relevant runtime version information.
- [ ] Identify a controlled disposable save and a representative playthrough save without mutating either during fixture preparation.
- [ ] Confirm exclusive ownership of Skyrim and the active MO2 profile before any runtime mutation.
- [ ] Preview any required deployment or rollback operation and record the intended source, destination, and restoration path before applying it.
- [ ] Create an acceptance matrix that can record spell or feature, input path, camera, hand mode, save fixture, persistence transition, expected and actual results, evidence, and pass/fail/open status.
- [ ] Leave every runtime acceptance cell open; static inspection, fixture preparation, and a successful build do not count as runtime proof.
