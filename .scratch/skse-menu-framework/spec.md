# SKSE Menu Framework as Spell Hotbar 2's ImGui host

Status: ready-for-agent

Created 2026-08-20. Research: `research-skse-menu-framework.md`. Decision: ADR-0012 (supersedes ADR-0003).

## Problem Statement

The player configures Spell Hotbar 2 through a SkyUI MCM while the plugin also runs its own Dear ImGui overlay for the HUD, editors, and bind menu. That is two settings languages, a Papyrus façade over natives that already live in C++, and a second ImGui host in a load order that already runs SKSE Menu Framework. Two hosts both create a context and both patch the same input dispatch, which is slower and unsafe. ADR-0003 froze that shape; it is outdated for this Personal Integration.

## Solution

SKSE Menu Framework becomes the ImGui Host. Spell Hotbar 2 becomes a guest: Mod Control Panel pages replace the MCM; editors, bar drag, and the bind menu become framework windows; the on-screen bar stays this mod's widgets, composited through the framework's HUD callback (ticket 01 may change only the call site). Gameplay — Direct Cast, Driver Cast, bars, co-save, JSON presets, Magic Menu bind arbitration — stays in the SKSE plugin. MCM is gone. Papyrus shrinks to hidden test natives plus whatever foreign APIs still require a script; init `AddSpell` and BattleMage tree-open move to SKSE. Function of the shipped product stays 100%. Aesthetic UX of settings may follow the Mod Control Panel's Skyrim theme (Pareto).

## User Stories

1. As a player, I want to open Spell Hotbar 2 settings from the Mod Control Panel, so that I am not hunting a SkyUI MCM.
2. As a player, I want every former MCM control to exist on those pages, so that I do not lose a setting.
3. As a player on a gamepad, I want to navigate those pages with the framework's gamepad nav, so that Dual-Input Compatibility includes configuration.
4. As a player, I want to rebind all hotbar actions including modifiers and Oblivion-mode keys, so that my pad or keyboard layout still drives Direct Cast.
5. As a player on a gamepad, I want bind capture to accept gamepad buttons, so that I can set those actions without a keyboard.
6. As a player, I want unmapping a bind to still work, so that I can clear a conflict.
7. As a player, I want input mode, HUD show rules, layout, anchors, scales, offsets, potion GCD, shout cooldown policy, and Oblivion-bar options on a Settings page, so that the bar still matches how I play.
8. As a player, I want per-bar enable and inherit modes on a Bars page, so that sneak/melee/magic bars still behave as before.
9. As a player, I want BattleMage proc sliders and perk-requirement toggles on a Perks page that write the same ESP globals, so that procs do not change meaning.
10. As a player with BattleMage installed, I want a Perks-page button that opens the BattleMage custom skill tree, so that I do not need a lesser power or tween as the only door.
11. As a player, I do not want that BattleMage opener power auto-granted, so that my Magic Menu is not cluttered for a CSF shim this list already replaces.
12. As a player, I want to save and load config, bar, and icon-edit presets from a Presets page, so that my JSON workflows survive the MCM.
13. As a player, I want `auto_profile.json` to still apply on new game / first init, so that the Installed Configuration's controller bind-menu preset still loads without MCM.
14. As a player, I want to open the spell editor from the Mod Control Panel without closing SkyUI first, so that editing icons is one step.
15. As a player, I want to open the potion editor the same way, so that potion icons stay editable.
16. As a player, I want those editors to list, filter, edit, and reset entries as they do today, so that save-specific icon data is unchanged.
17. As a player, I want to drag the main bar and the Oblivion bar from a window or Util action, so that live positioning remains.
18. As a player, I want reload resources, reload spell data, and clear bars on Util, so that pack authors and I can recover without a restart guess.
19. As a player, I want to add or remove the unbind and dual-cast spells from Util, so that those gameplay powers remain optional after init grants them from SKSE.
20. As a player, I want the advanced bind menu available on the same keybind while Magic Menu or a supported inventory tab is open, so that slotting does not fight vanilla menu keys.
21. As a player, I want that bind menu to swallow input the way it does today, so that Dual-Input Compatibility does not regress into menu conflicts.
22. As a player, I want the HUD bar to look like Spell Hotbar 2 (slots, icons, cooldowns, fades, layouts), so that combat chrome is 100% function, not a restyle experiment.
23. As a player, I want that HUD to draw every gameplay frame without capturing input, so that the bar never steals WASD or camera.
24. As a player, I want settings pages, editors, and the bind menu to inherit the Mod Control Panel Skyrim theme, so that configuration feels like the rest of the native menu stack.
25. As a player, I do not want a SkyUI Spell Hotbar entry after cutover, so that there are not two settings surfaces.
26. As a player, I want Direct Cast, Equip, and Oblivion modes to keep working after the input trampoline moves, so that hosting changes do not change combat.
27. As a player, I want Driver Cast, Ability slots, and existing co-save format 6 to load unchanged, so that this is not a new save format.
28. As an agent, I want hidden `SpellHotbar` natives (`castSlot`, load/save bars, get/set binds) to remain, so that Compatibility Evidence does not lose its Papyrus seam.
29. As a maintainer, I want SKSE Menu Framework treated as a hard requirement, so that we do not ship a second UI stack.
30. As a maintainer, I want SH2 to register only after detecting the framework, so that a missing DLL fails closed with a log rather than a second `CreateContext`.
31. As a maintainer, I want SH2 to stop writing the input-dispatch trampoline, so that only the framework owns that call site.
32. As a maintainer, I want the existing filter policy (casts, modifiers, Magic Menu gate, editor capture) to run as a framework input callback that can strip events, so that behaviour stays SH2's and the hook is not.
33. As a maintainer, I want editors ported to the framework's ImGui bindings, so that widgets mutate the host context, not a private one.
34. As a maintainer, I want SH2 texture atlases reused as the HUD's images, so that we do not reauthor slot art through framework LoadTexture unless a spike proves we must.
35. As a maintainer, I want quest init that only adds unbind/dual-cast spells and loads auto-profile moved into SKSE, so that those quest scripts die.
36. As a maintainer, I want BattleMage init and opener Papyrus gone, so that CSF is invoked from the Perks page, not a magic effect.
37. As a maintainer, I want dual-cast toggle to flip the same global from SKSE when that spell is used, so that the effect script can die if the spell still exists.
38. As a player under ENB / Display Tweaks, I want the HUD still visible and correctly scaled, so that ticket 01's call-site choice is judged on frames, not theory.
39. As a player, I want Freeze/blur of the Mod Control Panel to follow the framework's own settings, so that we do not invent a second pause policy for config pages.
40. As a player, I want independent editor windows to pause or not according to whether they should block the world, matching today's "editor open captures keys" vs HUD never capturing.

## Implementation Decisions

- **ADR-0012** is the host decision. Do not keep ADR-0003's "own ImGui interface" as live policy.
- **One ImGui Host.** Guest registration: section items (former MCM pages), windows (spell editor, potion editor, bind menu, bar drag), HUD element (the bar), input events (the current filter pipeline). No SH2 `CreateContext`, D3D init UI hook, Present UI hook, or WndProc for ImGui, except the ticket 01 escape hatch: Present **using the host context only** if MenuManager misses gameplay frames.
- **Do not** register MCP pages while SH2 still hosts ImGui.
- **Hard dependency** on SKSE Menu Framework for this Installed Configuration. No SkyUI MCM fallback.
- **Domain stays SH2:** settings objects, keybinds, bars, GameData, Storage co-save format 6, JSON presets, Direct Cast / input modes, Magic Menu / favorites / inventory bind gate.
- **View is the adapter:** ImGui callbacks display state and issue the same operations the Papyrus natives already wrap. Do not invent a parallel settings store.
- **Guest widgets** use the framework's ImGui API (`ImGuiMCP`), not a vcpkg imgui link.
- **Input trampoline:** SH2 stops patching the shared dispatch. The existing `processAndFilter` policy moves onto `AddInputEvent` (strip on true). Cast logic is not moved into the framework product.
- **HUD:** SH2 bar widgets via `AddHudElement`. Ticket 01 measures MenuManager vs Present-on-host-context. There is no SMF-native hotbar to switch to.
- **Theme:** MCP, editors, bind menu inherit framework Skyrim theme. HUD keeps SH2 slot textures, spacing, and layout.
- **BattleMage:** globals stay. No auto-granted opener power. Perks page opens the tree through Custom Skills' `OpenCustomSkillMenu` (CSF 2 and 3 already expose that native). CSF 2 vs 3 **data** files stay FOMOD / Compatibility Package.
- **Papyrus:** remove MCM and BattleMage quest/effect opener scripts from the Installed Configuration. Move init `AddSpell` (unbind, dual-cast) and `auto_profile.json` load into SKSE. Keep hidden `SpellHotbar` natives. Dual-cast toggle: SKSE flips the existing global when that spell fires; drop the effect script if the spell can persist without it.
- **Version detect:** do not trust `GetMenuFrameworkVersion()` (stale 3.7f in 3.13 source). Use `IsInstalled()` / module handle.
- **Input export:** the framework's register name is the misspelled host export; call it exactly.
- **Ownership (ADR-0001):** guest adapter and MCM retirement are Core Fork. Nolvus-only CSF data layout stays Compatibility Package.

## Testing Decisions

A good test asserts externally visible behaviour: a setting changed in the Mod Control Panel is the value Direct Cast / HUD already consume; a bind captured on keyboard and on gamepad is the bind `getKeyBind` returns; a slot press still goes through the existing Papyrus slot-activation native; the HUD is a **captured frame**, not a log line that a callback ran. Do not assert ImGui tree internals or trampoline addresses.

**Seams — prefer one existing, add at most one host seam.**

- **Existing (keep):** hidden `SpellHotbar` natives (`castSlot`, load/save bars and config, get/set binds). They already drive the same dispatch a key uses. Hosting must not break this seam; it is Compatibility Evidence for combat.
- **Existing (keep):** SKSE co-save format 6 and JSON preset round-trip. MCP writes must be visible after save/load the same way MCM writes were.
- **Existing (keep):** Dual-Input Compatibility cells — keyboard Direct Cast and native gamepad Direct Cast, including reWASD mappings recorded per mapping.
- **New, only if ticket 01 needs it:** HUD liveness under ENB / Display Tweaks — frames with the bar visible in gameplay, inventory, and Magic Menu overlay. That seam is visual, not a new native.

Prior art: mco-integration and baseline-adoption tickets (Papyrus `castSlot`, co-save, frames, `SpellHotbar2.log`). Editor behaviour can be owner-eyeballed on MCP the way it was on the old overlay.

Ticket 01 is the HUD call-site spike: if MenuManager misses frames, switch to Present-on-host-context and record Compatibility Evidence; do not restore a second `CreateContext`.

## Out of Scope

- Prisma UI, HTML/CSS settings, or a SpellHotbar Studio app.
- Public / SkyUI-only lists without SKSE Menu Framework.
- Changing Direct Cast, Driver Cast, Ability, or co-save meaning.
- Upgrading BattleMage from CSF 2 data to CSF 3 data, or making CSF 3 the Installed Configuration.
- Pixel-matching the old `StyleColorsDark` editors.
- Shipping a second hotbar renderer "in case SMF has a better HUD."
- Rewriting houseCARL, Nemesis, or OAR packs as part of this effort.
- Amending ADR-0001/0002 (Core Fork vs Personal Integration stays).

## Further Notes

- This list already has SKSE Menu Framework 3.13-Hotfix2. Default MCP toggle in this profile is F1 / LB double-press — do not steal those for SH2 without documenting the conflict.
- Tickets: `.scratch/skse-menu-framework/issues/` (`01` guest host, `02` MCP pages, `03` editor windows, `04` bind-menu window, `05` retire MCM). Frontier is `01`. `02`–`04` are parallel after `01`; `05` waits for all three.
- Do not cut MCM until MCP pages cover the seven SkyUI pages and Dual-Input bind capture is proven.

## 2026-08-23 — two tracks: upstream PR and addon

Owner ruling, after pWn3d1337's 2026-08-12 reply on issue #85 ("release it as addon…
feel free to create a pull request"): the SMF cutover is offered **upstream** as a PR to the
main mod, separate from the addon. The owner asked him directly on 2026-08-23
(<https://github.com/pWn3d1337/Skyrim_SpellHotbar2/issues/85#issuecomment-5388589316>):
hard cutover, SMF becomes a requirement, MCM removed — no dual-config option, deliberately,
since upstream is in maintenance mode (last release 2025-06-09).

Consequences for this tracker, pending his answer:

- **The PR is built on his codebase, not this fork.** Tickets 02–05 as written assume fork
  surfaces (Ability bind menu, Ability Editor) that do not exist upstream. When the PR track
  activates, rescope: upstream PR = his MCM controls → Mod Control Panel pages, his
  spell/potion editors, bar drag, his bind menu; the fork's extra windows stay addon-side.
- **The addon does not wait.** v1 ships against current SH2 with the MCM as-is (ticket 05 is
  not started; his MCM works against the fork DLL). This effort is v1.1/upstream work on its
  own clock.
- **If he declines or never answers**, the effort reverts to fork-side as originally specced;
  nothing in the addon path blocks on it. The `SpellHotbar_ArtSelector` question in the same
  comment has its own fallback: the one-global addon ESP from
  `../mco-integration/notes/26-addon-publish-shape.md`.
