# 02 — Configure the mod from the Mod Control Panel

**What to build:** The player can change every former MCM setting from the Mod Control Panel: keybinds (including unmap and gamepad capture), input mode and HUD/layout/Oblivion options, per-bar enable/inherit, BattleMage globals plus a button that opens the BattleMage custom skill tree (no auto-granted opener power), preset save/load (JSON format 2), Util (reload resources/data, clear bars, start bar drag, add/remove unbind and dual-cast), and addon-only Spell GCD. Those writes hit the same SH2 settings the HUD and Direct Cast already consume, and they persist in the format-7 co-save and JSON presets. SKSE grants unbind/dual-cast on new game / first init and loads `auto_profile.json` without the MCM. Pages inherit the Mod Control Panel Skyrim theme and are usable with framework gamepad nav.

**Blocked by:** 01 — Become an SMF guest

**Status:** claimed — development-line pages exist on `ng/smf-next`; runtime acceptance open

**Status (superseded — see the top):** deferred — post-release. Owner ruling 2026-08-29.

Development line: eight MCP sections plus Spell GCD slider. Ability/Weapon Art *editors* are ticket 03/04 windows, not extra MCM pages.

- [ ] Every former MCM page has an MCP equivalent; changing a control changes live behaviour or persisted state the same way the MCM did.
- [ ] Spell GCD on Settings writes `GameData::spell_gcd` and survives a format-7 save/load.
- [ ] Keybind capture works for keyboard and native gamepad; unmap works.
- [ ] Perks sliders write the existing ESP globals; Open BattleMage tree uses Custom Skills' native; the opener power is not auto-granted.
- [ ] Preset save/load round-trips config, bars, and icon edits in JSON format 2, including `settings.spell_gcd`.
- [ ] `auto_profile.json` applies on new game / first init without opening MCM.
- [ ] Unbind and dual-cast spells can be granted from SKSE init and toggled from Util.
- [ ] Hidden natives still read the same values MCP just wrote.
