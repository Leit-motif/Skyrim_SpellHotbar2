# Spell Hotbar 2 upstream SMF migration — master handoff

Status: PR 86 converted to draft 2026-08-31 after owner said publication
was premature. Magic Menu → hotbar slotting was never a recorded cell.
https://github.com/pWn3d1337/Skyrim_SpellHotbar2/pull/86 remains draft
until that path (and remaining gameplay cells) are observed. Do not mark
ready or mention the addon.

## Objective

Migrate upstream Spell Hotbar 2 `0.0.14` from its private Dear ImGui host plus
SkyUI MCM to an SKSE Menu Framework guest:

- SMF owns the ImGui context, rendering hooks, textures, and input trampoline.
- SH2 contributes its HUD, four existing native windows, one input policy callback,
  and seven Mod Control Panel pages.
- The SkyUI MCM and its Papyrus bootstrap/opener/effect scripts are retired only
  after their replacement pages work.
- Gameplay, upstream co-save format `5`, preset format `2`, records/FormIDs, and
  hidden upstream Papyrus natives retain their meaning.

## Canonical coordinates

| Purpose | Value |
|---|---|
| Upstream base | `pWn3d1337/Skyrim_SpellHotbar2` `master` at `f203cd26747e875336caed91f7d1453ca9a8a808` (`0.0.14`) |
| Contributor fork | `Leit-motif/Skyrim_SpellHotbar2` |
| Integration worktree | `C:\Nolvus\Projects\spell-hotbar-2-smf-upstream` |
| Integration branch | `codex/smf-migration` |
| Paused foundation commit | `5b6219166a1e51bd04c9608d2259979a6fc3dfea` |
| Phase 1 integrated host | `73af3db` on `codex/smf-migration` (cherry-pick `4e0c99e` + review findings) |
| SMF host commit to integrate | `79d5b4b52803cbfc9d2e21b85ebcd5ae0d84f205` on `codex/smf-ui-slice` |
| SMF host worktree | `C:\Nolvus\Projects\spell-hotbar-2-smf-ui` |
| Standalone input commit | `75ea901f0de44a00dda97550e3e2eb4bc3bfe8b8` on `codex/smf-input-slice` |

The paused foundation already contains the input commit's changes. Do not
cherry-pick `75ea901` again. Local MO2/skyrim-agent files in the integration
worktree are not part of the upstream PR.

Remotes in the integration worktree:

```text
origin   git@github.com:Leit-motif/Skyrim_SpellHotbar2.git
upstream git@github.com:pWn3d1337/Skyrim_SpellHotbar2.git
```

## What each commit contains

### `5b62191` — paused foundation

- SMF-shaped per-event input seam and three standalone contract tests.
- Native first-run/new-game bootstrap, settings-preserving HOTB detection,
  dual-cast toggle handling, and BattleMage tree dispatch guard.
- Full runtime-state reset before new game/save overlays, including held-key reset.
- Repository-owned Spriggit YAML for the 53-record main ESP and 4-record
  BattleMage ESP, Spriggit `0.40.1` pin, round-trip builder, and the official
  upstream hidden `SpellHotbar.pex` artifact.
- Removal of the six legacy Papyrus sources and their package entries.
- Partial release/FOMOD/README changes and an intentionally incomplete static
  verifier.

This commit is not a runnable cutover: the MCM is gone while the MCP pages are
not implemented, and the private renderer remains until the next commit is
integrated.

### `79d5b4b` — independently validated SMF host

- Vendored SMF consumer API and LGPL license.
- One SMF HUD callback and four mutually exclusive blocking windows: spell editor,
  potion editor, bar drag, and bind menu.
- Fail-closed SMF export/version checks, host/model close synchronization, Escape
  behavior, SMF texture loading/disposal, and removal of private ImGui/D3D/WndProc
  ownership plus private ImGui/DirectXTK dependencies.
- `OUTPUT_FOLDER` made opt-in and a focused host-boundary verifier.

It built cleanly in isolation. It deliberately has no MCP pages and used the old
input path mechanically; integration must join it to `Input::process_event` from
`5b62191`.

SMF provenance:

- Consumer API: <https://github.com/QTR-Modding/SKSE-Menu-Framework-3-API>,
  commit `1dcb70179076aae4ab626f43c5baab2735ca5877`.
- Header SHA-256:
  `48416E8220CA777E2FFFC2EF2BAF21F699AB2E6C409D437F44EEC5E311C3524C`.
- Runtime repo: <https://github.com/QTR-Modding/SKSE-Menu-Framework-3>,
  byte-identical header at commit
  `928e01ab459822a8d233ab99f0419ea1de23c775`.
- Runtime file is installed separately as
  `Data/SKSE/Plugins/SKSEMenuFramework.dll`; SH2 must not redistribute it.

## Required sequence

1. Execute `01-integrate-and-stabilize.md` in the integration worktree.
2. Execute `02-mcp-pages.md` only after the integrated full DLL builds and both
   SMF boundary checks pass.
3. Execute `03-package-runtime-and-pr.md` only after all seven pages exist and
   the legacy-surface verifier is green.

Each phase is its own completion boundary. Commit a phase only when its listed
checks pass. Keep all work on `codex/smf-migration`; use a separate worktree only
for a genuinely parallel writer, then serialize integration.

## Preserved evidence

At `5b62191` before pause:

- `ctest --test-dir skse_plugin/build/smf-unit-tests --output-on-failure`:
  3/3 passed (`input_event_adapter`, `lifecycle_policy`, `runtime_state_reset`).
- Full current-tree Release DLL built and linked after the final held-key reset.
- `python python_scripts/build_plugins.py`: both ESPs built and source-round-tripped.
- Python compile checks passed for the plugin builder, package builders, and static
  verifier.

At `79d5b4b` in its isolated worktree:

- Clean Release configure/build: 39/39, DLL linked.
- `python python_scripts/verify_smf_guest_boundary.py`: passed.
- DLL dependency inspection found no D3D11, DXGI, DirectXTK, or imgui DLL.

These are static/build results, not runtime acceptance.

## Authority and scope guardrails

- Work only from upstream `0.0.14`; the separate fork's save format `6`,
  `castSlot`, Driver Cast, Ability, and Weapon Art surfaces are out of scope.
- Preserve upstream save format `5` and preset format `2` exactly.
- Do not run LOOT or author into game `Data`.
- Deployment/profile changes require current ownership confirmation and preview.
- Launching/stopping Skyrim and agent-only DevBench telemetry are authorized after
  deployment ownership is established.
- Runtime acceptance assumes SMF is installed. Do not disable
  `SKSEMenuFramework.dll` or run the Missing-SMF fail-closed cell. Owner ruled
  that load out of scope on 2026-08-31.
- `../spec.md` and `../issues/` are useful feature inventories, but upstream
  baseline and this handoff win wherever they mention fork-only behavior or
  co-save format `6`.

## Whole-migration completion criterion

Opened as https://github.com/pWn3d1337/Skyrim_SpellHotbar2/pull/86; converted
to draft 2026-08-31. Blocking: Magic Menu / inventory bind-to-slot was not
in the recorded pass and the owner could not slot from the spell menu.
Do not mark the PR ready until that path is observed on a format-5 save
with bound slot keys. Other gaps: Missing-SMF not run; Equip and Oblivion
not separately proven; no committed HUD frame; guest does not call
`PushFont`.
