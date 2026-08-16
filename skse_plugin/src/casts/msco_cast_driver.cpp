#include "msco_cast_driver.h"
#include "combo_cache.h"
#include <array>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <string>
#include <Windows.h>
#include "../logger/logger.h"

using namespace std::literals;

namespace SpellHotbar::casts::MscoCastDriver {

	namespace {
		std::atomic<bool> state_active{ false };
		std::atomic<bool> combo_window{ false };
		std::atomic<int> trace_budget{ 0 };
		constexpr int post_cut_trace_events{ 24 };

		MscoChargeCurve g_curve{};
		RollingMcoCombo g_rolling;
		CastComboIndex g_castIndex;

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

		// PIE is #0006's reset payload. The original handler applies it before this
		// observer runs, so a write here lands after the stomp. The named ready
		// markers follow that burst; the first of them consumes so a later PIE
		// (idle, shout, or the next swing's own advance) cannot replay the index.
		bool is_reset_payload(std::string_view tag)
		{
			return tag == "PIE"sv;
		}

		bool is_restore_edge(std::string_view tag)
		{
			return tag == "SBF_ReadyStart"sv || tag == "MSCO_MagicReady"sv;
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
			if (const auto combo = g_rolling.arm(now_ms())) {
				logger::debug("SH2 cast: combo restore armed next={} power={}", combo->nextAttack,
					combo->nextPowerAttack);
			}
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
			combo_window.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: notified {} (clip {}) -> {}", event, index, sent);
			return sent;
		}

		void write_clip_speed(RE::PlayerCharacter* pc, float charge_time)
		{
			const float speed = charge_time_to_anim_speed(charge_time, g_curve);
			const bool ok = pc->SetGraphVariableFloat("MSCO_attackspeed", speed);
			logger::debug("SH2 cast: MSCO_attackspeed={} charge={} wrote={}", speed, charge_time, ok);
		}

		const char* msco_ini()
		{
			return "Data\\SKSE\\Plugins\\MSCO.ini";
		}

		float ini_float(const char* section, const char* key, float fallback)
		{
			char buf[64]{};
			GetPrivateProfileStringA(section, key, "", buf, static_cast<DWORD>(sizeof(buf)), msco_ini());
			if (buf[0] == '\0') {
				return fallback;
			}
			char* end = nullptr;
			const float v = std::strtof(buf, &end);
			return end != buf ? v : fallback;
		}
	}

	void load_charge_curve()
	{
		MscoChargeCurve curve{};
		const char* ini = msco_ini();
		curve.mechanic_on = GetPrivateProfileIntA("General", "ChargeMechanicOn", 1, ini) != 0;
		curve.exp_mode = GetPrivateProfileIntA("General", "ExpMode", 1, ini) != 0;
		curve.shortest = ini_float("ChargeTime", "Shortest", curve.shortest);
		curve.longest = ini_float("ChargeTime", "Longest", curve.longest);
		curve.base_time = ini_float("ChargeTime", "BaseTime", curve.base_time);
		curve.min_speed = ini_float("SpeedClamp", "MinSpeed", curve.min_speed);
		curve.max_speed = ini_float("SpeedClamp", "MaxSpeed", curve.max_speed);
		curve.exp_factor = ini_float("Exp", "ExpFactor", curve.exp_factor);
		g_curve = curve;
		logger::info(
			"SH2 cast: MSCO charge curve mechanic={} exp={} base={} short={} long={} min={} max={} p={}",
			curve.mechanic_on, curve.exp_mode, curve.base_time, curve.shortest, curve.longest,
			curve.min_speed, curve.max_speed, curve.exp_factor);
	}

	bool combo_window_open()
	{
		return combo_window.load(std::memory_order_relaxed);
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand, float charge_time)
	{
		if (!pc) {
			return false;
		}
		(void)hand;
		trace_budget.store(0, std::memory_order_relaxed);
		auto* left = pc->GetEquippedObject(true);
		const bool left_holds_spell =
			left && (left->Is(RE::FormType::Spell) || left->Is(RE::FormType::Scroll));
		if (isolate_left_hand_caster_for_driver_cast(left_holds_spell)) {
			if (auto* caster = pc->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand)) {
				caster->InterruptCast(true);
			}
			logger::debug("SH2 cast: isolated left-hand caster at Driver Cast start");
		}
		write_clip_speed(pc, charge_time);
		return send_entry(pc);
	}

	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag)
	{
		const std::string_view tag{ a_tag.c_str() ? a_tag.c_str() : "" };

		if (is_attack_time(tag)) {
			McoCombo sample{};
			if (sample_mco(a_player, sample)) {
				g_rolling.record(sample, now_ms());
			} else {
				g_rolling.disarm();
			}
		}

		if (is_msco_combo_window_open_event(tag) && is_active()) {
			combo_window.store(true, std::memory_order_relaxed);
			logger::debug("SH2 cast: combo window open ({})", tag);
		}

		if (is_msco_combo_window_close_event(tag)) {
			combo_window.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: combo window closed ({})", tag);
		}

		if (tag == "SH2_CastExit"sv) {
			if (is_active()) {
				arm_restore();
			}
			state_active.store(false, std::memory_order_relaxed);
			combo_window.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: state exiting (clip end or cancel)");
		}

		if (is_reset_payload(tag)) {
			if (const auto combo = g_rolling.peek()) {
				write_mco(a_player, *combo);
			}
		}

		if (is_restore_edge(tag)) {
			if (const auto combo = g_rolling.consume()) {
				write_mco(a_player, *combo);
			}
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
		combo_window.store(false, std::memory_order_relaxed);
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
		combo_window.store(false, std::memory_order_relaxed);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		if (is_active()) {
			arm_restore();
		}
		send_exit(pc);
		state_active.store(false, std::memory_order_relaxed);
		combo_window.store(false, std::memory_order_relaxed);
	}
}
