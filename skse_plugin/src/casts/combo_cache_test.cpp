#include "combo_cache.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::casts::CastComboIndex;
using SpellHotbar::casts::HotbarCastPress;
using SpellHotbar::casts::McoCombo;
using SpellHotbar::casts::RollingMcoCombo;
using SpellHotbar::casts::capture_hotbar_press_to_prevent_dual_fire;
using SpellHotbar::casts::classify_hotbar_cast_press;
using SpellHotbar::casts::cut_committed_cast_for_left_hand_press;
using SpellHotbar::casts::isolate_left_hand_caster_before_vanilla_spellfire;
using SpellHotbar::casts::isolate_left_hand_caster_for_driver_cast;
using SpellHotbar::casts::keep_commitment_until_cut;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

void rolling_empty_is_not_usable()
{
	RollingMcoCombo cache;
	expect(!cache.usable(0.0).has_value(), "empty rolling combo is not usable");
}

void rolling_returns_the_recorded_value_not_plus_one()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 3, .nextPowerAttack = 2 }, 100.0);
	const auto got = cache.usable(100.0);
	expect(got.has_value(), "fresh record is usable");
	expect(got && got->nextAttack == 3, "preserve nextAttack, never derive +1");
	expect(got && got->nextPowerAttack == 2, "preserve nextPowerAttack");
}

void rolling_is_usable_at_the_age_cap()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 2, .nextPowerAttack = 1 }, 0.0);
	expect(cache.usable(RollingMcoCombo::kMaxAgeMs).has_value(), "usable at 5000ms");
	expect(!cache.usable(RollingMcoCombo::kMaxAgeMs + 1.0).has_value(), "stale at 5001ms");
}

void rolling_later_record_replaces_the_earlier()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 2, .nextPowerAttack = 1 }, 0.0);
	cache.record(McoCombo{ .nextAttack = 4, .nextPowerAttack = 3 }, 10.0);
	const auto got = cache.usable(10.0);
	expect(got && got->nextAttack == 4, "latest attack-time sample wins");
}

void restore_is_consumed_after_one_write()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 3, .nextPowerAttack = 2 }, 100.0);
	cache.arm(100.0);
	const auto peeked = cache.peek();
	expect(peeked && peeked->nextAttack == 3, "peek preserves the sampled index");
	expect(cache.peek().has_value(), "peek does not consume");
	const auto first = cache.consume();
	expect(first.has_value(), "armed restore is writable once");
	expect(first && first->nextAttack == 3, "first write preserves nextAttack");
	expect(first && first->nextPowerAttack == 2, "first write preserves nextPowerAttack");
	expect(!cache.consume().has_value(), "a later ready marker cannot rewrite the index");
	expect(!cache.peek().has_value(), "consumed restore is no longer pending");
}

void stale_sample_does_not_arm_a_restore()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 4, .nextPowerAttack = 3 }, 0.0);
	cache.arm(RollingMcoCombo::kMaxAgeMs + 1.0);
	expect(!cache.consume().has_value(), "stale sample writes nothing and leaves MCO at 1");
}

void empty_cache_does_not_arm_a_restore()
{
	RollingMcoCombo cache;
	cache.arm(0.0);
	expect(!cache.consume().has_value(), "no attack-time sample means no restore");
}

void a_new_swing_drops_a_pending_restore()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 2, .nextPowerAttack = 1 }, 0.0);
	cache.arm(0.0);
	cache.record(McoCombo{ .nextAttack = 3, .nextPowerAttack = 2 }, 10.0);
	expect(!cache.consume().has_value(), "a real swing consumes no leftover restore");
}

void disarm_drops_a_pending_restore_without_writing()
{
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 2, .nextPowerAttack = 1 }, 0.0);
	cache.arm(0.0);
	cache.disarm();
	expect(!cache.consume().has_value(), "disarm writes nothing");
}

void cast_index_starts_at_one_and_wraps_after_four()
{
	CastComboIndex idx;
	expect(idx.current() == 1, "first clip is 1");
	idx.advance();
	expect(idx.current() == 2, "second clip is 2");
	idx.advance();
	expect(idx.current() == 3, "third clip is 3");
	idx.advance();
	expect(idx.current() == 4, "fourth clip is 4");
	idx.advance();
	expect(idx.current() == 1, "wraps to 1, never derives past the clip set");
}

void cast_index_is_unchanged_by_an_attack_gap()
{
	CastComboIndex idx;
	idx.advance();
	expect(idx.current() == 2, "an intervening attack does not reset the cast index");
}

void idle_press_starts_an_ordinary_cast()
{
	expect(classify_hotbar_cast_press(false, false) == HotbarCastPress::start,
		"no live cast is an ordinary first press");
	expect(classify_hotbar_cast_press(false, true) == HotbarCastPress::start,
		"a stale holding flag cannot turn an idle press into a chain");
}

void committed_follow_up_press_chains()
{
	expect(classify_hotbar_cast_press(true, true) == HotbarCastPress::chain,
		"a second press during a committed cuttable Driver Cast is a combo step");
}

void live_cast_without_commitment_is_refused()
{
	expect(classify_hotbar_cast_press(true, false) == HotbarCastPress::refuse,
		"before spellfire, or a concentration channel, the second press is refused");
}

void chain_press_keeps_commitment_until_the_cut()
{
	expect(keep_commitment_until_cut(HotbarCastPress::chain),
		"a chain press still needs the commitment bit when start_cast runs the cut");
	expect(!keep_commitment_until_cut(HotbarCastPress::start),
		"an idle start must not inherit a leftover shout's spellfire");
	expect(!keep_commitment_until_cut(HotbarCastPress::refuse),
		"a refused press does not keep commitment");
}

void handled_hotbar_press_is_captured_when_the_left_hand_holds_a_spell()
{
	expect(capture_hotbar_press_to_prevent_dual_fire(true, true),
		"a Direct Cast with a spell in the left hand must not also reach vanilla");
	expect(!capture_hotbar_press_to_prevent_dual_fire(true, false),
		"without a left-hand spell there is no dual-fire to prevent");
	expect(!capture_hotbar_press_to_prevent_dual_fire(false, true),
		"an unhandled press is not captured by this rule");
}

void driver_cast_isolates_the_left_caster_when_that_hand_holds_a_spell()
{
	expect(isolate_left_hand_caster_for_driver_cast(true),
		"an in-progress left charge is cut when a Driver Cast begins");
	expect(!isolate_left_hand_caster_for_driver_cast(false),
		"an empty or weapon left hand has no equipped spell to isolate");
}

void driver_cast_isolates_before_vanilla_sees_left_spellfire()
{
	expect(isolate_left_hand_caster_before_vanilla_spellfire(true, true),
		"a live Driver Cast must isolate before vanilla processes the clip's left SpellFire");
	expect(!isolate_left_hand_caster_before_vanilla_spellfire(false, true),
		"an ordinary left-hand MSCO cast keeps vanilla SpellFire");
	expect(!isolate_left_hand_caster_before_vanilla_spellfire(true, false),
		"other graph events still reach vanilla during a Driver Cast");
}

void left_hand_press_cuts_a_committed_hotbar_cast()
{
	expect(cut_committed_cast_for_left_hand_press(true, true, true),
		"a left-hand FNF press during a committed Driver Cast ends the hotbar state");
	expect(!cut_committed_cast_for_left_hand_press(false, true, true),
		"before commitment the left-hand press keeps today's behaviour");
	expect(!cut_committed_cast_for_left_hand_press(true, false, true),
		"left attack with a weapon or shield is block, not an MSCO hand cast");
	expect(!cut_committed_cast_for_left_hand_press(true, true, false),
		"an unrelated key does not cut");
}

}  // namespace

int main()
{
	rolling_empty_is_not_usable();
	rolling_returns_the_recorded_value_not_plus_one();
	rolling_is_usable_at_the_age_cap();
	rolling_later_record_replaces_the_earlier();
	restore_is_consumed_after_one_write();
	stale_sample_does_not_arm_a_restore();
	empty_cache_does_not_arm_a_restore();
	a_new_swing_drops_a_pending_restore();
	disarm_drops_a_pending_restore_without_writing();
	cast_index_starts_at_one_and_wraps_after_four();
	cast_index_is_unchanged_by_an_attack_gap();
	idle_press_starts_an_ordinary_cast();
	committed_follow_up_press_chains();
	live_cast_without_commitment_is_refused();
	chain_press_keeps_commitment_until_the_cut();
	handled_hotbar_press_is_captured_when_the_left_hand_holds_a_spell();
	driver_cast_isolates_the_left_caster_when_that_hand_holds_a_spell();
	driver_cast_isolates_before_vanilla_sees_left_spellfire();
	left_hand_press_cuts_a_committed_hotbar_cast();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
