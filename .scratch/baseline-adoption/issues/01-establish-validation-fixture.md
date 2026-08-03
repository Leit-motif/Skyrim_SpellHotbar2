# 01 — Establish the reproducible validation fixture

**What to build:** A reproducible Baseline Adoption fixture for the exact Installed Configuration in Nolvus Awakening, with every later runtime result attributable to a known source, binary, load order, save, and input configuration.

**Blocked by:** None — can start immediately.

**Status:** claimed

- [x] Record the exact selected FOMOD components, relevant MCM options, enabled compatibility data, hotbar bindings, and enabled input stack, including Gamepad++, Input Switcher, reWASD, and other input-affecting mods present. — FOMOD components and compatibility data derived decisively; input stack recorded. **MCM options and hotbar bindings do not exist yet**: they serialize into the SKSE co-save and the mod had never been active in this profile, so the fixture records their absence and assigns their first capture to ticket 02. reWASD was not running at capture; its mappings are recorded per-mapping in ticket 04.
- [x] Record the Core Fork commit and immutable identity of the tested DLL.
- [x] Inspect and record the active Nolvus Awakening MO2 instance, profile, mod list/load order, and relevant runtime version information.
- [x] Identify a controlled disposable save and a representative playthrough save without mutating either during fixture preparation.
- [x] Confirm exclusive ownership of Skyrim and the active MO2 profile before any runtime mutation.
- [x] Preview any required deployment or rollback operation and record the intended source, destination, and restoration path before applying it.
- [x] Create an acceptance matrix that can record spell or feature, input path, camera, hand mode, save fixture, persistence transition, expected and actual results, evidence, and pass/fail/open status.
- [x] Leave every runtime acceptance cell open; static inspection, fixture preparation, and a successful build do not count as runtime proof.

## Comments

### 2026-07-29 — Prepared through the runtime ownership gate

Claim checkpoint: `f3a380ff10c922b1487bcbe8a7d6be125a60c716`.

Created `../fixture.md` and `../acceptance-matrix.csv`. Read-only inspection identified immutable candidates for the installed and Core Fork DLLs, fingerprinted the intended profile and runtime, identified and hashed two save candidates without loading them, inventoried the installed input stack, and left all runtime matrix rows open.

Stopped before activation, deployment, or in-game testing as requested. Skyrim was already running for another task and the live MO2 bridge reported `Dev - Skeleton`, not `Nolvus Awakening`; exclusive ownership therefore could not be confirmed. The intended profile also does not currently include Spell Hotbar in its mod list or enable its plugins. Exact FOMOD answers, applied MCM options, hotbar bindings, active input mode, and representative reWASD mappings remain unobserved. The ticket stays `claimed`.

### 2026-08-03 — Fixture established

The 7/29 ownership blocker cleared. The owner granted exclusive ownership of the `Nolvus
Awakening` MO2 instance and the Skyrim runtime with mutation allowed, and enabled `Spell Hotbar 2`
and `Dev - Spell Hotbar 2` in that profile through MO2. Skyrim was not running and DevBench was
offline throughout. No in-game session was run; every acceptance cell stays `open` and the
matrix is unchanged.

`fixture.md` is rewritten from the ground up. What changed materially:

**Tested binary now has a proven producing commit.** Built from a clean worktree at `a50bda1`,
staged outside the MO2 instance, SHA-256 `9846FB9B…`. The 7/23 artifact `93357D44…` is explicitly
demoted — its producing commit was never proven — and preserved as the rollback file. Deployment
to `Dev - Spell Hotbar 2` was previewed, applied, and hash-verified at the destination; the
installed release mod and all profile files were untouched.

Source provenance is mechanically checkable rather than asserted: `git diff f203cd2...HEAD` over
the source directories touches only `CMakeLists.txt` and the two build `.bat` files, all added in
`7b40a68` before the review baseline. No C++ under `skse_plugin/src` differs from upstream
`0.0.14`, so the tested binary is a local build of unmodified upstream source.

**FOMOD answers were derived decisively, not guessed.** MO2 recorded no installer choices
(`[installedFiles]` empty) and the source archive is deleted, so direct observation was
impossible; the owner directed that the payload be authoritative. Rather than describe files,
the derivation inverts `python_scripts/create_fomod_installer.py` — the script that *generates*
this release's `ModuleConfig.xml` — against the files on disk. All six installer groups resolved
to exactly one answer each: Battle Mage **CSF2**, Perk Overhaul **Vokriinator Black**, **22 of 31**
spell packs, **Nordic UI**, **Sovngarde** font, Auto Profile **Controller with Bindmenu**. The
sharpest evidence is a byte-exact hash match between `auto_profile.json` and
`controller_bindmenu.json`, and the `Sovngarde` family name read out of the installed TTF's name
table, which separates it from the two other options that write the same filename.

Three findings change how later tickets must be run, and are the reason this was worth doing
before ticket 02 rather than during it:

1. **The auto-loaded profile is the controller preset with the Bind Menu**, so the keyboard cells
   (`KB-*`) are not exercising a shipped default binding set. Bindings must be established
   deliberately and stated in each cell.
2. **Four selected spell packs are inert here** because their gating plugins are not enabled
   (`odin`, `stellaris`, `shadow_spell_package`, `elemental_mastery_magic`). They will log
   `Skipping Plugin … not loaded`, which is expected and not a Material Interaction, and they are
   excluded from ticket 05's smoke coverage — a feature whose plugin is absent cannot pass.
3. **The 2026-07-30 `SpellHotbar2.log` is not evidence for this baseline.** It came from the
   `Dev - Skeleton` profile, and its "not loaded" list contains exactly the packs that *are* active
   here. The SKSE log path is shared across profiles, so `LOG-SKSE-1` needs the log bounded at the
   start of its own run.

Ownership-gated work applied: profile activation (by the owner), the DLL deployment above, and a
profile snapshot held beside the live files as `*.bak-sh2-baseline01-20260803`. Rollback paths are
recorded at three scopes. Save fixtures were read and hashed only and are byte-identical to the
7/29 record despite heavy unrelated use of the same character that day.

Open by construction: no bindings, MCM state, or mode state exist for either save fixture, because
they serialize into the SKSE co-save and the mod has never run in this profile. Their first
capture is ticket 02's opening act.
