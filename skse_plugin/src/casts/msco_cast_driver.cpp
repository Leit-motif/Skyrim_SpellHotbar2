#include "msco_cast_driver.h"
#include "combo_cache.h"
#include <array>
#include <atomic>
#include <chrono>
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {
		std::atomic<bool> state_active{ false };
		std::atomic<int> trace_budget{ 0 };
		constexpr int post_cut_trace_events{ 24 };

		RollingMcoCombo g_rolling;
		CastComboIndex g_castIndex;
		std::atomic<bool> restore_pending{ false };
		McoCombo restore_value{};

		constexpr std::array<std::string_view, CastComboIndex::kLength> kCastEvents{
			"SH2_CastRight"sv,
			"SH2_Cast2"sv,
			"SH2_Cast3"sv,
			"SH2_Cast4"sv,
		};

		double now_ms()
		{
			using clock = std::chrono::steady_clock;
			static const auto origin = clock::now();
			return std::chrono::duration<double, std::milli>(clock::now() - origin).count();
		}

		std::string_view event_for(int index)
		{
			if (index < 1 || index > CastComboIndex::kLength) {
				return kCastEvents.front();
			}
			return kCastEvents[static_cast<size_t>(index - 1)];
		}

		bool is_attack_time(std::string_view tag)
		{
			return tag == "MCO_AttackInitiate"sv || tag == "MCO_PowerAttackInitiate"sv ||
				   tag == "HitFrame"sv;
		}

		bool is_ready_pass(std::string_view tag)
		{
			return tag == "SBF_ReadyStart"sv || tag == "MSCO_MagicReady"sv || tag == "inRdy"sv ||
				   tag == "attackStop"sv || tag == "PIE"sv;
		}

		bool sample_mco(RE::Actor* actor, McoCombo& out)
		{
			if (!actor) {
				return false;
			}
			std::int32_t next = 0;
			std::int32_t power = 0;
			if (!actor->GetGraphVariableInt("MCO_nextattack", next) ||
				!actor->GetGraphVariableInt("MCO_nextpowerattack", power)) {
				return false;
			}
			out.nextAttack = next;
			out.nextPowerAttack = power;
			return true;
		}

		void write_mco(RE::Actor* actor, const McoCombo& combo)
		{
			if (!actor) {
				return;
			}
			actor->SetGraphVariableInt("MCO_nextattack", combo.nextAttack);
			actor->SetGraphVariableInt("MCO_nextpowerattack", combo.nextPowerAttack);
			logger::debug("SH2 cast: restored MCO_nextattack={} MCO_nextpowerattack={}",
				combo.nextAttack, combo.nextPowerAttack);
		}

		void arm_restore()
		{
			const auto combo = g_rolling.usable(now_ms());
			if (!combo) {
				restore_pending.store(false, std::memory_order_relaxed);
				return;
			}
			restore_value = *combo;
			restore_pending.store(true, std::memory_order_relaxed);
			logger::debug("SH2 cast: combo restore armed next={} power={}", combo->nextAttack,
				combo->nextPowerAttack);
		}

		void send_exit(RE::PlayerCharacter* pc)
		{
			if (pc) {
				const bool consumed = pc->NotifyAnimationGraph("SH2_CastExit"sv);
				logger::debug("SH2 cast: notified SH2_CastExit -> {}", consumed);
			}
		}

		bool send_entry(RE::PlayerCharacter* pc)
		{
			const int index = g_castIndex.current();
			const auto event = event_for(index);
			const bool sent = pc->NotifyAnimationGraph(event);
			if (sent) {
				g_castIndex.advance();
			}
			state_active.store(sent, std::memory_order_relaxed);
			logger::debug("SH2 cast: notified {} (clip {}) -> {}", event, index, sent);
			return sent;
		}
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand)
	{
		if (!pc) {
			return false;
		}
		(void)hand;
		trace_budget.store(0, std::memory_order_relaxed);
		return send_entry(pc);
	}

	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag)
	{
		const std::string_view tag{ a_tag.c_str() ? a_tag.c_str() : "" };

		if (is_attack_time(tag)) {
			McoCombo sample{};
			if (sample_mco(a_player, sample)) {
				g_rolling.record(sample, now_ms());
			}
			restore_pending.store(false, std::memory_order_relaxed);
		}

		if (tag == "SH2_CastExit"sv) {
			arm_restore();
			state_active.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: state exiting (clip end or cancel)");
		}

		if (restore_pending.load(std::memory_order_relaxed) && is_ready_pass(tag)) {
			write_mco(a_player, restore_value);
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

	bool replay(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return false;
		}
		if (is_active()) {
			return true;
		}
		// Concentration re-entry is not a combo step. Send the original event so a
		// looping channel does not walk the clip set; ticket 11 leaves channels out.
		const bool sent = pc->NotifyAnimationGraph("SH2_CastRight"sv);
		state_active.store(sent, std::memory_order_relaxed);
		return sent;
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		if (is_active()) {
			trace_budget.store(post_cut_trace_events, std::memory_order_relaxed);
			arm_restore();
		}
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		if (is_active()) {
			arm_restore();
		}
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
	}
}
