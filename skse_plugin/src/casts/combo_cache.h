#pragma once

#include <array>
#include <cmath>
#include <cstddef>
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

	// The clip teaches its two counters in SEPARATE `@SGVI` events, so the SGVI sampling path
	// can only ever update one field at a time. The other field keeps its last known value
	// rather than being reset: an attack clip that writes only `MCO_nextattack` must not blank
	// the power chain's position. Otherwise these are `record()` -- same timestamp refresh,
	// same valid_ latch, same pending_ clear, under the same mutex, because a real swing
	// teaching its advance invalidates a restore we were holding just as a full sample does.
	void record_next_attack(int nextAttack, double nowMs)
	{
		std::lock_guard lock{ mutex_ };
		valid_ = true;
		takenAtMs_ = nowMs;
		sample_.nextAttack = nextAttack;
		pending_ = false;
	}

	void record_next_power_attack(int nextPowerAttack, double nowMs)
	{
		std::lock_guard lock{ mutex_ };
		valid_ = true;
		takenAtMs_ = nowMs;
		sample_.nextPowerAttack = nextPowerAttack;
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

	[[nodiscard]] bool restore_pending() const
	{
		std::lock_guard lock{ mutex_ };
		return pending_;
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

	// A held channel is one continuous action, not a gap between actions. kMaxAgeMs measures
	// how long the player has been OUT of the chain, so the hold itself must not count against
	// it: a concentration channel is unbounded, so any hold past five seconds would age the
	// sample out and the chain could never continue (ticket 28). Crediting the held time keeps
	// the cap doing its real job -- refusing a combo sampled in an earlier fight -- while a
	// hold of any length hands its position on.
	void credit_held_time(double heldMs)
	{
		std::lock_guard lock{ mutex_ };
		if (!valid_ || heldMs <= 0.0) {
			return;
		}
		takenAtMs_ += heldMs;
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

// The SGVI payload is the primary sampling edge; window-close is the retained fallback.
//
// The advance itself is a clip annotation, `@SGVI|MCO_nextattack|<N>`, and its TIME is pack data
// rather than an MCO guarantee. Thuum's reference clips annotate at t=0.06 (before HitFrame);
// this load order's stance packs (Elder Creed - Blade, Mercenary Greatsword; dumps 2026-08-24)
// annotate AT `MCO_WinOpen`. Measured live 2026-08-24 over a four-hit chain, every `MCO_WinClose`
// carries the index the NEXT swing will use (1, 2, 3, 4), and each following `MCO_AttackInitiate`
// reads that value back as the swing it is playing -- so WinClose really is always after the
// advance, and attack-time edges are one step behind.
//
// What WinClose alone could not survive is an INTERRUPTION (measured 2026-08-24; sword attack2
// advances at 0.633s and closes at 1.467s). A hotbar cast during a swing lands between those two
// moments: the advance has already happened but WinClose never fires, so the cache still holds
// the PREVIOUS swing's teaching and the restore repeats the attack that was interrupted -- owner-
// observed on both 1H and greatsword. Sampling the `@SGVI` payload itself takes the value at the
// moment the clip writes it, so an interrupted swing has already taught its advance.
//
// The advance's VALUE is parsed out of the annotation text, never read back off the graph at that
// moment: the Payload Interpreter's own write may race the event dispatch. That keeps ticket 11's
// "preserve, never derive" intact -- the number is the clip's own, and moveset length still lives
// only in the annotations.
//
// WHERE that text arrives is the part that was wrong for a whole ticket (ticket 29, finding 1).
// The clip annotation is `PIE.@SGVI|MCO_nextattack|3`, and the engine splits it at the FIRST `.`:
// everything before is the event NAME (`PIE`), everything after is the event PAYLOAD
// (`@SGVI|MCO_nextattack|3`). `RE::BSAnimationGraphEvent` carries the two in separate members, and
// the sink hook forwarded only `tag`. So this parser was fed event names for its whole life and
// measured zero hits -- which was read at the time as "these packs emit no SGVI", when in truth
// the advance was arriving every swing in a field nobody passed on. The hook now forwards
// `payload` too, and the parser runs over both: a pack that puts the annotation in the event name
// (no `.`) and a pack that puts it in the payload both reach the same sampler.
//
// WinClose stays an edge as a fallback: if `@SGVI` never parses out of either field under some
// pack, behaviour degrades to exactly what shipped before. A WinClose arriving after an SGVI
// simply re-records the same value.
//
// `attackStop` is deliberately NOT an edge, though it carries a value. It is MCO's end-of-swing
// reset to 1 -- the stomp this whole rolling cache exists to outlive (ticket 11: "already ended
// and stomped MCO_nextattack to 1 ... this is why thuum carries a RollingCombo"). Sampling it
// overwrites the good index with the reset and restores 1, measured live before it was excluded.
[[nodiscard]] constexpr bool is_mco_combo_index_edge(std::string_view tag) noexcept
{
	return is_msco_combo_window_close_event(tag);
}

// `@SGVI|<variable>|<integer>` -- the Payload Interpreter's set-graph-variable-int annotation, as
// it arrives at the animation-event sink. The argument is deliberately named for neither field:
// depending on how the pack wrote the annotation this text is the event's TAG (no `.` in the
// annotation) or its PAYLOAD (everything after the first `.`), and the caller feeds it both.
// The variable name is matched IN FULL: `msco_nextright`
// and other variables sharing a prefix must be rejected, or an unrelated clip counter would be
// learned as a combo position. Allocation-free so it can run on the animation thread.
[[nodiscard]] constexpr std::optional<int> parse_sgvi_int(
	std::string_view tag, std::string_view variable) noexcept
{
	constexpr std::string_view kPrefix{ "@SGVI|" };
	if (variable.empty() || tag.size() <= kPrefix.size() ||
		tag.substr(0, kPrefix.size()) != kPrefix) {
		return std::nullopt;
	}
	const std::string_view rest = tag.substr(kPrefix.size());
	if (rest.size() <= variable.size() || rest.substr(0, variable.size()) != variable ||
		rest[variable.size()] != '|') {
		return std::nullopt;
	}
	const std::string_view digits = rest.substr(variable.size() + 1);
	if (digits.empty()) {
		return std::nullopt;
	}
	std::size_t i = 0;
	bool negative = false;
	if (digits[0] == '-' || digits[0] == '+') {
		negative = digits[0] == '-';
		i = 1;
		if (digits.size() == 1) {
			return std::nullopt;
		}
	}
	int value = 0;
	for (; i < digits.size(); ++i) {
		const char c = digits[i];
		if (c < '0' || c > '9') {
			return std::nullopt;
		}
		value = value * 10 + (c - '0');
	}
	return negative ? -value : value;
}

// Which of the two counters a clip just taught. They arrive as separate events, so the cache
// takes them as separate partial updates.
enum class McoSgviVariable {
	next_attack,
	next_power_attack,
};

struct McoSgviSample {
	McoSgviVariable variable = McoSgviVariable::next_attack;
	int value = 1;
};

[[nodiscard]] constexpr std::optional<McoSgviSample> parse_mco_sgvi_sample(
	std::string_view tag) noexcept
{
	if (const auto next = parse_sgvi_int(tag, "MCO_nextattack")) {
		return McoSgviSample{ McoSgviVariable::next_attack, *next };
	}
	if (const auto power = parse_sgvi_int(tag, "MCO_nextpowerattack")) {
		return McoSgviSample{ McoSgviVariable::next_power_attack, *power };
	}
	return std::nullopt;
}

// Which swing is currently up, and why anything needs to know (ticket 29).
//
// Measured live 2026-08-24 (finding 2): at `MCO_AttackInitiate` the graph's `MCO_nextattack` reads
// back as the index of the swing that is PLAYING, not its successor. The clip's own `@SGVI`
// advance overwrites it later, mid-clip. So one live read at cast begin() means two opposite
// things depending only on WHEN in the clip the interrupt landed:
//
//   * before the advance -- the value IS the playing index, and handing it on replays the very
//     swing the player just interrupted. That is the ticket-29 bug, owner-observed on 1H and 2H.
//   * after the advance -- the value is the successor, which is exactly what should be handed on.
//
// Nothing in the number distinguishes the two. Knowing which swing is open does: equal to the
// playing index means pre-advance, different means post-advance.
//
// `taught_by_restore` marks a swing whose initiate consumed a pending restore. On 2H the engine
// currently ignores the restored value (separate open ticket), so that swing's playing index is
// UNVERIFIED -- it may not be the index we wrote. Its advance is still real and worth recording,
// but the pair (playing -> advance) must not be taught to the successor table, or one bad restore
// poisons the table for every later swing at that index.
//
// Opened on the animation thread, read on the input/main thread at begin(), so every access takes
// the mutex.
class McoSwingTracker {
public:
	static constexpr int kMinIndex = 1;
	static constexpr int kMaxIndex = 10;

	struct OpenSwing {
		McoSgviVariable kind = McoSgviVariable::next_attack;
		int playing = 1;
		bool taught_by_restore = false;
	};

	// An index outside the plausible moveset range is not a combo position -- the read failed, or
	// the graph holds something that is not a swing index. Rather than remember a value we cannot
	// reason about, forget the swing entirely: a closed tracker degrades to today's behaviour,
	// while a wrong playing index would mislabel a post-advance sample as a replay.
	void open(McoSgviVariable kind, int playing, bool taught_by_restore)
	{
		std::lock_guard lock{ mutex_ };
		if (playing < kMinIndex || playing > kMaxIndex) {
			open_ = false;
			return;
		}
		open_ = true;
		swing_ = OpenSwing{ kind, playing, taught_by_restore };
	}

	void close()
	{
		std::lock_guard lock{ mutex_ };
		open_ = false;
	}

	[[nodiscard]] std::optional<OpenSwing> open_swing() const
	{
		std::lock_guard lock{ mutex_ };
		if (!open_) {
			return std::nullopt;
		}
		return swing_;
	}

private:
	mutable std::mutex mutex_;
	bool open_ = false;
	OpenSwing swing_{};
};

// What follows what, learned from the clips themselves (ticket 29).
//
// The successor of attack2 is NOT "3". Movesets wrap at their own length, stance packs reorder
// steps, and power chains are shorter than light ones -- deriving the next index arithmetically
// is the exact mistake ticket 11 was written to stop. So the table is filled only from a clip's
// own teaching: swing N was playing when the clip wrote M, therefore on this weapon, for this
// kind, N is followed by M. Both the payload edge and the WinClose fallback teach the same way.
//
// Keyed by weapon type because the movesets differ per weapon, and a greatsword's wrap must not
// answer for a dagger's. `weaponKey` is `RE::WEAPON_TYPE` cast to int (0..9, hand-to-hand through
// crossbow); anything else is refused rather than folded onto a neighbour's row.
//
// Plain arrays, zero-initialised, no allocation: this is written from the animation thread.
// Zero means "not taught yet", which is why index 0 of each row is never a valid entry.
class McoSuccessorTable {
public:
	static constexpr int kWeaponKeys = 10;
	static constexpr int kMinIndex = McoSwingTracker::kMinIndex;
	static constexpr int kMaxIndex = McoSwingTracker::kMaxIndex;

	void learn(int weaponKey, McoSgviVariable kind, int from, int to)
	{
		if (!in_range(weaponKey, from) || to < kMinIndex || to > kMaxIndex) {
			return;
		}
		std::lock_guard lock{ mutex_ };
		table_[static_cast<std::size_t>(weaponKey)][kind_index(kind)]
			  [static_cast<std::size_t>(from)] = to;
	}

	[[nodiscard]] std::optional<int> lookup(int weaponKey, McoSgviVariable kind, int from) const
	{
		if (!in_range(weaponKey, from)) {
			return std::nullopt;
		}
		std::lock_guard lock{ mutex_ };
		const int to = table_[static_cast<std::size_t>(weaponKey)][kind_index(kind)]
							 [static_cast<std::size_t>(from)];
		if (to == 0) {
			return std::nullopt;
		}
		return to;
	}

private:
	[[nodiscard]] static constexpr bool in_range(int weaponKey, int from) noexcept
	{
		return weaponKey >= 0 && weaponKey < kWeaponKeys && from >= kMinIndex && from <= kMaxIndex;
	}

	[[nodiscard]] static constexpr std::size_t kind_index(McoSgviVariable kind) noexcept
	{
		return kind == McoSgviVariable::next_attack ? 0u : 1u;
	}

	mutable std::mutex mutex_;
	std::array<std::array<std::array<int, kMaxIndex + 1>, 2>, kWeaponKeys> table_{};
};

// The whole ticket-29 discriminator, in one line, so it can be tested without a graph.
//
// A live sample equal to the open swing's playing index is PRE-advance: the clip has not written
// its successor yet, and handing this value on replays the interrupted swing. Different means the
// advance already landed and the value is the successor we want. With no open swing of that kind
// there is nothing to compare against, so the sample is taken as-is -- today's behaviour.
[[nodiscard]] constexpr bool live_sample_is_pre_advance(
	bool swing_open_matching_kind, int playing, int sampled) noexcept
{
	return swing_open_matching_kind && sampled == playing;
}

// The cast-begin live read, and why it is gated on IsAttacking.
//
// The `@SGVI` path above is the seam that survives an interruption, and it measured zero hits on
// 2026-08-24 -- because the sink hook forwarded only the event NAME, and the annotation's text
// lives in the PAYLOAD (ticket 29, finding 1). This predicate was written to close the same blind
// spot without it. Both run now: the payload edge takes the advance the moment the clip writes it,
// and this live read still covers a pack whose annotation the parser cannot read at all.
//
// Live reads are not interchangeable with the payload edge, which is the rest of ticket 29: a live
// read taken BEFORE the clip's advance returns the playing index, so restoring it replays the
// interrupted swing. `live_sample_is_pre_advance` above is what tells the two apart, and the
// successor table is what fixes it.
//
// The seam that DOES fire is `MscoCastDriver::begin()`, the moment a hotbar cast starts. Measured
// live 2026-08-24: a cast begun mid-swing logs `IsAttacking=1` and the graph still holds the
// interrupted swing's ADVANCED value (n=3 while attack2 was playing) -- MCO's stomp back to 1
// happens only after the swing is cut. The same cast re-begun through the ShoutMCO deferral 272ms
// later logs `IsAttacking=0` and reads the post-stomp 1.
//
// So a live read at begin() is trustworthy exactly when the player is attacking at that instant.
// A begin() with IsAttacking=0 -- the deferred re-begin, a cast from idle, any post-stomp tail --
// must record nothing, so the cache keeps whatever the WinClose or SGVI edges last taught it.
[[nodiscard]] constexpr bool should_sample_live_at_cast_begin(bool is_attacking) noexcept
{
	return is_attacking;
}

// What a window-close means depends on whether we are holding a restore.
//
// Measured live 2026-08-24: after a Driver Cast writes the sampled index back, a further
// `MCO_WinClose` arrives carrying 1. The EVENT is our own borrowed cast clip's -- the ready
// reset fires @SGVI payloads, never a WinClose, and MSCO_left2.hkx carries `MCO_winclose` at
// 1.2s (annotation dump, 2026-08-24). The VALUE 1 it carried is the ready reset's stomp read
// at that moment. Sampling it both learned the wrong value AND dropped the pending restore
// (record() clears it, by design, because a real SWING invalidates one). A reset is not a
// swing. So while a restore is pending, a window-close is a stomp to put back, not a value to
// learn from; the restore stays authoritative until an actual attack consumes it.
[[nodiscard]] constexpr bool window_close_is_a_stomp_to_undo(bool restore_pending) noexcept
{
	return restore_pending;
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

// A Driver Cast or Ability interrupts the left-hand MagicCaster when that
// hand holds a spell, so an in-progress MSCO left charge cannot stay
// IsCasting after the shtb clip takes over (sheathe was the only reset).
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

// What the shtb cast state is being used for. A fire-and-forget cast owns the state for the
// whole clip and leaves when the clip ends. A concentration channel owns its own state for the
// whole hold: SH2_Channel_State plays a MODE_LOOPING generator on the shout-inhale path, which
// is the one clip every non-idle conc submod replaces, so OAR still picks the per-family clip
// from SpellHotbar_SpellAnimationType (ADR-0013, ticket 28 spike).
enum class CastShape {
	fire_and_forget,
	channel,
};

// Which notify enters the state. A fire-and-forget press walks the MSCO_left1..left4 clip set,
// so its event comes from the cast index; a channel has one entry of its own and never touches
// that set. Kept here rather than in the driver so the split is testable without a graph.
[[nodiscard]] constexpr bool cast_entry_walks_clip_set(CastShape shape) noexcept
{
	return shape == CastShape::fire_and_forget;
}

// SH2_CastExit arriving with no SpellFire behind it means a fire-and-forget press produced no
// payload -- the clip was cut before it committed -- and the cast index must go back to 1. A
// channel reaches its exit that way by design: it commits on ADR-0006's authored cast-time floor
// rather than on a clip annotation, so treating its exit as a dropped press would reset the
// fire-and-forget combo position after every hold.
[[nodiscard]] constexpr bool exit_without_spellfire_is_a_dropped_press(CastShape shape) noexcept
{
	return shape == CastShape::fire_and_forget;
}

// SH2's cast-combo index walks the MSCO_left1..left4 clip set. A channel is one cast held, not
// a chain step, so its SpellFire commits the cast without moving the index (ticket 11).
[[nodiscard]] constexpr bool spellfire_advances_cast_index(CastShape shape) noexcept
{
	return shape == CastShape::fire_and_forget;
}

// Nor does a channel's SpellFire open the follow-up-press window. That window is an envelope
// inside a clip that is still playing; a channel's start clip has already handed off, and a
// second hotbar cast during a hold is a refusal, not a chain.
[[nodiscard]] constexpr bool spellfire_opens_combo_window(CastShape shape) noexcept
{
	return shape == CastShape::fire_and_forget;
}

// A channel's chain-out window is the hold itself. It opens when the channel commits and the
// spell starts streaming, and it closes when the player lets go. There is no clip clock to
// read here, because the start clip ended long before the hold does.
[[nodiscard]] constexpr bool channel_chain_window_open(
	bool channel_live, bool channel_streaming) noexcept
{
	return channel_live && channel_streaming;
}

// An attack inside that window ends the channel and becomes the swing. The press itself is not
// captured: it travels the rest of the dispatch, the same fail-safe ticket 10 uses.
[[nodiscard]] constexpr bool should_cut_channel_for_attack(
	bool channel_chain_open, bool is_attack_press) noexcept
{
	return channel_chain_open && is_attack_press;
}

// Combo position must survive the hold: the swing after a channel continues the chain the
// channel interrupted, through the same ADR-0005 write-back a Driver Cast arms at SH2_CastExit.
// A channel that never started streaming has nothing to hand off.
[[nodiscard]] constexpr bool channel_end_arms_combo_restore(bool channel_streaming) noexcept
{
	return channel_streaming;
}

// Is a sample still usable at the end of a hold? `sampleAgeMs` is wall-clock age; `heldMs` is
// the part of it spent holding the channel, which does not count. Time between the sampled
// swing and the start of the hold still does.
[[nodiscard]] constexpr bool combo_sample_survives_hold(double sampleAgeMs, double heldMs) noexcept
{
	return (sampleAgeMs - heldMs) <= RollingMcoCombo::kMaxAgeMs;
}

// A hotbar shout that is allowed to start must leave SH2_Art_State / the
// Driver Cast before the Shout ButtonEvent, or both clips play together.
[[nodiscard]] constexpr bool should_yield_shtb_before_hotbar_shout(
	bool our_shtb_busy, bool local_latch_open) noexcept
{
	return our_shtb_busy && local_latch_open;
}

}  // namespace SpellHotbar::casts
