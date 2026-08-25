# 04 — Bind menu as a framework window

**What to build:** The advanced bind menu opens on the existing “Open Binding Menu” bind while Magic Menu or a supported inventory tab is open, as a framework window (not the HUD callback). It still swallows input so vanilla menu keys do not fight slotting. Keyboard and native gamepad can slot skills. Dual-Input Compatibility holds, including the Installed Configuration’s controller bind-menu preset behaviour.

**Blocked by:** 01 — Become an SMF guest

**Status:** ready-for-agent

- [ ] The bind-menu key opens the window only in the same menu contexts as today; it does not open in gameplay.
- [ ] While open, bound keys slot skills and are not forwarded to the game; closing restores forwarding.
- [ ] Keyboard and native gamepad can both complete a bind (Dual-Input Compatibility).
- [ ] The menu is not drawn from the HUD callback.
- [ ] Existing slot-activation natives still match what the player just bound.
