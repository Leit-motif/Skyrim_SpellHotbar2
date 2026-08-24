#pragma once

#include <string_view>

// Throwaway ticket-28 instrumentation: WHERE does a write to MCO_nextattack land?
//
// The player holds several behavior graphs (at least 1hm_behavior and magicbehavior host the
// shtb states), and holder-level Actor::SetGraphVariableInt reaches whichever the holder picks
// -- not necessarily the graph whose AttackNodes_StateMachine binds startStateId to the
// variable. Every line here is per-graph so the log says which graph accepted a write and which
// one MCO actually reads.
//
// The mode is read fresh from the ini at each use so a probe can change between game runs
// without a rebuild. Nothing here changes an existing write path; mode 0 only logs.
namespace SpellHotbar::casts::ComboProbe {

	// Data\SKSE\Plugins\SpellHotbar2_ComboProbe.ini, [Probe] iMode. Missing file = 0.
	// 0 = read-only, 2 = write every graph on a right-attack press, 3 = write every graph at
	// the stomp-undo window-close.
	int mode();

	// One compact info line: every graph's identity plus its own MCO_nextattack /
	// MCO_nextpowerattack, and the holder-level read for comparison.
	void probe_read_graphs(RE::Actor* actor, std::string_view where);

	// Write both counters into EVERY graph individually, then read each back on the same line,
	// so a graph that refused the write is visible rather than assumed.
	void probe_write_graphs(RE::Actor* actor, int nextAttack, int nextPowerAttack,
		std::string_view where);
}
