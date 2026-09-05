# Hotbar Actions runtime acceptance evidence

Date: 2026-09-05 09:25–09:38 America/Chicago  
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
- The final runtime lease was released cleanly; `runtime status` is `free`.

## Runtime blocker

- Skyrim was launched through the MO2 `moshortcut://:SKSE` path. DevBench loaded and `ping` returned `pong`.
- On both launches, the first main-thread `inspect` call at the title screen returned the documented 504: the frame counter did not advance and the game was fully paused or hung. The second launch used PID 101300.
- The second session was given a graceful close request; no process kill was used. Final read-only process check reported `HasExited=false` / `STILL_ACTIVE`, so PID 101300 is an active hung process rather than a zombie.
- No Action matrix cell was claimed: `game loadLast` was not reached, so slot dispatch, mode refusal, recursion, cost/GCD/CD, current-cast policy, save/reload, and owner visual/physical cells remain unproven.
- The next runtime attempt must clear the active hung process outside this task, then make `game loadLast` the first main-thread DevBench call before any `inspect` call.
