# 01 — Become an SMF guest

**What to build:** Spell Hotbar 2 no longer hosts Dear ImGui. The live HUD bar (and, until later tickets, the editors and bind menu still drawn with it) appears through SKSE Menu Framework's HUD callback. Cast, modifier, Magic Menu bind-gate, and editor capture filtering run on the framework's input callback. SH2 does not create an ImGui context and does not patch D3D init, Present, WndProc, or the shared input-dispatch site. Direct Cast, Equip, and Oblivion modes still work. Co-save format 6 still loads. Hidden `SpellHotbar` natives still drive a slot. If the HUD misses gameplay frames under ENB / Display Tweaks, Present may be used **only** with the framework's existing context — never a second `CreateContext`. MCM may still exist. Missing SMF fails closed (log, no second host).

**Blocked by:** None — can start immediately.

**Status:** ready-for-agent

- [ ] With SMF present, the HUD bar is visible in gameplay without capturing movement or camera (captured frames, not a log that a callback ran).
- [ ] Direct Cast still works from keyboard and from native gamepad through the existing native slot-activation seam.
- [ ] Bind/editor capture still strips the intended keys; keys that should reach the game still do.
- [ ] SH2 does not install a second ImGui context or the old UI trampolines.
- [ ] ENB / Display Tweaks: bar visible and scaled; if MenuManager misses frames, only the Present-on-host-context escape hatch is used, and that choice is recorded as Compatibility Evidence.
- [ ] Without SMF, the plugin does not `CreateContext`; it logs and stays inert as a UI host.
