# 02 — Configure the mod from the Mod Control Panel

**What to build:** The player can change every former MCM setting from the Mod Control Panel: keybinds (including unmap and gamepad capture), input mode and HUD/layout/Oblivion options, per-bar enable/inherit, BattleMage globals plus a button that opens the BattleMage custom skill tree (no auto-granted opener power), preset save/load, Util (reload resources/data, clear bars, start bar drag, add/remove unbind and dual-cast). Those writes hit the same SH2 settings the HUD and Direct Cast already consume, and they persist in the co-save and JSON presets. SKSE grants unbind/dual-cast on new game / first init and loads `auto_profile.json` without the MCM. Pages inherit the Mod Control Panel Skyrim theme and are usable with framework gamepad nav. SkyUI MCM may still be present until ticket 05.

**Blocked by:** 01 — Become an SMF guest

**Status:** ready-for-agent

- [ ] Every former MCM page has an MCP equivalent; changing a control changes live behaviour or persisted state the same way the MCM did.
- [ ] Keybind capture works for keyboard and native gamepad; unmap works.
- [ ] Perks sliders write the existing ESP globals; Open BattleMage tree uses Custom Skills' native; the opener power is not auto-granted.
- [ ] Preset save/load round-trips config, bars, and icon edits.
- [ ] `auto_profile.json` applies on new game / first init without opening MCM.
- [ ] Unbind and dual-cast spells can be granted from SKSE init and toggled from Util.
- [ ] Hidden natives still read the same values MCP just wrote.
