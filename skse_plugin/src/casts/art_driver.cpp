#include "art_driver.h"
#include <atomic>
#include "../logger/logger.h"
#include "../game_data/game_data.h"

using namespace std::literals;

namespace SpellHotbar::casts::ArtDriver {

	namespace {
		std::atomic<bool> state_active{ false };
		std::atomic<int> trace_budget{ 0 };
		constexpr int post_exit_trace_events{ 24 };

		void normalize_combo(RE::Actor* actor)
		{
			if (!actor) {
				return;
			}
			actor->SetGraphVariableInt("MCO_nextattack", 1);
			actor->SetGraphVariableInt("MCO_nextpowerattack", 1);
			logger::debug("SH2 art: normalised MCO_nextattack=1 MCO_nextpowerattack=1");
		}

		void send_exit(RE::PlayerCharacter* pc)
		{
			if (pc) {
				const bool consumed = pc->NotifyAnimationGraph("SH2_ArtExit"sv);
				logger::debug("SH2 art: notified SH2_ArtExit -> {}", consumed);
			}
		}
	}

	bool begin(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return false;
		}
		trace_budget.store(0, std::memory_order_relaxed);
		const bool sent = pc->NotifyAnimationGraph("SH2_ArtStart"sv);
		state_active.store(sent, std::memory_order_relaxed);
		logger::debug("SH2 art: notified SH2_ArtStart -> {}", sent);
		if (sent) {
			normalize_combo(pc);
		}
		return sent;
	}

	void observe_graph_event(RE::Actor*, const RE::BSFixedString& a_tag)
	{
		const std::string_view tag{ a_tag.c_str() ? a_tag.c_str() : "" };
		if (tag == "SH2_ArtExit"sv ||
			(is_active() && (tag == "MCO_AttackEnterNotify"sv || tag == "MCO_AttackInitiate"sv))) {
			state_active.store(false, std::memory_order_relaxed);
			GameData::reset_art_selector();
			logger::debug("SH2 art: state exiting ({})", tag);
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
		int remaining = trace_budget.load(std::memory_order_relaxed);
		while (remaining > 0) {
			if (trace_budget.compare_exchange_weak(remaining, remaining - 1, std::memory_order_relaxed)) {
				return true;
			}
		}
		return false;
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		if (is_active()) {
			trace_budget.store(post_exit_trace_events, std::memory_order_relaxed);
		}
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
		GameData::reset_art_selector();
	}

	void finish(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
		GameData::reset_art_selector();
	}

	void reset_session()
	{
		state_active.store(false, std::memory_order_relaxed);
		trace_budget.store(0, std::memory_order_relaxed);
		GameData::reset_art_selector();
	}
}
