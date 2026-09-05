# Hotbar Actions runtime acceptance evidence

Date: 2026-09-05 09:25–09:48 America/Chicago
Source commit: `0ad742eab92ad3108b3f2b2cad669b3a4124e27d` on `ng/smf-next`  
Review: Standards PASS and Spec PASS against fixed point `429b937`; no concrete blockers in source scope.

## Static evidence

- Native Release build completed; source DLL SHA-256: `346908B15B72B887781FCCFD192D779F4B1680898FD23B94AC28E8A8E1332788`.
- CTest: `ctest --test-dir skse_plugin/build/release -C Release --output-on-failure` — 17/17 passed.
- Papyrus `SpellHotbar.psc` compiled with Bethesda `PapyrusCompiler.exe` — 0 errors, 0 warnings; tracked PEX SHA-256: `F993A689159A5ADAF9600D36F1BB5FC0D43A70E3E8B31CD00EBE82D8BA5214F`.
- Release guard: `python python_scripts/build_mod_release.py --check` — passed; 267 files, 2 intended overwrites, 0 redundant files.

## Deployment evidence

- `skyrim-agent` bridge was refreshed and MO2 restarted; doctor reported bridge connected to `Nolvus Awakening`.
- Dry-run and apply used the root `skyrim-agent.json` and staged Data-shaped runtime tree. Apply completed through the bridge at MO2 priority 4621 for `Dev - Spell Hotbar 2 SMF Next`.
- Active profile: `C:\Nolvus\Instances\Nolvus Awakening\MODS\profiles\Nolvus Awakening`.
- Overlay hashes matched the staged source exactly:
  - `SKSE/Plugins/SpellHotbar2.dll`: `346908B15B72B887781FCCFD192D779F4B1680898FD23B94AC28E8A8E1332788`
  - `Scripts/SpellHotbar.pex`: `F993A689159A5ADAF9600D36F1BB5FC0D43A70E3E8B31CD00EBE82D8BA5214F`
  - `SKSE/Plugins/SpellHotbar/localization/translation.txt`: `5C64812817E7DF52B932C04E592672FFFABE92B2D9F3E83E3F6BA99CB0C0AB56`
- The final runtime lease was recovered with the CLI preview/apply path after the ambiguous DevBench drain; `runtime status` is `free`.

## Runtime blocker

- Skyrim was launched through the MO2 `moshortcut://:SKSE` path. DevBench loaded and `ping` returned `pong`.
- The first two launches stalled on a title-screen `inspect` call before a load; both returned the documented 504/`DEVBENCH_DRAIN` because the frame counter did not advance. The second launch used PID 101300 and was later closed gracefully.
- The third launch used PID 120812. `game loadLast` was queued first for `Save17_00000000_0_43532D544553542D4E4557_Tamriel_000251_20260905131608_1_1`; the first post-load `inspect kind=state` then returned `DEVBENCH_DRAIN` after no frame progress.
- The third session was not force-killed. The earlier `active hung` wording is superseded: contemporaneous `CommunityShaders.log` evidence showed disk-cache invalidation followed by `ShaderTiming` progress (`remaining 3412 -> 3211`), so that session is an unresolved load/compile stall rather than a proven Skyrim hang. The lease was recovered and is free.
- No Action matrix cell was claimed: slot dispatch, mode refusal, recursion, cost/GCD/CD, current-cast policy, save/reload, and owner visual/physical cells remained unproven pending a later runtime attempt.

## Follow-up: binding-tab layout fix deployment

- Source fix: commit `6ad3dd5` (`Fit binding menu category tabs`). It caps the fifteen category buttons to the available left-pane width while preserving the existing one-row layout and the 60px scaled maximum. The user-provided visual symptom is recorded at `C:\Users\Rando\AppData\Local\Temp\codex-clipboard-7ad219f2-c6e1-4b66-926a-cdd9aaec292d.png`.
- Static validation was unchanged from the reviewed source: native Release build, CTest 17/17, Papyrus 0 errors/0 warnings, and the release guard passed. The rebuilt DLL SHA-256 is `9B889C7073A0F02ACD6C716D5B478ADCDC4961F8D73196C474975FE74042FF62`.
- `skyrim-agent` dry-run and apply completed through the `Nolvus Awakening` MO2 profile at priority 4621. Deployment record: `.skyrim-agent/deployment.json`, id `20260905T152057819Z`. The active overlay DLL matches the rebuilt SHA; the PEX remains `F993A689159A5ADAF9600D36F1BB5FC0D43A70E3E8B31CD00EBE82D8BA5214F`.
- The named DevBench save request did not create a file or temporary save. The newest existing save, `Save18_00000000_0_43532D544553542D4E4557_Tamriel_000254_20260905145806_1_1`, was preserved and queued for load after deployment.
- The replacement Skyrim process is PID 53740. After the normal multi-minute load window, `CommunityShaders.log` showed cached features valid, shader resources loading, and `InitializeMenuIcons: Loaded 22/22 icons successfully`; no Community Shaders settings or cache were changed. A later DevBench state drain remained ambiguous, so no runtime acceptance cell is claimed. `GetExitCodeProcess` semantics confirmed the process is active (`HasExited=false`); it remains running for owner visual review. The orphaned lease was recovered through the documented CLI preview/apply path and is now free.
- Owner visual check remains open: with the preserved save loaded in the `Nolvus Awakening` profile, open Spell Hotbar 2's Binding Menu and verify that the `Actions` category is visible in the left tab row. Slot dispatch and all earlier action-matrix cells remain unproven.
