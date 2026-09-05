# Phase 3 — package, runtime acceptance, cold review, and upstream PR

Start only from the clean Phase 2 commit with all static/build checks green.

## Package and documentation

1. Make `build_plugins.py` the required producer for both repository-owned ESPs
   in every release/FOMOD entry point. Keep the official hidden
   `papyrus/Scripts/SpellHotbar.pex`; exclude all six retired PEX files.
2. Add a FOMOD dependency check for an independently installed
   `SKSE/Plugins/SKSEMenuFramework.dll`. Never redistribute the host DLL.
3. Resolve fonts deliberately. SMF scans only
   `Data/SKSE/Plugins/Fonts`. The isolated host currently uses the SMF default
   font. To preserve SH2's custom title/text/symbol faces, package the selected
   `text_font*.ttf` and `skyrim_symbols_font.ttf` into that global Fonts folder
   and select their named faces through SMF; otherwise remove/reword obsolete
   font choices instead of installing unused files under `SpellHotbar/fonts`.
4. Audit `create_fomod_installer.py` and `build_release_package.py` from a clean
   checkout. They still inherit upstream machine-local `F:\Skyrim Dev` asset
   roots and hard-coded version strings; make the SMF PR reproducible enough for
   the maintainer and ensure every output path contains upstream assets only.
5. Update README requirements, MCP terminology, build instructions, SMF/API
   provenance/license, and save-compatibility statement. SkyUI may remain a
   separate player dependency only if another shipped upstream feature truly
   needs it; it is no longer the SH2 configuration host.
6. Inspect the final archive. It must contain the two rebuilt ESPs, current DLL,
   hidden native PEX, upstream assets, and correct font choice; it must not contain
   MCM/init/opener/toggle PEX files, private ImGui/DirectXTK binaries, SMF host DLL,
   addon files, or development paths.

## Runtime acceptance

Read the repository's `skyrim-agent` skill and
`docs/agents/headless-testing-playbook.md` before deployment. Preview the exact
MO2 operation and confirm current ownership/profile through the tool; keep the
test mod separate from addon outputs.

Owner ruling 2026-08-31: do **not** run a Missing-SMF load. SMF is a hard player
requirement; fail-closed-without-host is out of scope for runtime acceptance.
Every remaining cell assumes `SKSEMenuFramework.dll` is installed and loaded.

Record these cells against the named save/profile/log/frame:

| Cell | Required evidence |
|---|---|
| Missing SMF | **Out of scope** (owner). Do not disable the host to prove fail-closed. |
| Host startup | SMF present, SH2 HUD/windows/pages register once, no hook collision |
| Existing save | HOTB settings, binds, bars, icon edits, and preset meaning survive unchanged |
| New game | defaults reset; unbind/dual powers grant once; auto profile/edits apply once |
| Cross-save/new-game | no bars, held keys, user edits, cooldowns, or modes leak from prior save |
| MCP coverage | all seven pages and every contract in Phase 2 operate |
| Bind capture | keyboard and native gamepad binds set, unmap, cancel, and drive gameplay |
| Modes | Direct Cast, Equip, and Oblivion paths still work through SMF input |
| Windows | spell editor, potion editor, main/Oblivion drag, bind menu open/close and swallow input correctly |
| BattleMage absent | page remains safe; no dispatch or crash |
| BattleMage + CSF | page button opens the correct tree; legacy opener power is absent/removed |
| Persistence | page changes survive save/reload under co-save format `5`; JSON remains format `2` |
| Visual HUD | committed gameplay frame proves slots/icons/cooldowns/fades under the actual host |

Use DevBench's structured input/Papyrus seams where supported. A successful
callback log is not visual or gameplay proof. Leave Skyrim running only for an
explicit owner review; otherwise close it through `qqq` after agent-only evidence.

### Recorded 2026-08-31 (Nolvus Awakening, overlay `Dev - Spell Hotbar 2 SMF`)

Profile `Nolvus Awakening`. Winner: this overlay's `SpellHotbar2.dll` /
`SpellHotbar.esp` / `SpellHotbar_BattleMage.esp`. DevBench bound **8933**
(Windows excludes 8833–8932); Cursor MCP `user-devbench-se` on 8920 stayed
offline. Telemetry used the worktree `.skyrim-agent/devbench8933.py` client.
Owner quit Skyrim after the last cells; lease released.

| Cell | Result | Evidence |
|---|---|---|
| Missing SMF | Out of scope | Owner 2026-08-31 |
| Host startup | Pass | `SpellHotbar2.log`: HUD + 1 input + 4 windows + 7 MCP pages. Owner screenshot of all seven pages. |
| Existing save (`CS-Test`) | Fail vs later co-save | Last written as SKSE layout newer than 5. HOTB prefix survived (9 slots, scale 0.75). Keybinds all `-1`; bar deserialize garbage; unknown `WART` skipped. Not a 0.0.14 format-5 save. |
| Persistence format 5 | Pass | Saved `SH2SMFTelB` with slot 0 = DX 16; mutated to 30; reload restored 16. Clean load log. |
| New game | Pass | `Prisoner-Arthas` lv1 `WhiterunExterior15`. First init once: `auto profile: true, auto icon edits: false`. Binds/HUD match `auto_profile.json`. Powers 0 and 1 granted. |
| Cross-save leak | Pass | Marker bind 22=45 on `SH2NewGameLeak` did not appear on `CS-Test` (22 stayed -1). Reload new-game save restored 45 / 12 slots / 0.7. First init did not run again. Marker then unmapped on live character. |
| MCP coverage | Pass | Seven pages exist. Natives: unmap, rebind, mode 0↔1, slots 9↔7. |
| Bind capture | Partial | `SetKeyBind`/`GetKeyBind`/`-1` proven. Owner rebound in MCP; Direct Cast worked. SMF Rebind click not agent-clicked. |
| Modes | Partial | Direct Cast owner-pass after rebound. Equip / Oblivion not separately proven. |
| Windows | Partial | `OpenSpellEditor` / potion / drag invoked; no SMF-unavailable error. Engine menu list does not show ImGui. Native screenshots drop ImGui. |
| BattleMage absent | Not run | Plugin was loaded in this profile. |
| BattleMage + CSF | Pass | `IsBattlemageAvailable` true; `CustomSkills.OpenCustomSkillMenu("SpellHotbar_Battlemage")` opened `StatsMenu`. Log removed legacy opener power. |
| Visual HUD | Owner pass | Owner MCP screenshot (Fire Breath + greatsword). Native DevBench captures missed ImGui. No frame committed on the branch. |

Static (not runtime): CTest 4/4; `verify_smf_guest_boundary.py` PASS;
`verify_smf_migration.py` 10/10; ESPs round-tripped; dumpbin no D3D11/imgui/DirectXTK.

Post-runtime polish on the branch: MCP shows **Unbound** for keybind `-1`
instead of **Unknown Key**; README/UPSTREAM state that this guest does not
call `PushFont`. Do not commit local `skyrim-agent.json`, `stage_mo2_dist.py`,
or `.gitignore` `dist/` / `.skyrim-agent/` entries.

## Cold review and publication

Run the `code-review` skill against fixed base
`f203cd26747e875336caed91f7d1453ca9a8a808`. Review both the upstream request and
repository standards. Resolve every P0/P1/P2 finding, rerun one proportionate final
validation pass, and commit a coherent final branch.

Push explicitly to the contributor fork, never with an ambiguous plain push:

```powershell
git push origin codex/smf-migration
gh pr create --repo pWn3d1337/Skyrim_SpellHotbar2 `
  --base master `
  --head Leit-motif:codex/smf-migration
```

The PR should explain the hard SMF dependency, one-host architecture, complete
MCM-to-MCP parity, unchanged save/preset formats, native bootstrap replacement,
repository-owned ESP build, and exact static/runtime evidence. Maintainer issue
`#85` already accepted an SMF migration in principle and prioritized preserving
settings; link it. Do not mention or include the user's addon.

Completion criterion: the pushed head exactly matches the reviewed/tested commit,
the upstream PR URL resolves, and the PR body distinguishes static build proof
from the recorded runtime and frame evidence.
