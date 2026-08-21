#pragma once

namespace SpellHotbar::casts::ArtDriver {

	// Enter SH2_Art_State. True means the ready-state transition consumed SH2_ArtStart.
	// False means no listener (sheathed, mid-swing, or the patch is missing).
	bool begin(RE::PlayerCharacter* pc);

	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag);

	// True while SH2_Art_State is live. WASD capture (Cast Plant) follows this,
	// not the casting instance — same rule as MscoCastDriver::is_active().
	bool is_active();

	// Classify the live clip's latch from its annotations (WinOpen else HitFrame
	// else SH2_ArtExit). Called when SH2_Art_Clip activates.
	void bind_latch(bool has_win_open, bool has_hit_frame);

	bool latch_open();

	bool should_trace_graph_events();

	void cancel(RE::PlayerCharacter* pc);

	void finish(RE::PlayerCharacter* pc);

	// Drop in-memory art liveness without touching the graph (save load / new game).
	void reset_session();
}
