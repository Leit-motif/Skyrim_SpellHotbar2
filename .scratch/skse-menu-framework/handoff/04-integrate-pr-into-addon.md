# Integrate PR #86 into the addon development line

## Objective

Resume addon development immediately with the completed SKSE Menu Framework work from upstream
PR #86. The addon already overwrites `SKSE/Plugins/SpellHotbar2.dll`, so it must carry one combined
implementation: upstream Spell Hotbar 2, the SMF migration, and the addon's existing gameplay
extensions.

The upstream contribution and addon remain separate release lines:

- `codex/smf-migration` is the upstream-only PR branch. Change it only for upstream review.
- `ng/smf-next` is the addon development branch. It may contain the PR plus addon-only behavior.
- PR #86 remains free of Ability, Weapon Art, Direct Cast, ShoutMCO, format-7, branding, and other
  addon-only changes.

This is a Build task. Do not merge `ng/smf-next` into stable `origin/master` until runtime
acceptance is complete.

## Canonical coordinates

| Role | Coordinate |
|---|---|
| Canonical addon checkout | `C:\Nolvus\Projects\spell-hotbar-2` |
| Current addon stable base | `origin/master` at `9186eeece8a1d0f3fc3577811ed63200461752c5` |
| Upstream-only PR worktree | `C:\Nolvus\Projects\spell-hotbar-2-smf-upstream` |
| Upstream-only PR branch | `codex/smf-migration` at `4131cf01c713d4ef8177e00c26117fff3d873398` |
| PR | <https://github.com/pWn3d1337/Skyrim_SpellHotbar2/pull/86> |
| New addon worktree | `C:\Nolvus\Projects\spell-hotbar-2-smf-addon` |
| New addon branch | `ng/smf-next` |

Read the repository `AGENTS.md`, `CONTEXT.md`, `docs/agents/release-packaging.md`, and this file
before acting. The handoff directory is currently untracked in the canonical checkout, so read
this file from its absolute path before creating or switching worktrees.

## Product and runtime invariant

The public addon may later receive a different product name. That naming decision is outside this
integration. Preserve the established technical identity during the merge:

- exactly one winning `SpellHotbar2.dll`;
- Papyrus class `SpellHotbar`;
- serialization unique id `0xB8498471`;
- existing ESP names, FormIDs, record meanings, paths, and configuration filenames;
- SMF is the only ImGui host, and Spell Hotbar 2 registers as one guest.

History may temporarily contain SMF-equivalent commits on both the downstream and upstream sides.
The shipped implementation may not contain duplicate hosts, guest registration, input callbacks,
render callbacks, lifecycle bootstrap, or configuration pages.

## Phase 1: preflight and isolate the work

1. Confirm the canonical checkout is still at `9186eee`, `codex/smf-migration` is still at
   `4131cf0`, `ng/smf-next` does not exist, and the proposed worktree path is unused.
2. Record the canonical checkout's status. Preserve all existing untracked `.scratch` material;
   do not add, remove, move, or commit it as part of this work.
3. Confirm no other writer owns the proposed branch or worktree.
4. Create the new worktree and branch from the exact addon base:

   ```powershell
   git worktree add -b ng/smf-next C:\Nolvus\Projects\spell-hotbar-2-smf-addon 9186eeece8a1d0f3fc3577811ed63200461752c5
   ```

5. In the new worktree, confirm `HEAD` is `9186eee` and push the empty branch as a recoverable
   remote line before beginning the merge.

Completion criterion: the canonical checkout and all pre-existing worktrees are untouched;
`C:\Nolvus\Projects\spell-hotbar-2-smf-addon` is clean on `ng/smf-next` at `9186eee`; the remote
branch exists.

## Phase 2: merge the PR exactly once

Merge the complete seven-commit PR branch with ancestry preserved:

```powershell
git merge --no-ff codex/smf-migration
```

Do not replace this with selected file copies, a squash, or a partial cherry-pick. Resolve every
conflict deliberately against the invariants below. Inspect all shared files, including files Git
merges automatically; a clean textual merge does not prove correct behavior.

### Addon invariants that win conflicts

- SKSE co-save writer remains format `7`.
- Loading older supported formats remains intact; format 6 holds Ability slots and format 7 adds
  `GameData::spell_gcd`.
- JSON preset format remains `2`.
- Ability slots, Ability catalogues/editor, Custom Abilities, Ability costs/cooldowns, and Ability
  Selector behavior survive.
- Weapon Arts, their binds, icons, OAR content, and `WART` persistence survive.
- Direct Cast, Driver Cast, Cast Channel, ShoutMCO cast-intent integration, combo continuity, and
  commitment behavior survive.
- Keyboard, mouse, gamepad, and existing bind arbitration survive.
- Addon package identity, overwrite classification, and additive assets survive.

### SMF invariants that win conflicts

- SMF owns the ImGui context, render hooks, texture loading, and input trampoline.
- Spell Hotbar 2 registers one HUD callback, one input policy, its MCP pages, and its native
  windows through SMF.
- The retired private ImGui/D3D11/DirectXTK host does not return.
- MCP pages replace the retired SkyUI MCM surfaces. Addon-only controls and editors must also be
  reachable through the resulting SMF interface before MCM removal is considered complete.
- Native lifecycle initialization replaces the retired MCM/init Papyrus bootstrap without
  dropping addon initialization.
- SMF remains a separately installed hard requirement and is not redistributed.
- Missing-host behavior remains fail-closed by code and static verification. Do not disable SMF
  in the live profile to test this; the owner ruled that live test out of scope on 2026-08-31.

### High-risk shared files

Start the reconciliation review with the files changed on both lines:

- `papyrus/Scripts/Source/SpellHotbarMCM.psc`
- `papyrus/Scripts/SpellHotbar.pex`
- `skse_plugin/CMakeLists.txt`
- `skse_plugin/src/bar/hotbar.{h,cpp}` and `hotbars.cpp`
- `skse_plugin/src/events/gameloop_hook.cpp`
- `skse_plugin/src/game_data/game_data.{h,cpp}`
- `skse_plugin/src/input/input.{h,cpp}`
- `skse_plugin/src/plugin.cpp`
- `skse_plugin/src/rendering/advanced_bind_menu.cpp`
- `skse_plugin/src/rendering/render_manager.{h,cpp}`
- `skse_plugin/src/rendering/texture_loader.cpp`
- `skse_plugin/src/storage/storage.{h,cpp}` and `user_data_io.cpp`

Completion criterion: the merge commit has both `9186eee` and `4131cf0` in its ancestry; no
unmerged paths remain; searches and inspection show one SMF guest implementation and no restored
private host; every addon and SMF invariant above is accounted for in code or an explicit open
acceptance cell.

## Phase 3: reconcile the tracker and encode the boundary

Update `.scratch/skse-menu-framework/spec.md` and issues `01` through `05` on `ng/smf-next` to
describe the combined addon line rather than the upstream-only PR baseline:

- replace stale co-save format `6` assertions with format `7` plus backward-load acceptance;
- add addon MCP coverage for Ability/Weapon Art and other fork-only surfaces;
- retain preset format `2`;
- keep MCM retirement last, after every configuration/editor surface has an SMF replacement;
- distinguish development-line acceptance from release/runtime acceptance.

Add one concise branch-policy note to the new branch stating:

> Integrate the PR branch exactly once; never reapply equivalent changes. Upstream review changes
> land on `codex/smf-migration` first and are then merged into `ng/smf-next`.

Do not edit the upstream PR description, upstream-only handoff, or PR branch to mention addon-only
behavior.

Completion criterion: tracker claims match the combined code, format 7 is named consistently,
and the branch policy contains the future-upstream decision tree below.

## Phase 4: development validation

Discover the current build and test commands from repository configuration rather than copying
stale build paths from the upstream handoff. Run one complete development gate:

1. Configure and build the Release DLL.
2. Run every CTest target, including existing addon tests and the imported SMF contract tests.
3. Run `python_scripts/verify_smf_guest_boundary.py`.
4. Run the SMF migration/UI verifier after adapting its assertions to the combined addon surface.
5. Build the ESPs from repository-owned source where required and verify their round trip.
6. Run `python_scripts/build_mod_release.py` and inspect the archive plus manifest.
7. Verify the archive contains one `SpellHotbar2.dll`, required addon additions, and no private
   UI-host binaries, SMF host DLL, development paths, or upstream-only package assumptions.
8. Add a focused persistence regression test where practical: a format-7 record must not be
   written as format 5, and the format-6/7 field ordering must remain compatible.

Fix failures on `ng/smf-next`, rerun the smallest relevant check during iteration, then run one
complete final gate. Commit coherent fixes rather than verification checkpoints and push the
reviewable branch.

Completion criterion: clean checkout; Release build, all tests, boundary checks, plugin checks,
and package build pass; the produced archive contains one combined implementation. At this point
call `ng/smf-next` the addon development line, not release-ready.

## Phase 5: runtime acceptance before stable merge

Read `docs/agents/headless-testing-playbook.md` and the `skyrim-agent` skill before deployment.
Preview the exact operation and confirm the active MO2 instance, profile, and exclusive runtime
ownership. Keep the test deployment separate from stable addon outputs.

Acceptance requires evidence for the combined addon, not reuse of the upstream PR's format-5 run:

- startup with SMF and exactly one guest registration/input callback/HUD callback;
- visible HUD frame;
- MCP pages, including addon-only settings and editors;
- keyboard and gamepad binding plus activation;
- Magic Menu/inventory assignment to a hotbar slot;
- Direct Cast and representative Driver Cast/Channel behavior;
- an Ability and a Weapon Art fired from bound hotbar slots;
- editor operation and bar drag;
- save/reload of a format-7 addon save preserving hotbars, binds, Abilities, Weapon Arts,
  cooldown/GCD data, presets, and `auto_profile.json` behavior;
- clean shutdown and logs without duplicate registration or retired-host activity.

Do not run the missing-SMF live cell. Static fail-closed verification is the accepted evidence for
that branch.

Completion criterion: runtime evidence is tied to the tested commit, DLL, archive, MO2 instance,
profile, save, logs, telemetry, and visible frame. Only then may the addon stable-branch merge be
proposed.

## Future upstream decision tree

Use this whenever PR #86 changes state:

1. **PR remains unmerged:** continue addon development normally on `ng/smf-next`.
2. **PR receives review fixes:** implement upstream-appropriate fixes on
   `codex/smf-migration`, validate that branch against upstream scope, then merge its new head once
   into `ng/smf-next`. Resolve only addon-line differences there.
3. **PR merges preserving ancestry:** fetch upstream and merge the new `upstream/master` into the
   addon development line. Git should recognize the seven PR commits as ancestors. Verify that the
   final DLL still has one SMF implementation and retains addon behavior.
4. **PR is squashed or rebased:** fetch upstream, compare patch IDs and semantic file changes, and
   merge/rebase deliberately. Treat the accepted upstream implementation as the new baseline,
   retain only addon-specific deltas, and verify one guest/host/input/render/lifecycle path.
   Duplicate commit history is acceptable; duplicate runtime implementation is not.
5. **PR is partially accepted or substantially revised:** create a reconciliation matrix mapping
   every PR concern to upstream, addon-only, replaced, or still carried downstream. Resolve the
   final source to one implementation before updating the baseline.
6. **PR is rejected or abandoned:** no addon rollback is required. The addon line already owns the
   integrated SMF implementation.

The standing rule is: **carry the SMF migration downstream until upstream contains an equivalent
implementation; integrate it exactly once into each addon lineage and ship exactly one SMF guest.**

## Final handoff

Report:

- branch and merge commit;
- both merge parents and proof of ancestry;
- conflicts and how each invariant was preserved;
- tracker/spec changes;
- exact validation commands and results;
- package manifest summary;
- runtime cells completed and still open;
- upstream PR state and which decision-tree case currently applies.

Do not claim stable, deployed, or release-ready from static checks alone.

## Owner close — 2026-09-04

Owner: tested live, did not cover every cell, “feels like it works,” close the handoff for now.
`ng/smf-next` stays the addon development line. Remaining Phase 5 / ticket 01–05 runtime
checkboxes stay unchecked. Do not merge into `origin/master` from this close.
