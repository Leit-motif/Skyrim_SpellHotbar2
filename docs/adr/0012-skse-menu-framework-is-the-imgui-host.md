# SKSE Menu Framework is the ImGui host; Spell Hotbar 2 is a guest

Date: 2026-08-20

Status: accepted

Supersedes ADR-0003 (keep the mod's own ImGui interface).

This Personal Integration uses SKSE Menu Framework as the only Dear ImGui host. Spell Hotbar 2 stops creating an ImGui context and stops hooking D3D init, DXGI Present, WndProc, and the shared input-dispatch site for UI. Configuration lives on the Mod Control Panel; editors, bar drag, and the bind menu are framework windows; the HUD bar is drawn through the framework's HUD callback (ticket 01 may move only the call site to Present while still using the framework's context). Settings, binds, co-save and JSON presets, Direct Cast, and Magic Menu bind arbitration stay Spell Hotbar 2's. Dual-hosting two ImGui contexts is rejected: both plugins patch the same input trampoline and both `CreateContext`, which is slower and unsafe.

SkyUI MCM is retired for this Installed Configuration. SKSE Menu Framework is a hard requirement. Papyrus that only exists to drive MCM, grant the BattleMage opener power, or `AddSpell` on quest init is replaced by SKSE. Hidden `SpellHotbar` natives stay as a test seam. BattleMage perk sliders stay ESP globals. The BattleMage lesser power is not auto-granted; the Perks page opens the tree through Custom Skills' existing native. Unbind and dual-cast remain gameplay spells, granted from SKSE.

ADR-0003's load-bearing facts survive as constraints, not as a host choice: the HUD must still draw every frame without stealing input; bind capture must still decide whether a key reaches the game while Magic Menu is open; theming the bar is not a reason to pick Prisma or a second overlay.

## Considered options

- Keep SH2's host and restyle it (ADR-0003). Rejected: leaves SkyUI MCM, a second ImGui frame, and hook conflict with the SMF already in this load order.
- Register MCP pages while SH2 still `CreateContext`. Rejected: dual host.
- Prisma UI for settings. Rejected earlier: second UI stack, no released gamepad host, poor match for this HUD.
- Optional SMF with MCM fallback. Rejected for this Personal Integration.

## Consequences

- Core Fork work: guest adapter, `ImGuiMCP` port of editors, input filter moved onto the framework's `AddInputEvent`, MCM scripts gone from the Installed Configuration. Implement on `ng/smf-next`; see `docs/agents/smf-addon-line.md`.
- Compatibility Package / FOMOD: CSF 2 vs 3 tree *data* stays out of this decision.
- A later public list without SMF is out of scope until someone chooses to reverse the hard requirement.
