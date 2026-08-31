#pragma once

namespace SpellHotbar::Lifecycle {
    constexpr bool should_run_first_initialization(bool is_new_game, bool loaded_existing_settings)
    {
        return is_new_game || !loaded_existing_settings;
    }
}
