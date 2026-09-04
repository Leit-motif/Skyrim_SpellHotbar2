# 04 — Bind menu as a framework window

**What to build:** The advanced bind menu opens on the existing “Open Binding Menu” bind while Magic Menu or a supported inventory tab is open, as a framework window (not the HUD callback). It still swallows input so vanilla menu keys do not fight slotting. Keyboard and native gamepad can slot skills, Abilities, and Weapon Arts. Dual-Input Compatibility holds, including the Installed Configuration’s controller bind-menu preset behaviour.

**Blocked by:** 01 — Become an SMF guest

**Status:** parked — owner 2026-09-04 accepted the development line after a partial live test; remaining runtime cells stay unchecked. Do not merge to `origin/master`.

**Status (superseded — see the top):** claimed — development-line window exists on `ng/smf-next`; runtime acceptance open

**Status (superseded — see the top):** deferred — post-release. Owner ruling 2026-08-29.

- [ ] The bind-menu key opens the window only in the same menu contexts as today; it does not open in gameplay.
- [ ] While open, bound keys slot skills and are not forwarded to the game; closing restores forwarding.
- [ ] Keyboard and native gamepad can both complete a bind (Dual-Input Compatibility).
- [ ] The Arts tab lists Abilities / Weapon Arts; drag or assign still writes `art_id` slots.
- [ ] The menu is not drawn from the HUD callback.
- [ ] Existing slot-activation natives still match what the player just bound, including `castSlot` and `slotArt`.

## Comments

Owner 2026-09-04: partial live test; integration handoff closed for now. Runtime checkboxes stay open. No stable merge.
