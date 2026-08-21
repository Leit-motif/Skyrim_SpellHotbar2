#pragma once

#include <cmath>
#include <mutex>
#include <optional>
#include <string_view>

namespace SpellHotbar::casts {

struct McoCombo {
	int nextAttack = 1;
	int nextPowerAttack = 1;
};

// Last MCO combo position taken at an attack-time event. The value is preserved, never
// derived: a read of 3 is written back as 3. Age is the discriminator that a live read
// at cast start cannot be, because the ready-state payload has already stomped the
// variable to 1 by then.
//
// Capture (animation thread) and arm (main thread, on cancel/finish) share this object,
// so every mutation takes the mutex. consume() is one-shot: the first ready pass after
// a Driver Cast writes the sampled index, and a later ready/PIE cannot replay it.
class RollingMcoCombo {
public:
	static constexpr double kMaxAgeMs = 5000.0;

	void record(McoCombo sample, double nowMs)
	{
		std::lock_guard lock{ mutex_ };
		valid_ = true;
		takenAtMs_ = nowMs;
		sample_ = sample;
		pending_ = false;
	}

	[[nodiscard]] std::optional<McoCombo> usable(double nowMs) const
	{
		std::lock_guard lock{ mutex_ };
		return usable_locked(nowMs);
	}

	std::optional<McoCombo> arm(double nowMs)
	{
		std::lock_guard lock{ mutex_ };
		const auto combo = usable_locked(nowMs);
		if (!combo) {
			pending_ = false;
			return std::nullopt;
		}
		restore_ = *combo;
		pending_ = true;
		return restore_;
	}

	[[nodiscard]] std::optional<McoCombo> peek() const
	{
		std::lock_guard lock{ mutex_ };
		if (!pending_) {
			return std::nullopt;
		}
		return restore_;
	}

	[[nodiscard]] std::optional<McoCombo> consume()
	{
		std::lock_guard lock{ mutex_ };
		if (!pending_) {
			return std::nullopt;
		}
		pending_ = false;
		return restore_;
	}

	void disarm()
	{
		std::lock_guard lock{ mutex_ };
		pending_ = false;
	}

	void clear()
	{
		std::lock_guard lock{ mutex_ };
		valid_ = false;
		pending_ = false;
	}

private:
	[[nodiscard]] std::optional<McoCombo> usable_locked(double nowMs) const
	{
		if (!valid_) {
			return std::nullopt;
		}
		if (nowMs - takenAtMs_ > kMaxAgeMs) {
			return std::nullopt;
		}
		return sample_;
	}

	mutable std::mutex mutex_;
	bool valid_ = false;
	bool pending_ = false;
	double takenAtMs_ = 0.0;
	McoCombo sample_{};
	McoCombo restore_{};
};

// SH2's own cast combo. Independent of MCO_nextattack: advances on a cast, wraps at the
// clip-set length, and is not reset by an attack.
class CastComboIndex {
public:
	static constexpr int kLength = 4;

	[[nodiscard]] int current() const { return index_; }

	void advance() { index_ = index_ % kLength + 1; }

	void reset() { index_ = 1; }

private:
	int index_ = 1;
};

// Public hotbar path: a second press during a committed Driver Cast's SpellFire-to-WinClose
// window is a combo step, not a refusal. Concentration and pre-spellfire are excluded by the
// holding flag the caller already computed (is_committed_cast_holding_graph); the separate
// window bit refuses late presses after WinClose until CastExit ends the instance.
enum class HotbarCastPress {
	start,
	chain,
	refuse,
};

[[nodiscard]] constexpr HotbarCastPress classify_hotbar_cast_press(
	bool has_live_cast, bool committed_cuttable_holding_graph, bool combo_window_open) noexcept
{
	if (!has_live_cast) {
		return HotbarCastPress::start;
	}
	if (committed_cuttable_holding_graph && combo_window_open) {
		return HotbarCastPress::chain;
	}
	return HotbarCastPress::refuse;
}

[[nodiscard]] constexpr bool is_msco_combo_window_open_event(std::string_view tag) noexcept
{
	return tag == "MLh_SpellFire_Event";
}

[[nodiscard]] constexpr bool is_msco_combo_window_close_event(std::string_view tag) noexcept
{
	return tag == "MSCO_WinClose" || tag == "MCO_WinClose" || tag == "MSCO_winclose" ||
		   tag == "MCO_winclose";
}

// MSCO v2 charge time → clip playback speed. Defaults match the shipped MSCO.ini
// exponential curve (Nexus 168499 / Desmos px706ivga2).
struct MscoChargeCurve {
	bool mechanic_on = true;
	bool exp_mode = true;
	float shortest = 0.0f;
	float longest = 2.0f;
	float base_time = 0.15f;
	float min_speed = 0.6f;
	float max_speed = 1.25f;
	float exp_factor = 0.17f;
};

[[nodiscard]] inline float charge_time_to_anim_speed(float charge, const MscoChargeCurve& curve) noexcept
{
	if (!curve.mechanic_on) {
		return 1.0f;
	}
	float t = charge;
	if (t < curve.shortest) {
		t = curve.shortest;
	}
	if (t > curve.longest) {
		t = curve.longest;
	}
	float speed = 1.0f;
	if (curve.exp_mode) {
		const float x = t < 1.0e-6f ? 1.0e-6f : t;
		speed = std::pow(curve.base_time / x, curve.exp_factor);
	} else {
		const float o1 = (curve.base_time - curve.shortest) > 1.0e-6f
			? (curve.base_time - curve.shortest)
			: 1.0e-6f;
		const float o2 = (curve.longest - curve.base_time) > 1.0e-6f
			? (curve.longest - curve.base_time)
			: 1.0e-6f;
		const float p1 = 1.0f + (curve.max_speed - 1.0f) * (curve.base_time - t) / o1;
		const float p2 = 1.0f - (1.0f - curve.min_speed) * (t - curve.base_time) / o2;
		speed = (t < curve.base_time) ? p1 : p2;
	}
	if (speed < curve.min_speed) {
		return curve.min_speed;
	}
	if (speed > curve.max_speed) {
		return curve.max_speed;
	}
	return speed;
}

// The cut still reads commitment. Clearing spellfire between classify and start_cast
// turns a chain press into a refuse against the live instance.
[[nodiscard]] constexpr bool keep_commitment_until_cut(HotbarCastPress press) noexcept
{
	return press == HotbarCastPress::chain;
}

// A handled spell-slot press must not also reach vanilla when the left hand
// holds a spell: the borrowed clip raises MLh_SpellFire_Event, and an
// uncaptured hotbar key is vanilla Hotkey1.
[[nodiscard]] constexpr bool capture_hotbar_press_to_prevent_dual_fire(
	bool handled_spell_slot, bool left_hand_holds_spell) noexcept
{
	return handled_spell_slot && left_hand_holds_spell;
}

// A Driver Cast interrupts the left-hand MagicCaster when that hand holds a
// spell, so an in-progress left charge cannot complete alongside the payload.
// Idle casters are a no-op; clip SpellFire is isolated separately.
[[nodiscard]] constexpr bool isolate_left_hand_caster_for_driver_cast(
	bool left_hand_holds_spell) noexcept
{
	return left_hand_holds_spell;
}

// InterruptCast at begin() is too early: the left caster is idle, and the
// borrowed clip's SpellFire is ~0.5s later. Vanilla also processes that
// event before this plugin's observer. Isolate immediately before vanilla
// sees left SpellFire while a Driver Cast is live.
[[nodiscard]] constexpr bool isolate_left_hand_caster_before_vanilla_spellfire(
	bool driver_cast_active, bool is_left_spellfire) noexcept
{
	return driver_cast_active && is_left_spellfire;
}

// The left control is block when that hand holds a weapon or shield. A
// left-hand spell press during a committed Driver Cast ends the hotbar state
// the same way an attack press does.
[[nodiscard]] constexpr bool cut_committed_cast_for_left_hand_press(
	bool committed_holding_graph, bool left_hand_holds_spell, bool is_left_attack_key) noexcept
{
	return committed_holding_graph && left_hand_holds_spell && is_left_attack_key;
}

// WASD capture follows the shtb state, not the cast instance. A ticket-10 cut
// ends the state while the instance may still be alive for a frame; translation
// must resume then so the swing can travel. Consecutive clips never send
// CastExit, so they stay planted. Abilities reuse this plant: SH2_Art_State
// is an shtb state the same way the cast clips are.
[[nodiscard]] constexpr bool shtb_state_blocks_movement(bool shtb_state_active) noexcept
{
	return shtb_state_active;
}

[[nodiscard]] constexpr bool driver_cast_blocks_movement(
	bool shtb_state_active, bool has_live_cast_instance) noexcept
{
	(void)has_live_cast_instance;
	return shtb_state_blocks_movement(shtb_state_active);
}

// ADR-0006: the SpellFire annotation leads; the authored cast time is the
// floor only as a missing-annotation fallback. Timer expiry while the clip
// is still playing is not that fallback — clip 4's SpellFire is at ~0.92s,
// past a 0.5s authored time, so a live clip waits for the pose.
enum class CastDelivery {
	wait,
	deliver,
	cancel,
};

[[nodiscard]] constexpr CastDelivery classify_cast_delivery(
	bool already_delivered, bool timer_expired, bool spellfire_seen, bool anim_ok) noexcept
{
	if (already_delivered) {
		return CastDelivery::wait;
	}
	if (spellfire_seen) {
		return CastDelivery::deliver;
	}
	if (anim_ok) {
		return CastDelivery::wait;
	}
	return timer_expired ? CastDelivery::deliver : CastDelivery::cancel;
}

// Ability latch: WinOpen if the bound clip carries it, else HitFrame, else SH2_ArtExit.
enum class AbilityLatch {
	winOpen,
	hitFrame,
	artExit,
};

[[nodiscard]] constexpr AbilityLatch classify_ability_latch(
	bool has_win_open, bool has_hit_frame) noexcept
{
	if (has_win_open) {
		return AbilityLatch::winOpen;
	}
	if (has_hit_frame) {
		return AbilityLatch::hitFrame;
	}
	return AbilityLatch::artExit;
}

[[nodiscard]] constexpr bool is_ability_win_open_event(std::string_view tag) noexcept
{
	return tag == "MSCO_WinOpen" || tag == "MCO_WinOpen" || tag == "MSCO_winopen" ||
		   tag == "MCO_winopen";
}

[[nodiscard]] constexpr bool is_ability_hit_frame_event(std::string_view tag) noexcept
{
	return tag == "HitFrame";
}

[[nodiscard]] constexpr bool is_ability_latch_event(AbilityLatch latch, std::string_view tag) noexcept
{
	switch (latch) {
	case AbilityLatch::winOpen:
		return is_ability_win_open_event(tag);
	case AbilityLatch::hitFrame:
		return is_ability_hit_frame_event(tag);
	case AbilityLatch::artExit:
		return tag == "SH2_ArtExit";
	}
	return false;
}

// Our Driver Cast or Ability owns release until its latch opens. Concentration is
// excluded. Someone else's swing is ShoutMCO's clock (ADR-0005).
[[nodiscard]] constexpr bool should_retain_local_cast_intent(
	bool our_shtb_busy, bool local_latch_open, bool concentration_live) noexcept
{
	if (concentration_live || !our_shtb_busy) {
		return false;
	}
	return !local_latch_open;
}

[[nodiscard]] constexpr bool should_cut_ability_for_attack(
	bool art_active, bool latch_open, bool is_attack_press) noexcept
{
	return art_active && latch_open && is_attack_press;
}

[[nodiscard]] constexpr bool should_capture_attack_during_ability(
	bool art_active, bool latch_open, bool is_attack_press) noexcept
{
	return art_active && !latch_open && is_attack_press;
}

// HitFrame / AttackInitiate on our Driver Cast or Ability are not a new MCO
// swing. Recording them would replace the pre-Ability sample with 1 and the
// recovery attack would restart the combo.
[[nodiscard]] constexpr bool should_record_mco_combo_sample(bool our_shtb_busy) noexcept
{
	return !our_shtb_busy;
}

// A hotbar shout that is allowed to start must leave SH2_Art_State / the
// Driver Cast before the Shout ButtonEvent, or both clips play together.
[[nodiscard]] constexpr bool should_yield_shtb_before_hotbar_shout(
	bool our_shtb_busy, bool local_latch_open) noexcept
{
	return our_shtb_busy && local_latch_open;
}

}  // namespace SpellHotbar::casts
