#include "msco_cast_driver.h"
#include <atomic>
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {
		// True from a consumed SH2_CastRight send until the state ends. The state's enter/exit
		// notify events cannot feed this flag: SH2_CastDone never arrives at all, and while
		// SH2_CastEnter does arrive on schedule (+0.197s, traced 2026-08-12, correcting what
		// ticket 08 first recorded), it says only that the state was entered -- which the
		// notify's own true return already said, a frame earlier and without a graph round
		// trip. The notify's return is the entry signal, SH2_CastExit is the exit signal.
		std::atomic<bool> state_active{ false };

		// How many more graph events to trace after a cut. See should_trace_graph_events.
		std::atomic<int> trace_budget{ 0 };
		constexpr int post_cut_trace_events{ 24 };

		// SH2_CastExit is this mod's own event: the only listener is the state-local
		// transition inside SH2_CastRight_State, so sending it while the state is not
		// live reaches nothing. Sending unconditionally covers the case where entry
		// happened but the state was never observed going live.
		void send_exit(RE::PlayerCharacter* pc)
		{
			if (pc) {
				// The return says whether a transition consumed the event, exactly as it does
				// for the entry send. It is logged because a cut that reaches nothing and a cut
				// the state ignores look identical from outside, and the chain-out depends on
				// telling them apart.
				const bool consumed = pc->NotifyAnimationGraph("SH2_CastExit"sv);
				logger::debug("SH2 cast: notified SH2_CastExit -> {}", consumed);
			}
		}
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand)
	{
		if (!pc) {
			return false;
		}
		// Minimal slice: one state, one clip (MSCO_left1.hkx, whose SpellFire annotation
		// is the LEFT-hand event); every hand routes into it. Entry transitions live in two
		// graphs -- magicbehavior's MagicRoot and 1hm_behavior's 1HM_Ready_State -- so a false
		// here means no hosting drawn idle is current: the weapon is sheathed, or the graph
		// sits in AttackState mid-swing. The controller then tears the cast down through its
		// normal failure path.
		// A true means the transition was consumed and the state is live NOW -- the clip
		// starts this frame (annotations verified on schedule from the send timestamp).
		(void)hand;
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		state_active.store(sent, std::memory_order_relaxed);
		trace_budget.store(0, std::memory_order_relaxed);
		logger::debug("SH2 cast: notified SH2_CastRight -> {}", sent);
		return sent;
	}

	void observe_graph_event(RE::Actor*, const RE::BSFixedString& a_tag)
	{
		// SH2_CastExit reaches this sink two ways, both meaning the state is ending: the
		// clip's own end-of-clip trigger, and our cancel/finish sends echoed back. Do NOT
		// react to SH2_CastEnter here -- it says only that the state was entered, which the
		// entry notify's return already established, and reacting to a late one would raise
		// the flag on a state that had since died.
		if (a_tag == "SH2_CastExit"sv) {
			state_active.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: state exiting (clip end or cancel)");
		}
	}

	bool is_active()
	{
		return state_active.load(std::memory_order_relaxed);
	}

	bool should_trace_graph_events()
	{
		if (is_active()) {
			return true;
		}
		// One event of the burst per call, so an idle session cannot be traced at all and a
		// cut is followed by a bounded window whatever happens next.
		int remaining = trace_budget.load(std::memory_order_relaxed);
		while (remaining > 0) {
			if (trace_budget.compare_exchange_weak(remaining, remaining - 1, std::memory_order_relaxed)) {
				return true;
			}
		}
		return false;
	}

	bool replay(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return false;
		}
		if (is_active()) {
			return true;
		}
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		state_active.store(sent, std::memory_order_relaxed);
		return sent;
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		// Only a cut taken while the state was live is worth tracing out of; finish()'s
		// routine end-of-cast send is not, and neither is a second cut after the first.
		if (is_active()) {
			trace_budget.store(post_cut_trace_events, std::memory_order_relaxed);
		}
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
	}
}
