# 03 — Editors and bar drag as framework windows

**What to build:** The spell editor, potion editor, Ability editor, Weapon Art icon editor, and bar-drag overlay open as independent SKSE Menu Framework windows, not from the HUD callback. List, filter, edit, reset, and icon behaviour stay what the player has today (save-specific data, format-7 co-save, JSON preset format 2). Windows follow the Mod Control Panel theme. Opening them does not require closing SkyUI. The HUD callback draws only the bar (and anything ticket 04 has not yet moved).

**Blocked by:** 01 — Become an SMF guest

**Status:** claimed — development-line windows exist on `ng/smf-next`; runtime acceptance open

**Status (superseded — see the top):** deferred — post-release. Owner ruling 2026-08-29.

- [ ] Spell editor opens from MCP, edits persist, and the HUD shows the new icon/data after close.
- [ ] Potion editor does the same for potions.
- [ ] Ability editor opens for Custom Abilities and pointer-pack ashes; name, atlas icon, Ability Class, cooldown, GCD, and Ability Costs persist.
- [ ] Weapon Art icon edits persist and draw on the HUD after close.
- [ ] Main bar and Oblivion bar can be dragged from a framework window / Util action; offsets persist.
- [ ] These tools are not drawn inside the HUD callback.
- [ ] Editor-open still captures the keys it must and does not steal input when closed.
