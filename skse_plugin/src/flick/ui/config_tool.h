#pragma once

// Spell Hotbar 2's configuration as a FLICK sidebar tool. The MCM's pages -- keybinds,
// settings, bars, perks, presets, the window openers and the utilities -- are seven tabs of one
// FUCK::ITool named "Spell Hotbar 2", the shape other FLICK guests use for theirs. The player
// reaches it through FLICK's own menu (F7 by default). The Papyrus MCM keeps working beside it.
namespace SpellHotbar::FlickUi::ConfigTool {
    // Register the tool with FLICK. Call at kPostLoadGame with the windows (registering during
    // the initial load crashed on a driver worker thread; see flick_watch.h). Idempotent.
    void register_tool();
}
