#include "msco_cast_driver.h"
#include "art_driver.h"
#include "clip_translation_driver.h"
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
		std::atomic<CastShape> cast_shape{ CastShape::fire_and_forget };
		std::atomic<bool> clip_committed{ false };
		std::atomic<int> trace_budget{ 0 };
		// When the live channel entered its state, so the hold can be discounted from the combo
		// sample's age at release. Zero means no channel is holding.
		std::atomic<double> channel_started_ms{ 0.0 };
		// When the entry notify was accepted, so a state the graph never leaves can be timed out.
		// Zero means no entry is recorded and there is nothing to time.
		std::atomic<double> state_entered_ms{ 0.0 };
		constexpr int post_cut_trace_events{ 24 };

		MscoChargeCurve g_curve{};
		RollingMcoCombo g_rolling;
		CastComboIndex g_castIndex;
		// Ticket 29. `g_swing` says which swing is up, so a live sample equal to the playing index
		// can be recognised as pre-advance instead of restored as a replay; `g_successors` holds
		// what each swing taught, so the successor can be substituted without ever deriving it.
		McoSwingTracker g_swing;
		McoSuccessorTable g_successors;

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

		// The ready edges only exist on some weapon graphs: the 1H trace passes attackStop /
		// SBF_ReadyStart / MSCO_MagicReady on the way out of our state (ticket 10), but a
		// greatsword session fired neither ready tag after SH2_CastExit (measured 2026-08-24),
		// so a pending restore was never consumed and the stomp-undo branch rewrote the stale
		// index over every later swing's WinClose teaching -- the combo pinned at the restored
		// attack. The initiate tags are the consume edge that exists on every weapon: they fire
		// on the fresh swing itself, after the engine has already read the variable, and our own
		// Driver Cast never raises them.
		bool is_restore_edge(std::string_view tag)
		{
			return tag == "SBF_ReadyStart"sv || tag == "MSCO_MagicReady"sv ||
				   tag == "MCO_AttackInitiate"sv || tag == "MCO_PowerAttackInitiate"sv;
		}

		constexpr std::string_view kNextAttackVar{ "MCO_nextattack"sv };
		constexpr std::string_view kNextPowerAttackVar{ "MCO_nextpowerattack"sv };

		constexpr std::string_view var_name(McoSgviVariable kind)
		{
			return kind == McoSgviVariable::next_attack ? kNextAttackVar : kNextPowerAttackVar;
		}

		// Which moveset the successor table is being taught about. Movesets differ per weapon type
		// -- lengths, wraps, and stance reorderings all differ -- so a greatsword's teaching must
		// never answer for a dagger's. Anything that is not a weapon in the right hand (fists, a
		// spell, an empty hand) keys to 0, which is also `kHandToHandMelee`: they share a moveset,
		// so sharing a row is correct rather than a fallback.
		int weapon_key(RE::Actor* actor)
		{
			if (!actor) {
				return 0;
			}
			auto* right = actor->GetEquippedObject(false);
			if (!right) {
				return 0;
			}
			auto* weapon = right->As<RE::TESObjectWEAP>();
			if (!weapon) {
				return 0;
			}
			const int key = static_cast<int>(weapon->GetWeaponType());
			return (key >= 0 && key < McoSuccessorTable::kWeaponKeys) ? key : 0;
		}

		// -1 says the graph bool itself could not be read, which is neither state: it must not be
		// mistaken for "not attacking", because the gates below only ever act on a definite true.
		int read_is_attacking(RE::Actor* actor)
		{
			bool attacking{ false };
			if (!actor || !actor->GetGraphVariableBool(RE::BSFixedString("IsAttacking"sv), attacking)) {
				return -1;
			}
			return attacking ? 1 : 0;
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
			// This is the write that matters, and it lands in the ROOT graph's storage --
			// which is precisely where the nested MCO_Attack.hkb selector reads it from when
			// it reactivates (ADR-0014). Actor-level is not a compromise here; it is the
			// only storage the nested graph's variable link resolves against.
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
				// Write now so a same-frame recovery attack (Ability latch cut) sees the
				// sampled index. The payload edge re-asserts it against each stomp and the
				// initiate/ready edges consume it, so it survives #0006's reset.
				write_mco(RE::PlayerCharacter::GetSingleton(), *combo);
			}
		}

		// The state is gone and nothing is chainable through it. Shared by every way a cast
		// leaves the state, so a new piece of per-cast state is cleared in one place.
		void clear_state_flags()
		{
			channel_started_ms.store(0.0, std::memory_order_relaxed);
			state_entered_ms.store(0.0, std::memory_order_relaxed);
			state_active.store(false, std::memory_order_relaxed);
			combo_window.store(false, std::memory_order_relaxed);
			cast_shape.store(CastShape::fire_and_forget, std::memory_order_relaxed);
		}

		void send_exit(RE::PlayerCharacter* pc)
		{
			if (pc) {
				const bool consumed = pc->NotifyAnimationGraph("SH2_CastExit"sv);
				logger::debug("SH2 cast: notified SH2_CastExit -> {}", consumed);
			}
		}

		constexpr std::string_view kChannelEvent{ "SH2_CastChannel"sv };

		bool send_entry(RE::PlayerCharacter* pc, CastShape shape)
		{
			const int index = g_castIndex.current();
			const auto event = cast_entry_walks_clip_set(shape) ? event_for(index) : kChannelEvent;
			const bool sent = pc->NotifyAnimationGraph(event);
			state_active.store(sent, std::memory_order_relaxed);
			state_entered_ms.store(sent ? now_ms() : 0.0, std::memory_order_relaxed);
			combo_window.store(false, std::memory_order_relaxed);
			clip_committed.store(false, std::memory_order_relaxed);
			if (cast_entry_walks_clip_set(shape)) {
				logger::debug("SH2 cast: notified {} (clip {}) -> {}", event, index, sent);
			} else {
				logger::debug("SH2 cast: notified {} (held channel) -> {}", event, sent);
			}
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
		WritePrivateProfileStringA(nullptr, nullptr, nullptr, nullptr);
		curve.mechanic_on = GetPrivateProfileIntA("General", "ChargeMechanicOn", 1, ini) != 0;
		curve.exp_mode = GetPrivateProfileIntA("General", "ExpMode", 1, ini) != 0;
		curve.shortest = ini_float("ChargeTime", "Shortest", curve.shortest);
		curve.longest = ini_float("ChargeTime", "Longest", curve.longest);
		curve.base_time = ini_float("ChargeTime", "BaseTime", curve.base_time);
		curve.min_speed = ini_float("SpeedClamp", "MinSpeed", curve.min_speed);
		curve.max_speed = ini_float("SpeedClamp", "MaxSpeed", curve.max_speed);
		curve.exp_factor = ini_float("Exp", "ExpFactor", curve.exp_factor);
		const bool changed = curve.mechanic_on != g_curve.mechanic_on || curve.exp_mode != g_curve.exp_mode
			|| curve.shortest != g_curve.shortest || curve.longest != g_curve.longest
			|| curve.base_time != g_curve.base_time || curve.min_speed != g_curve.min_speed
			|| curve.max_speed != g_curve.max_speed || curve.exp_factor != g_curve.exp_factor;
		static bool logged_once = false;
		g_curve = curve;
		if (!logged_once || changed) {
			logged_once = true;
			logger::info(
				"SH2 cast: MSCO charge curve mechanic={} exp={} base={} short={} long={} min={} max={} p={}",
				curve.mechanic_on, curve.exp_mode, curve.base_time, curve.shortest, curve.longest,
				curve.min_speed, curve.max_speed, curve.exp_factor);
		}
	}

	bool combo_window_open()
	{
		return combo_window.load(std::memory_order_relaxed);
	}

	bool begin(RE::PlayerCharacter* pc, hand_mode hand, float charge_time, CastShape shape)
	{
		if (!pc) {
			return false;
		}
		(void)hand;
		// The interruption seam. The value of the swing this cast is cutting is taken here, live,
		// while the swing is still up: at that instant the graph still holds its index and MCO has
		// not stomped back to 1 yet. `IsAttacking` is the whole gate -- a begin() outside a swing
		// (the ShoutMCO deferred re-begin, an idle cast) would learn the stomp, so it records
		// nothing and the cache keeps its last WinClose/SGVI teaching.
		//
		// TICKET 29: what that live read MEANS depends on where in the clip the interrupt landed.
		// Before the clip's own advance it is the PLAYING index, and handing it on replays the
		// swing the player just interrupted; after the advance it is the successor. The open-swing
		// tracker tells the two apart, and the successor table -- filled only from what the clips
		// themselves taught -- supplies the successor when the sample is pre-advance. With nothing
		// learned yet the sampled value is kept, which is exactly the behaviour that shipped.
		{
			const int attacking_state = read_is_attacking(pc);
			McoCombo sample{};
			bool recorded = false;
			std::string light_note{ "not sampled" };
			std::string power_note{ "not sampled" };
			const auto open = g_swing.open_swing();
			if (should_sample_live_at_cast_begin(attacking_state == 1) && sample_mco(pc, sample)) {
				const int key = weapon_key(pc);
				const auto resolve = [&](McoSgviVariable kind, int& field) {
					const bool matching = open && open->kind == kind;
					if (!live_sample_is_pre_advance(matching, open ? open->playing : 0, field)) {
						return std::string{ "post-advance, keeping " } + std::to_string(field);
					}
					if (const auto successor = g_successors.lookup(key, kind, open->playing)) {
						const int was = field;
						field = *successor;
						return std::string{ "pre-advance, substituted=" } + std::to_string(*successor) +
							   " (was " + std::to_string(was) + ")";
					}
					return std::string{ "pre-advance, successor unknown, keeping " } +
						   std::to_string(field) + " (replay)";
				};
				light_note = resolve(McoSgviVariable::next_attack, sample.nextAttack);
				power_note = resolve(McoSgviVariable::next_power_attack, sample.nextPowerAttack);
				// Full record(): a real swing being interrupted also invalidates any restore
				// we were still holding, exactly as its own advance would have.
				g_rolling.record(sample, now_ms());
				recorded = true;
			}
			// One line per cast, not per event: this is the record that explains a wrong
			// combo restore after the fact, and it is the only place the pre/post-advance
			// call is made. Cheap enough to keep now that the per-event probes are gone.
			logger::debug(
				"SH2 cast: combo sample next={} power={} attacking={} recorded={} swing={} playing={} | next: {} | power: {}",
				sample.nextAttack, sample.nextPowerAttack, attacking_state, recorded,
				open ? "open"sv : "closed"sv, open ? open->playing : 0, light_note, power_note);
		}
		cast_shape.store(shape, std::memory_order_relaxed);
		channel_started_ms.store(
			shape == CastShape::channel ? now_ms() : 0.0, std::memory_order_relaxed);
		trace_budget.store(0, std::memory_order_relaxed);
		interrupt_left_caster_if_spell(pc);
		load_charge_curve();
		write_clip_speed(pc, charge_time);
		return send_entry(pc, shape);
	}

	void observe_graph_event(RE::Actor* a_player, const RE::BSFixedString& a_tag,
		const RE::BSFixedString& a_payload)
	{
		const std::string_view tag{ a_tag.c_str() ? a_tag.c_str() : "" };
		const std::string_view payload{ a_payload.c_str() ? a_payload.c_str() : "" };

		// TICKET 29: which swing is open. This runs FIRST, before the restore-consume branch at
		// the bottom of this function, because the answer to "did this swing's initiate consume a
		// restore" is only true until that branch runs -- read after it, every swing would look
		// like an ordinary one and a restore-taught (unverified) playing index would be taught to
		// the successor table as fact.
		//
		// A real swing opens the tracker at its own index; our own cast and art clips raise the
		// same initiates, and those CLOSE it instead -- there is no MCO swing behind them, and a
		// stale open swing would mislabel the next payload.
		if (tag == "MCO_AttackInitiate"sv || tag == "MCO_PowerAttackInitiate"sv) {
			const McoSgviVariable kind = tag == "MCO_AttackInitiate"sv
				? McoSgviVariable::next_attack
				: McoSgviVariable::next_power_attack;
			std::int32_t playing = 0;
			if (should_record_mco_combo_sample(is_active() || ArtDriver::is_active()) && a_player &&
				a_player->GetGraphVariableInt(RE::BSFixedString(var_name(kind)), playing)) {
				// Measured live: at the initiate the variable still reads as the swing that is
				// PLAYING, not its successor. That is precisely the index the clip's advance is
				// about to move on from, so it is the "from" side of every pair learned below.
				g_swing.open(kind, static_cast<int>(playing), g_rolling.restore_pending());
			} else {
				g_swing.close();
			}
		} else if (tag == "attackStop"sv || tag == "MCO_EndAnimation"sv) {
			// The swing is over. Anything SGVI arriving after this is the ready-state or
			// AttackState-exit reset -- which is also `@SGVI|MCO_nextattack|1` -- and recording
			// that is the exact stomp the rolling cache exists to outlive (ticket 11).
			g_swing.close();
		}

		// The clip's own advance, taken at the moment it is written. This is the primary edge:
		// a cast that interrupts a swing lands after the WinOpen-time advance but before
		// WinClose, so WinClose alone never learned the interrupted swing's teaching. The value
		// comes from the TAG, never from a graph read here -- the Payload Interpreter's write
		// may race this dispatch.
		if (const auto sgvi = parse_mco_sgvi_sample(tag)) {
			const bool pending = g_rolling.restore_pending();
			const bool record = !pending && should_record_mco_combo_sample(
				is_active() || ArtDriver::is_active());
			// Ticket-28 measurement: does an @SGVI payload reach this sink at all? One line
			// per tag, so the log answers that without a second run.
			logger::info("SH2 probe: sgvi {}={} pending={} record={}",
				sgvi->variable == McoSgviVariable::next_attack ? "MCO_nextattack"sv
															  : "MCO_nextpowerattack"sv,
				sgvi->value, pending, record);
			if (pending) {
				// Same rule as the window-close stomp-to-undo branch: while a restore is
				// pending the payload we are seeing is the ready reset's, not a swing's, so
				// put ours back rather than learning from it. Parity note: like that branch,
				// this does not distinguish a genuine real swing arriving while a restore is
				// still pending -- the same accepted gap, kept identical on purpose.
				if (const auto combo = g_rolling.peek()) {
					write_mco(a_player, *combo);
				}
			} else if (record) {
				if (sgvi->variable == McoSgviVariable::next_attack) {
					g_rolling.record_next_attack(sgvi->value, now_ms());
				} else {
					g_rolling.record_next_power_attack(sgvi->value, now_ms());
				}
			}
		}

		// TICKET 29: the same advance, arriving where it actually lands. The annotation
		// `PIE.@SGVI|...` splits into an event name (raised as `Pie`) and this payload, so
		// this -- not the tag branch above -- is the edge that fires under real packs.
		//
		// The gate is stricter than the tag branch's, and deliberately so. The ready-state reset
		// and the AttackState exit fire the SAME payload, `@SGVI|MCO_nextattack|1`. Most arrive
		// with no swing open and `IsAttacking` false -- but NOT all: when a swing is cut, the
		// AttackState-exit notify lands BEFORE attackStop (measured 2026-08-24 16:41, tracker
		// still open, IsAttacking still 1), so the value itself is the last line of defence:
		// `payload_advance_is_recordable` quarantines 1, the only value a reset can carry.
		if (const auto sgvi = parse_mco_sgvi_sample(payload)) {
			const bool pending = g_rolling.restore_pending();
			const int attacking_state = read_is_attacking(a_player);
			const auto open = g_swing.open_swing();
			const bool matching_kind = open && open->kind == sgvi->variable;
			std::string_view decision{ "ignored"sv };

			if (pending) {
				// Same rule as the tag branch and the window-close stomp-to-undo branch: while a
				// restore is pending, what we are seeing is the reset's payload, not a swing's.
				decision = "restore pending -> put ours back"sv;
				if (const auto combo = g_rolling.peek()) {
					write_mco(a_player, *combo);
				}
			} else if (matching_kind && attacking_state == 1 &&
					   !payload_advance_is_recordable(sgvi->value)) {
				// A cut swing's exit notify beats attackStop to this sink, so an open swing and a
				// live IsAttacking do NOT prove a clip advance. A reset always teaches 1 and a
				// clip can never teach itself, so 1 goes to the WinClose edge instead -- see
				// payload_advance_is_recordable.
				decision = "reset-valued (1) -> left to the WinClose edge"sv;
			} else if (matching_kind && attacking_state == 1) {
				if (sgvi->variable == McoSgviVariable::next_attack) {
					g_rolling.record_next_attack(sgvi->value, now_ms());
				} else {
					g_rolling.record_next_power_attack(sgvi->value, now_ms());
				}
				if (!open->taught_by_restore) {
					// The clip just said what follows the swing it is playing. That pair is the
					// only source of successor knowledge -- never arithmetic.
					g_successors.learn(weapon_key(a_player), sgvi->variable, open->playing,
						sgvi->value);
					decision = "recorded + learned successor"sv;
				} else {
					// This swing's initiate consumed a restore, so its playing index is what we
					// wrote rather than what the engine ran (2H currently ignores the write). The
					// advance is still the clip's own and worth recording; the PAIR is not
					// trustworthy and must not enter the table.
					decision = "recorded; restore-taught swing, no pair learned"sv;
				}
			} else if (!open) {
				decision = "no open swing (ready/AttackState reset) -> ignored"sv;
			} else if (!matching_kind) {
				decision = "open swing is the other kind -> ignored"sv;
			} else {
				decision = "IsAttacking not true -> ignored"sv;
			}

			logger::debug("SH2 cast: advance {}={} ({})",
				sgvi->variable == McoSgviVariable::next_attack ? kNextAttackVar
															  : kNextPowerAttackVar,
				sgvi->value, decision);
		}

		if (is_mco_combo_index_edge(tag)) {
			if (window_close_is_a_stomp_to_undo(g_rolling.restore_pending())) {
				// This window-close is usually our own cast clip's (MSCO_left2.hkx fires
				// MCO_winclose at 1.2s) carrying the ready reset's stomped value. Put ours
				// back instead; the next real swing consumes it.
				if (const auto combo = g_rolling.peek()) {
					write_mco(a_player, *combo);
				}
			} else if (should_record_mco_combo_sample(is_active() || ArtDriver::is_active())) {
				McoCombo sample{};
				if (sample_mco(a_player, sample)) {
					g_rolling.record(sample, now_ms());
					// TICKET 29 fallback teaching. A window-close arrives after the advance, so
					// the value it carries is the successor of the swing still open -- the same
					// pair the payload edge learns, one edge later. Only the open swing's OWN
					// kind is paired: a light swing's WinClose says nothing about what follows a
					// power attack, and pairing both would invent a power chain from a light one.
					if (const auto open = g_swing.open_swing(); open && !open->taught_by_restore) {
						const int taught = open->kind == McoSgviVariable::next_attack
							? sample.nextAttack
							: sample.nextPowerAttack;
						g_successors.learn(weapon_key(a_player), open->kind, open->playing, taught);
					}
					logger::debug("SH2 cast: sampled MCO next={} power={} at {}", sample.nextAttack,
						sample.nextPowerAttack, tag);
				} else {
					g_rolling.disarm();
				}
			}
		}

		if (is_msco_combo_window_open_event(tag) && is_active()) {
			const CastShape shape = cast_shape.load(std::memory_order_relaxed);
			combo_window.store(spellfire_opens_combo_window(shape), std::memory_order_relaxed);
			bool expected = false;
			if (clip_committed.compare_exchange_strong(expected, true, std::memory_order_relaxed) &&
				spellfire_advances_cast_index(shape)) {
				g_castIndex.advance();
			}
			logger::debug("SH2 cast: commitment point ({}), shape={}, window={}", tag,
				shape == CastShape::channel ? "channel" : "fnf", spellfire_opens_combo_window(shape));
		}

		if (is_msco_combo_window_close_event(tag)) {
			combo_window.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: combo window closed ({})", tag);
		}

		if (tag == "SH2_CastExit"sv) {
			if (is_active()) {
				arm_restore();
			}
			if (!clip_committed.load(std::memory_order_relaxed) &&
				exit_without_spellfire_is_a_dropped_press(cast_shape.load(std::memory_order_relaxed))) {
				logger::warn("SH2 cast: graph raised SH2_CastExit before SpellFire (clip {}); press produced no payload",
					g_castIndex.current());
				g_castIndex.reset();
			}
			state_active.store(false, std::memory_order_relaxed);
			combo_window.store(false, std::memory_order_relaxed);
			clip_committed.store(false, std::memory_order_relaxed);
			logger::debug("SH2 cast: state exiting (clip end or cancel)");
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

	void end_channel(RE::PlayerCharacter* pc)
	{
		// The state is live for the whole hold -- SH2_Channel_State plays a MODE_LOOPING clip and
		// has no end-of-clip trigger -- so this exit is what ends it. It also hands the combo
		// position on, so the swing after a hold continues the chain the hold interrupted rather
		// than starting at attack1. Sent unconditionally either way: the event reaches nothing when
		// the state is already gone.
		//
		// Discount the hold from the sample's age first. No sample is taken while our state is
		// live, so the position comes from the swing before the channel, and a hold longer than
		// kMaxAgeMs would otherwise age it out and reset the chain to attack1.
		if (const double started = channel_started_ms.exchange(0.0, std::memory_order_relaxed);
			started > 0.0) {
			const double held = now_ms() - started;
			g_rolling.credit_held_time(held);
			logger::debug("SH2 cast: channel held {:.0f}ms; discounted from the combo sample age", held);
		}
		arm_restore();
		send_exit(pc);
		clear_state_flags();
	}

	void arm_combo_restore()
	{
		arm_restore();
	}

	void cancel(RE::PlayerCharacter* pc)
	{
		if (is_active()) {
			trace_budget.store(post_cut_trace_events, std::memory_order_relaxed);
			arm_restore();
		}
		send_exit(pc);
		clear_state_flags();
	}

	void poll_watchdog(RE::PlayerCharacter* pc)
	{
		// Ticket 36: `notified SH2_CastExit -> false` is a measured, repeating event, and a graph
		// that refuses the exit never raises it back -- which used to leave the state live with
		// nothing able to clear it, and the input latch behind it retaining every press.
		if (!cast_state_watchdog_expired(is_active(),
				cast_shape.load(std::memory_order_relaxed) == CastShape::channel, now_ms(),
				state_entered_ms.load(std::memory_order_relaxed), kCastStateCapMs)) {
			return;
		}
		const double elapsed = now_ms() - state_entered_ms.load(std::memory_order_relaxed);
		logger::warn("SH2 cast: state watchdog expired after {:.0f}ms; clearing wedged cast state",
			elapsed);
		cancel(pc);
	}

	void finish(RE::PlayerCharacter* pc)
	{
		if (is_active()) {
			arm_restore();
		}
		send_exit(pc);
		clear_state_flags();
	}

	void reset_session()
	{
		clear_state_flags();
		clip_committed.store(false, std::memory_order_relaxed);
		trace_budget.store(0, std::memory_order_relaxed);
		g_castIndex.reset();
		// No swing survives a load. A stale open swing would label the first sample of the new
		// session against an index from the old one. The successor table is NOT cleared: it is
		// learned moveset shape keyed by weapon type, and that outlives a save the same way the
		// clips do.
		g_swing.close();
		ClipTranslationDriver::reset();
	}

	void interrupt_left_caster_if_spell(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return;
		}
		auto* left = pc->GetEquippedObject(true);
		const bool left_holds_spell =
			left && (left->Is(RE::FormType::Spell) || left->Is(RE::FormType::Scroll));
		if (!isolate_left_hand_caster_for_driver_cast(left_holds_spell)) {
			return;
		}
		if (auto* caster = pc->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand)) {
			caster->InterruptCast(true);
		}
		logger::debug("SH2: isolated left-hand caster (spell in left hand)");
	}
}
