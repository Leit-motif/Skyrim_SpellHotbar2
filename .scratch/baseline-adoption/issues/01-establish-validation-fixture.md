# 01 — Establish the reproducible validation fixture

**What to build:** A reproducible Baseline Adoption fixture for the exact Installed Configuration in Nolvus Awakening, with every later runtime result attributable to a known source, binary, load order, save, and input configuration.

**Blocked by:** None — can start immediately.

**Status:** claimed

- [ ] Record the exact selected FOMOD components, relevant MCM options, enabled compatibility data, hotbar bindings, and enabled input stack, including Gamepad++, Input Switcher, reWASD, and other input-affecting mods present.
- [ ] Record the Core Fork commit and immutable identity of the tested DLL.
- [ ] Inspect and record the active Nolvus Awakening MO2 instance, profile, mod list/load order, and relevant runtime version information.
- [x] Identify a controlled disposable save and a representative playthrough save without mutating either during fixture preparation.
- [ ] Confirm exclusive ownership of Skyrim and the active MO2 profile before any runtime mutation.
- [ ] Preview any required deployment or rollback operation and record the intended source, destination, and restoration path before applying it.
- [x] Create an acceptance matrix that can record spell or feature, input path, camera, hand mode, save fixture, persistence transition, expected and actual results, evidence, and pass/fail/open status.
- [x] Leave every runtime acceptance cell open; static inspection, fixture preparation, and a successful build do not count as runtime proof.

## Comments

### 2026-07-29 — Prepared through the runtime ownership gate

Claim checkpoint: `f3a380ff10c922b1487bcbe8a7d6be125a60c716`.

Created `../fixture.md` and `../acceptance-matrix.csv`. Read-only inspection identified immutable candidates for the installed and Core Fork DLLs, fingerprinted the intended profile and runtime, identified and hashed two save candidates without loading them, inventoried the installed input stack, and left all runtime matrix rows open.

Stopped before activation, deployment, or in-game testing as requested. Skyrim was already running for another task and the live MO2 bridge reported `Dev - Skeleton`, not `Nolvus Awakening`; exclusive ownership therefore could not be confirmed. The intended profile also does not currently include Spell Hotbar in its mod list or enable its plugins. Exact FOMOD answers, applied MCM options, hotbar bindings, active input mode, and representative reWASD mappings remain unobserved. The ticket stays `claimed`.
