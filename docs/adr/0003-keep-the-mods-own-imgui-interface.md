# Keep the mod's own ImGui interface

Date: recorded during Baseline Adoption planning

Status: superseded by ADR-0012

The fork will not migrate the interface to SKSE Menu Framework or any other shared menu host. The interface stays as upstream built it.

Spell Hotbar 2 draws three surfaces with different requirements. The hotbar itself renders every frame with no input capture, so the plugin's `DXGIPresent` hook has to stay whatever else changes. The advanced bind menu depends on the mod's own per-event input interception, which decides whether a key reaches the game at all and is gated on the vanilla Magic Menu being open; a shared host is not built to arbitrate that. Only the spell, potion, icon and bar-drag editors are shaped like the config windows SKSE Menu Framework exists to host, and they are the surface the player touches least. A migration would therefore remove no hooks, resolve no conflicts with other ImGui-based mods in the load order, and change only the door to the least-used third of the interface.

The complaint behind the proposal was that the interface reads as foreign rather than part of the game. That is presentation, not framework: the theme is a bare `ImGui::StyleColorsDark()`, the stock debug-tool palette. If it still grates after the Accepted Baseline, the response is a theming ticket against the existing renderer — palette, font, bar styling — not a change of UI framework.

The baseline compatibility spec already excludes redesigning the UI. This decision records why the idea was raised and set aside, so it is not reopened as scope once customization begins.
