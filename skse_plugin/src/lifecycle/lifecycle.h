#pragma once

namespace SpellHotbar::Lifecycle {
    void reset();
    void on_new_game();
    void on_post_load_game();

    bool player_has_power(int type);
    bool toggle_player_power(int type);
    bool open_battlemage_tree();

    /** Return true when the cast was the Spell Hotbar dual-cast toggle. */
    bool process_spell_cast(RE::FormID spell);
}
