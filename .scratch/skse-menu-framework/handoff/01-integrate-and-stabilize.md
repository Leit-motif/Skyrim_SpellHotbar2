# Phase 1 — integrate and stabilize the SMF host

Start from `C:\Nolvus\Projects\spell-hotbar-2-smf-upstream` on clean branch
`codex/smf-migration` at
`5b6219166a1e51bd04c9608d2259979a6fc3dfea`.

## Step 1: integrate the isolated host

Cherry-pick `79d5b4b52803cbfc9d2e21b85ebcd5ae0d84f205`. Resolve conflicts by
preserving both sides of this matrix:

| Conflict area | Preserve from `5b62191` | Preserve from `79d5b4b` |
|---|---|---|
| `plugin.cpp` | lifecycle new/load messages; serialization revert callback | fail-closed `SmfGuest::install`; no `RenderManager::install` |
| `input.cpp/.h` | `Input::process_event`, injected-event adapter, no trampoline hook | no private ImGui IO forwarding; SMF callback ownership |
| `CMakeLists.txt` | lifecycle/storage-reset sources and tests | vendored SMF include, SMF sources, removed imgui/DirectXTK, opt-in output |
| `PCH.h` | current CommonLib/project headers | `imgui_mcp_compat.h` and SMF guest vocabulary |
| rendering | SH2 domain widgets and window state | SMF callbacks/textures and removal of private context/backends/hooks |

Register `Input::process_event` exactly once through SMF's input export. Preserve
the existing filter policy and synthetic shout down/up injection. The final tree
has no `Input::install_hook`, no SH2 `CreateContext`, no ImGui backend init, no
Present/D3D/WndProc hook, and no second input forwarding path.

Completion criterion: a clean Release DLL links from the integrated tree and both
`verify_smf_guest_boundary.py` and the unit tests pass.

## Step 2: close the known review findings

Address every item before MCP work:

1. `python_scripts/verify_smf_migration.py` is deliberately incomplete. Update
   its required header path to
   `skse_plugin/third_party/skse-menu-framework/SKSEMenuFramework.h`; include
   committed, staged, unstaged, and untracked names in the upstream-only check;
   check all six retired scripts/PEX names and all five sanctioned VMAD removals;
   and make plugin-source/build invariants explicit.
2. Encode plugin provenance rather than trusting a local import. Add a manifest
   tying the YAML trees to the official upstream `0.0.14` ESPs and proving that
   only these record fields changed:
   - main QUST `000800`: VMAD removed/empty; StartGameEnabled removed;
   - main QUST `000D62`: VMAD removed/empty; StartGameEnabled removed;
   - main MGEF `00083A`: VMAD removed/empty;
   - BattleMage QUST `000800`: VMAD removed/empty; StartGameEnabled removed;
   - BattleMage MGEF `000802`: VMAD removed/empty.
   Preserve every record and FormID. Make `build_plugins.py` verify the manifest.
3. Confirm the safe BattleMage call in `lifecycle.cpp` on all targets. It now
   gates on optional `SpellHotbar_BattleMage.esp`, loads/inspects the
   `CustomSkills.OpenCustomSkillMenu` static function before dispatch, and must
   return false without a crash when CSF is absent.
4. Prove the native lifecycle event ordering. `kNewGame`/`kPostLoadGame` normally
   have a player, but the current code only logs and returns if the player is
   unavailable. Add a bounded retry seam or evidence that both supported event
   paths cannot miss initialization.
5. Verify dual-toggle sound identity. The original Papyrus properties referenced
   Skyrim forms `0362B6` (on) and `03966B` (off); the WIP uses EditorID strings
   `MAGCloakIn`/`MAGCloakOut`. Resolve the forms/EditorIDs and use the exact same
   descriptors instead of assuming equivalence.
6. Fix the profile loader's existing missing call:
   `GameData::toggle_individual_shout_cooldowns;` in
   `storage/user_data_io.cpp` generated MSVC warning C4551. It must invoke the
   function so profile load preserves the saved setting.
7. Decide the minimum host version from the actual public release. The isolated
   host currently requires SMF `3.14`; the older tracker recorded
   `3.13-Hotfix2`. Verify installed/available runtime and align code, README, and
   package dependency rather than silently selecting either.
8. Correct the WIP README's guessed SMF URL to the QTR-Modding runtime/API links
   listed in `00-master-handoff.md`.

The broad stale-state finding was already fixed in `5b62191`: load/revert rebuild
bars and defaults, clear keybinds/held state, clear co-save-owned GameData, reset
input mode, and reset lifecycle state. Retest it after conflict resolution.

## Step 3: make the integrated boundary mechanically green

Run from a VS2022 x64 environment:

```powershell
Push-Location skse_plugin
cmake --preset release
cmake --build build/release --config Release
Pop-Location
cmake -S skse_plugin/tests -B skse_plugin/build/smf-unit-tests
cmake --build skse_plugin/build/smf-unit-tests --config Release
ctest --test-dir skse_plugin/build/smf-unit-tests --output-on-failure
python python_scripts/build_plugins.py
python python_scripts/verify_smf_guest_boundary.py
python python_scripts/verify_smf_migration.py
```

The migration verifier may still fail only for the intentionally absent MCP files
at the end of this phase. Every host/input/persistence/plugin-source check must be
green. Commit the integrated foundation, leave the worktree clean, and record the
commit in `00-master-handoff.md` before Phase 2.
