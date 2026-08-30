#include "art_driver.h"
#include "clip_translation_driver.h"
#include "combo_cache.h"
#include "msco_cast_driver.h"
#include <atomic>
#include "../logger/logger.h"
#include "../game_data/game_data.h"

using namespace std::literals;

namespace SpellHotbar::casts::ArtDriver {

	namespace {
		std::atomic<bool> state_active{ false };
		std::atomic<bool> latch_is_open{ false };
		std::atomic<int> latch{ static_cast<int>(AbilityLatch::artExit) };
		std::atomic<int> trace_budget{ 0 };
		constexpr int post_exit_trace_events{ 24 };

		void send_exit(RE::PlayerCharacter* pc)
		{
			if (pc) {
				const bool consumed = pc->NotifyAnimationGraph("SH2_ArtExit"sv);
				logger::debug("SH2 art: notified SH2_ArtExit -> {}", consumed);
			}
		}

		void on_exit(std::string_view tag)
		{
			state_active.store(false, std::memory_order_relaxed);
			latch_is_open.store(false, std::memory_order_relaxed);
			GameData::reset_art_selector();
			MscoCastDriver::arm_combo_restore();
			logger::debug("SH2 art: state exiting ({})", tag);
		}
	}

	/**
	 * TICKET 37. The bound on a live art state does NOT live in this file. It is
	 * `CastingInstanceWeaponArt::update()` in casting_controller.cpp, which caps the art at eight
	 * seconds and tears it down through `cancel()` below -- the same number, and the same
	 * teardown, the cast state's own watchdog uses.
	 *
	 * That holds only because `begin()` is called exactly once, with a `CastingInstanceWeaponArt`
	 * freshly installed as `current_cast`, so something is always polling this state. A change
	 * that lets the driver outlive its casting instance removes the bound silently: `state_active`
	 * would stay true with nothing to time it out, and the input latch behind it would retain
	 * every press. Give the driver its own deadline before making that change.
	 */
	bool begin(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return false;
		}
		trace_budget.store(0, std::memory_order_relaxed);
		latch_is_open.store(false, std::memory_order_relaxed);
		latch.store(static_cast<int>(AbilityLatch::artExit), std::memory_order_relaxed);
		const bool sent = pc->NotifyAnimationGraph("SH2_ArtStart"sv);
		state_active.store(sent, std::memory_order_relaxed);
		if (sent) {
			MscoCastDriver::interrupt_equipped_casters_if_spell(pc);
		}
		logger::debug("SH2 art: notified SH2_ArtStart -> {}", sent);
		return sent;
	}

	void bind_latch(bool has_win_open, bool has_hit_frame)
	{
		const auto kind = classify_ability_latch(has_win_open, has_hit_frame);
		latch.store(static_cast<int>(kind), std::memory_order_relaxed);
		logger::debug("SH2 art: latch {} (winopen={} hitframe={})",
			static_cast<int>(kind), has_win_open, has_hit_frame);
	}

	void observe_graph_event(RE::Actor*, const RE::BSFixedString& a_tag)
	{
		const std::string_view tag{ a_tag.c_str() ? a_tag.c_str() : "" };
		const auto kind = static_cast<AbilityLatch>(latch.load(std::memory_order_relaxed));
		if (is_active() && is_ability_latch_event(kind, tag)) {
			latch_is_open.store(true, std::memory_order_relaxed);
			logger::debug("SH2 art: latch open ({})", tag);
		}
		if (tag == "SH2_ArtExit"sv ||
			(is_active() && latch_is_open.load(std::memory_order_relaxed) &&
				(tag == "MCO_AttackEnterNotify"sv || tag == "MCO_AttackInitiate"sv))) {
			on_exit(tag);
		}
	}

	bool is_active()
	{
		return state_active.load(std::memory_order_relaxed);
	}

	bool latch_open()
	{
		return latch_is_open.load(std::memory_order_relaxed);
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
		on_exit("SH2_ArtExit"sv);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		send_exit(pc);
		on_exit("SH2_ArtExit"sv);
	}

	void reset_session()
	{
		state_active.store(false, std::memory_order_relaxed);
		latch_is_open.store(false, std::memory_order_relaxed);
		latch.store(static_cast<int>(AbilityLatch::artExit), std::memory_order_relaxed);
		trace_budget.store(0, std::memory_order_relaxed);
		GameData::reset_art_selector();
		ClipTranslationDriver::reset();
	}
}
