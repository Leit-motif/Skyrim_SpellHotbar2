#pragma once

namespace SpellHotbar::casts::ArtDriver {

	// Enter SH2_Art_State. True means the ready-state transition consumed SH2_ArtStart.
	// False means no listener (sheathed, mid-swing, or the patch is missing).
	bool begin(RE::PlayerCharacter* pc);

	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag);

	bool is_active();

	bool should_trace_graph_events();

	void cancel(RE::PlayerCharacter* pc);

	void finish(RE::PlayerCharacter* pc);
}
