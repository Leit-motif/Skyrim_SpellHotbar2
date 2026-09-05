# Phase 2 — implement all seven Mod Control Panel pages

Start only from the clean integrated Phase 1 commit. The SkyUI MCM is already
retired in the paused branch, so this phase is a release blocker, not optional
polish.

## Source of truth

- Former UI/behavior inventory:
  `git show upstream/master:papyrus/Scripts/Source/SpellHotbarMCM.psc`.
- Existing native seam inventory:
  `papyrus/Scripts/Source/SpellHotbar.psc` and
  `skse_plugin/src/papyrus_extensions/papyrus_functions.cpp`.
- Domain state stays in `Bars`, `Input`, `GameData`, `Storage::IO`, `Lifecycle`,
  and `RenderManager`; MCP callbacks are views/adapters, not a second store.
- Use the vendored SMF/ImGuiMCP API. Do not link private Dear ImGui.

Add `skse_plugin/src/mcp/mcp_pages.h/.cpp` and register the seven section items
from `SmfGuest::install` only after the host is ready.

## Page contracts

### 1. Keybinds

Expose all upstream key IDs `0..22`: twelve slots, next/previous menu bar, three
modifiers, dual-cast modifier, show-bar modifier, Oblivion cast, Oblivion potion,
Oblivion show-bar modifier, and advanced bind-menu opener.

`Rebind` arms one pending key ID. The next keyboard, mouse, or gamepad ButtonEvent
on the down edge is translated with the existing DX scan-code mapping, passed to
`Input::rebind_key`, consumed, and disarms capture. `Unmap` writes `-1`. Escape
cancels capture without changing the bind. Show the current localized key name.
Only one pending capture may exist.

### 2. Settings

Cover every control created by the old MCM Settings page, including:

- input mode; slot count; layout; row length; circle radius; cross distance;
- main scale, spacing, resolution-scaled offsets, anchor, text mode, and show mode;
- Vampire Lord and werewolf show modes;
- default bar when sheathed; menu rendering; menu binding; non-modifier bar;
- key-icon preference; potion GCD; individual shout cooldown policy;
- Oblivion scale, spacing, offsets, anchor, show mode, held-show time, vertical
  orientation, and power visibility.

Use the existing clamping/scaling rules and mutate the same globals the gameplay
and serializer already consume.

### 3. Bars

Expose enable and inherit mode for the 15 configurable bars from
`Bars::bar_cycle`/the old MCM. The non-sneak main bar remains mandatory. Preserve
the exact inherit values and parent relationships; do not create a parallel bar
model.

### 4. Perks

Expose the existing BattleMage globals: override perk requirements, half-cost
perk requirement, timed-block window, block/power-attack/sneak/crit chances, and
proc cooldown. Use the old MCM ranges/defaults. Show an `Open BattleMage tree`
button only when the optional plugin is present; call
`Lifecycle::open_battlemage_tree` and report unavailable CSF without dispatching
or crashing.

### 5. Presets

List, save, and load configuration presets, bar presets, and icon-edit presets
using the existing mod-directory/user-directory precedence and formats. Preserve
filename validation, overwrite confirmation, and error feedback. Loading config
must update the page-visible state immediately. `auto_profile.json` and
`auto_edits.json` remain native first-run behavior, not a new page store.

### 6. Spells

Provide buttons to open the existing spell editor and potion editor through
`SmfGuest::open_window`. Window mutual exclusion, close synchronization, filtering,
editing, reset behavior, and save-specific co-save data remain those of the
existing editors.

### 7. Util

Expose reload resources, reload spell data, clear bars, drag the main bar, drag
the Oblivion bar, and add/remove the unbind and dual-cast powers. Use
`Lifecycle::player_has_power`/`toggle_player_power`; do not restore Papyrus init
quests.

## Hard scope boundary

Upstream has no Ability page/editor, Weapon Art page/editor, addon record, Driver
Cast setting, fork `castSlot`, or co-save format `6`. None may appear in code,
docs, package, screenshots, or PR text.

## Verification

Add focused, non-ImGui contract tests for bind-capture state and any extracted
page/domain adapters. Extend `verify_smf_migration.py` to require the seven page
registrations and to fail on MCM/script/package remnants. Then run the full DLL
build, all CTests, plugin round-trip build, and both static boundary verifiers.

Completion criterion: every former upstream MCM operation is reachable through
one of the seven pages, all four native windows open through SMF, keyboard/mouse/
gamepad bind capture is wired to the single SMF input callback, and every static
and build check passes. Commit and leave a clean tree before Phase 3.
