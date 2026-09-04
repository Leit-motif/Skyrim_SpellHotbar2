# 01 — Become an SMF guest

**What to build:** Spell Hotbar 2 no longer hosts Dear ImGui. The live HUD bar appears through SKSE Menu Framework's HUD callback. Cast, modifier, Magic Menu bind-gate, and editor capture filtering run on the framework's input callback. SH2 does not create an ImGui context and does not patch D3D init, Present, WndProc, or the shared input-dispatch site. Direct Cast, Equip, Oblivion, Driver Cast, Cast Channel, Ability, and Weapon Art still work. Co-save writes format 7; formats 5 and 6 still load. Hidden `SpellHotbar` natives still drive a slot. If the HUD misses gameplay frames under ENB / Display Tweaks, Present may be used **only** with the framework's existing context — never a second `CreateContext`. Missing SMF fails closed (log, no second host). Do not disable SMF in the live profile to test that; static verification is the accepted evidence.

**Blocked by:** None

**Status:** claimed — development-line code landed on `ng/smf-next`; runtime acceptance open

**Status (superseded — see the top):** deferred — post-release. Owner ruling 2026-08-29.

Development line (`ng/smf-next` merge `e19b273`): guest registration, SMF input callback, no private host, format 7 writer. These do not close runtime cells.

- [ ] With SMF present, the HUD bar is visible in gameplay without capturing movement or camera (captured frames, not a log that a callback ran).
- [ ] Direct Cast still works from keyboard and from native gamepad through the existing native slot-activation seam.
- [ ] Bind/editor capture still strips the intended keys; keys that should reach the game still do.
- [ ] SH2 does not install a second ImGui context or the old UI trampolines. *(development: static boundary check; runtime: logs)*
- [ ] ENB / Display Tweaks: bar visible and scaled; if MenuManager misses frames, only the Present-on-host-context escape hatch is used, and that choice is recorded as Compatibility Evidence.
- [ ] Without SMF, the plugin does not `CreateContext`; it logs and stays inert as a UI host. *(static fail-closed only; live missing-SMF cell is out of scope)*
