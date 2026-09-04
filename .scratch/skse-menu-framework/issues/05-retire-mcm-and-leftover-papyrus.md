# 05 — Retire MCM and leftover Papyrus

**What to build:** After MCP pages, editors, and the bind menu cover the old surfaces, the Installed Configuration has no SkyUI Spell Hotbar MCM. BattleMage opener/init Papyrus is gone. Quest init that only added spells or loaded auto-profile is gone (already SKSE from 02). Dual-cast toggle is SKSE flipping the existing global if the spell remains; the effect script is gone. Hidden `SpellHotbar` natives remain as the test seam. The player has one settings door: the Mod Control Panel.

MCM retirement stays last in *acceptance*: do not call this ticket runtime-complete until tickets 02–04 have live evidence, including addon-only Spell GCD / Ability / Weapon Art surfaces. On `ng/smf-next` the MCM sources and `SpellHotbarMCM.pex` are already removed because those MCP replacements exist in code.

**Blocked by:** 02 — Configure from the Mod Control Panel; 03 — Editors and bar drag as windows; 04 — Bind menu as a framework window

**Status:** claimed — development-line MCM/init scripts removed on `ng/smf-next`; runtime acceptance open

**Status (superseded — see the top):** deferred — post-release. Owner ruling 2026-08-29.

- [ ] No Spell Hotbar entry in SkyUI MCM.
- [ ] BattleMage opener power is not granted; the tree still opens from the Perks page.
- [ ] Init quest / BattleMage init / MCM / opener / dual-cast effect scripts are not in the repository source or the Installed Configuration.
- [ ] Hidden natives still compile and drive `castSlot` / load-save / get-set binds / `slotArt` / Spell GCD.
- [ ] New game still gets unbind/dual-cast and `auto_profile.json` from SKSE.
- [ ] Direct Cast and Dual-Input Compatibility still pass on the same natives-and-frames seam as ticket 01.
