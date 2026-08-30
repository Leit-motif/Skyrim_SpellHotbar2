# 04 — Bind menu as a framework window

**What to build:** The advanced bind menu opens on the existing “Open Binding Menu” bind while Magic Menu or a supported inventory tab is open, as a framework window (not the HUD callback). It still swallows input so vanilla menu keys do not fight slotting. Keyboard and native gamepad can slot skills. Dual-Input Compatibility holds, including the Installed Configuration’s controller bind-menu preset behaviour.

**Blocked by:** 01 — Become an SMF guest

**Status:** deferred — post-release. Owner ruling 2026-08-29: *"i've said this so many times. this is out of scope until after release."* The whole SMF effort ships **after** the first Nexus release; the release ships the SkyUI MCM and the current ImGui editors. Marked deferred so a frontier scan stops surfacing it — the ruling has been given repeatedly and restating it more firmly did not hold. Unblocks the moment the release is published; see `../release/issues/01-freeze-the-product-surface.md`.

- [ ] The bind-menu key opens the window only in the same menu contexts as today; it does not open in gameplay.
- [ ] While open, bound keys slot skills and are not forwarded to the game; closing restores forwarding.
- [ ] Keyboard and native gamepad can both complete a bind (Dual-Input Compatibility).
- [ ] The menu is not drawn from the HUD callback.
- [ ] Existing slot-activation natives still match what the player just bound.
