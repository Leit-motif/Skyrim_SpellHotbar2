# SMF addon line (`ng/smf-next`)

Read this before editing the UI host, Mod Control Panel, MCM retirement, or merging `ng/smf-next`.
ADR-0012 is the host decision. This file is the branch and merge procedure.

## Lines

| Line | Branch | Role |
|---|---|---|
| Stable addon | `origin/master` (`main` locally) | Shipped addon: SkyUI MCM + private ImGui host. Do not land SMF here yet. |
| Forward addon | `ng/smf-next` | Current mod development: upstream SH2 + the folded-in SMF guest + addon gameplay. |
| Upstream PR | `codex/smf-migration` | PR #86 only. No Ability, Weapon Art, Direct Cast, format 7, or addon branding. |

Worktrees:

- Canonical / forward addon: `C:\Nolvus\Projects\spell-hotbar-2`
- Frozen stable: `C:\Nolvus\Projects\spell-hotbar-2-stable`
- Upstream PR: `C:\Nolvus\Projects\spell-hotbar-2-smf-upstream`

If the stable worktree is missing: `git worktree add C:\Nolvus\Projects\spell-hotbar-2-stable main`.

## What to do

`ng/smf-next` is the forward development line for this mod. Put new addon features and fixes there
now that the SMF guest is folded in. This remains true while PR #86 is unmerged and if it never
merges: the addon ships its own `SpellHotbar2.dll`, which overwrites the upstream DLL. The canonical
checkout stays on `ng/smf-next`; use the stable worktree only to inspect or maintain the frozen
first-release line.

The PR merge already exists: `e19b273` (parents `9186eee` and `4131cf0`). Integrate `codex/smf-migration` **once** per addon lineage. Upstream review fixes land on `codex/smf-migration` first, then merge that new head once into `ng/smf-next`.

Owner 2026-09-04: partial live test, “feels like it works,” handoff closed for now. Tickets `01`–`05` on that branch stay **parked** with runtime checkboxes open. That close does **not** authorize a merge into `origin/master`. Propose the stable merge only after the Phase 5 cells in `.scratch/skse-menu-framework/handoff/04-integrate-pr-into-addon.md` have Compatibility Evidence.

Test overlay: `Dev - Spell Hotbar 2 SMF Next`. Leave `Dev - Spell Hotbar 2` (stable addon) and `Dev - Spell Hotbar 2 SMF` (upstream PR overlay) unwritten.

Missing-SMF live load stays out of scope; static fail-closed is the evidence.

## When PR #86 changes

1. **Still unmerged:** keep working on `ng/smf-next`.
2. **Review fixes:** implement on `codex/smf-migration`, then merge its new head once into `ng/smf-next`.
3. **Merges with ancestry:** fetch upstream and merge `upstream/master` into `ng/smf-next`. The seven PR commits should already be ancestors. Confirm one SMF guest and retained addon behavior.
4. **Squashed or rebased:** compare patch IDs, take upstream as the new baseline, keep only addon deltas. Duplicate history is fine; duplicate host/guest/input/render paths are not.
5. **Partial or rewritten:** map every concern to upstream, addon-only, replaced, or still carried. Resolve to one implementation before updating the baseline.
6. **Rejected or abandoned:** no addon rollback. This line already owns the guest.

Standing rule: carry the migration downstream until upstream has an equivalent implementation; integrate it once per addon lineage; ship one SMF guest.

Invariants on `ng/smf-next`: one `SpellHotbar2.dll`; Papyrus class `SpellHotbar`; unique id `0xB8498471`; co-save format 7 (format 6 still loads Ability slots); JSON presets format 2; SMF is a hard requirement and is not redistributed.
