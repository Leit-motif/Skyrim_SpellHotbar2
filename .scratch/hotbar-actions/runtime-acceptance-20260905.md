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
- The third session was not force-killed. Final read-only `GetExitCodeProcess` reported `259` (`STILL_ACTIVE`), `HasExited=false`, `Responding=true`, and no main window, so PID 120812 is an active hung process rather than a zombie. The lease was recovered and is free.
- No Action matrix cell was claimed: slot dispatch, mode refusal, recursion, cost/GCD/CD, current-cast policy, save/reload, and owner visual/physical cells remain unproven. The active hung process must be cleared before any later runtime attempt.
