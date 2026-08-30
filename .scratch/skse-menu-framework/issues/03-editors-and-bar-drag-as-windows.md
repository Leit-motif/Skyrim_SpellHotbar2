# 03 — Editors and bar drag as framework windows

**What to build:** The spell editor, potion editor, and bar-drag overlay open as independent SKSE Menu Framework windows from the Mod Control Panel, not from the HUD callback. List, filter, edit, reset, and icon behaviour stay what the player has today (save-specific data, same co-save). Windows follow the Mod Control Panel theme. Opening them does not require closing SkyUI. The HUD callback draws only the bar (and anything ticket 04 has not yet moved).

**Blocked by:** 01 — Become an SMF guest

**Status:** deferred — post-release. Owner ruling 2026-08-29: *"i've said this so many times. this is out of scope until after release."* The whole SMF effort ships **after** the first Nexus release; the release ships the SkyUI MCM and the current ImGui editors. Marked deferred so a frontier scan stops surfacing it — the ruling has been given repeatedly and restating it more firmly did not hold. Unblocks the moment the release is published; see `../release/issues/01-freeze-the-product-surface.md`.

- [ ] Spell editor opens from MCP, edits persist, and the HUD shows the new icon/data after close.
- [ ] Potion editor does the same for potions.
- [ ] Main bar and Oblivion bar can be dragged from a framework window / Util action; offsets persist.
- [ ] These tools are not drawn inside the HUD callback.
- [ ] Editor-open still captures the keys it must and does not steal input when closed.
