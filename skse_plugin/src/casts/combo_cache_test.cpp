#include "combo_cache.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::casts::CastComboIndex;
using SpellHotbar::casts::CastDelivery;
using SpellHotbar::casts::HotbarCastPress;
using SpellHotbar::casts::McoCombo;
using SpellHotbar::casts::RollingMcoCombo;
using SpellHotbar::casts::capture_hotbar_press_to_prevent_dual_fire;
using SpellHotbar::casts::classify_cast_delivery;
using SpellHotbar::casts::classify_hotbar_cast_press;
using SpellHotbar::casts::cut_committed_cast_for_left_hand_press;
using SpellHotbar::casts::driver_cast_blocks_movement;
using SpellHotbar::casts::isolate_left_hand_caster_before_vanilla_spellfire;
using SpellHotbar::casts::isolate_left_hand_caster_for_driver_cast;
using SpellHotbar::casts::keep_commitment_until_cut;
using SpellHotbar::casts::MscoChargeCurve;
using SpellHotbar::casts::charge_time_to_anim_speed;
using SpellHotbar::casts::is_msco_combo_window_close_event;
using SpellHotbar::casts::is_msco_combo_window_open_event;

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
	expect(classify_hotbar_cast_press(false, false, false) == HotbarCastPress::start,
		"no live cast is an ordinary first press");
	expect(classify_hotbar_cast_press(false, true, true) == HotbarCastPress::start,
		"a stale holding flag cannot turn an idle press into a chain");
}

void committed_follow_up_press_chains()
{
	expect(classify_hotbar_cast_press(true, true, true) == HotbarCastPress::chain,
		"a second press inside the open window on a committed Driver Cast is a combo step");
}

void committed_press_after_winclose_is_refused()
{
	expect(classify_hotbar_cast_press(true, true, false) == HotbarCastPress::refuse,
		"after WinClose, a committed Driver Cast refuses a late follow-up until CastExit");
}

void live_cast_without_commitment_is_refused()
{
	expect(classify_hotbar_cast_press(true, false, false) == HotbarCastPress::refuse,
		"before spellfire, or a concentration channel, the second press is refused");
	expect(classify_hotbar_cast_press(true, false, true) == HotbarCastPress::refuse,
		"a window bit without commitment cannot chain");
}

void left_spellfire_opens_the_combo_window()
{
	expect(is_msco_combo_window_open_event("MLh_SpellFire_Event"),
		"the borrowed left-hand SpellFire opens the combo window");
	expect(!is_msco_combo_window_open_event("MRh_SpellFire_Event"),
		"right-hand SpellFire does not open the combo window");
	expect(!is_msco_combo_window_open_event("MSCO_WinOpen"),
		"WinOpen does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MCO_WinOpen"),
		"MCO WinOpen does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MSCO_winopen"),
		"lowercase MSCO WinOpen does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MCO_winopen"),
		"lowercase WinOpen does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MSCO_WinClose"),
		"WinClose does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MCO_WinClose"),
		"MCO WinClose does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MSCO_winclose"),
		"lowercase MSCO WinClose does not open the cast combo window");
	expect(!is_msco_combo_window_open_event("MCO_winclose"),
		"lowercase WinClose does not open the cast combo window");
}

void winclose_tags_close_the_combo_window()
{
	expect(is_msco_combo_window_close_event("MSCO_WinClose"), "MSCO_WinClose closes the window");
	expect(is_msco_combo_window_close_event("MCO_WinClose"), "MCO_WinClose closes the window");
	expect(is_msco_combo_window_close_event("MSCO_winclose"), "MSCO_winclose closes the window");
	expect(is_msco_combo_window_close_event("MCO_winclose"), "MCO_winclose closes the window");
	expect(!is_msco_combo_window_close_event("MSCO_WinOpen"), "MSCO_WinOpen does not close the window");
	expect(!is_msco_combo_window_close_event("MCO_WinOpen"), "MCO_WinOpen does not close the window");
	expect(!is_msco_combo_window_close_event("MSCO_winopen"), "MSCO_winopen does not close the window");
	expect(!is_msco_combo_window_close_event("MCO_winopen"), "MCO_winopen does not close the window");
}

void shipped_exponential_curve_is_1_at_base_time()
{
	const MscoChargeCurve shipped{};
	expect(charge_time_to_anim_speed(0.15f, shipped) == 1.0f,
		"base time 0.15s is speed 1.0 on the shipped exponential curve");
}

void charge_mechanic_off_is_always_one()
{
	MscoChargeCurve off{};
	off.mechanic_on = false;
	expect(charge_time_to_anim_speed(0.5f, off) == 1.0f, "mechanic off leaves clips at 1.0");
	expect(charge_time_to_anim_speed(2.0f, off) == 1.0f, "mechanic off ignores long charge");
}

void firebolt_charge_is_slower_than_base_on_the_shipped_curve()
{
	const MscoChargeCurve shipped{};
	const float speed = charge_time_to_anim_speed(0.5f, shipped);
	expect(speed > 0.81f && speed < 0.82f,
		"Firebolt 0.5s is ~0.815 on (0.15/0.5)^0.17");
}

void zero_charge_clamps_to_max_speed()
{
	const MscoChargeCurve shipped{};
	expect(charge_time_to_anim_speed(0.0f, shipped) == 1.25f,
		"zero charge would divide by zero; clamp to max speed 1.25");
}

void two_second_charge_is_near_min_speed()
{
	const MscoChargeCurve shipped{};
	const float speed = charge_time_to_anim_speed(2.0f, shipped);
	expect(speed > 0.63f && speed < 0.66f, "2s charge is ~0.644 on (0.15/2)^0.17");
}

void linear_mode_is_1_at_its_base_time()
{
	MscoChargeCurve linear{};
	linear.exp_mode = false;
	linear.base_time = 0.5f;
	expect(charge_time_to_anim_speed(0.5f, linear) == 1.0f,
		"linear mode is speed 1.0 at the authored base time");
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

void clip_4_does_not_deliver_during_its_windup()
{
	expect(classify_cast_delivery(false, true, false, true) == CastDelivery::wait,
		"clip 4's 0.5s floor must not release during the windup before SpellFire");
}

void clip_4_delivers_when_spellfire_arrives_past_the_floor()
{
	expect(classify_cast_delivery(false, true, true, true) == CastDelivery::deliver,
		"clip 4 delivers at the SpellFire pose once the authored floor has passed");
}

void clips_1_to_3_still_deliver_near_the_start()
{
	expect(classify_cast_delivery(false, false, true, true) == CastDelivery::deliver,
		"clips 1-3 deliver at SpellFire even when it lands before the authored time");
	expect(classify_cast_delivery(false, false, true, false) == CastDelivery::deliver,
		"a cut after SpellFire still delivers; the annotation already led");
	expect(classify_cast_delivery(false, true, true, true) == CastDelivery::deliver,
		"clips 1-3 still deliver when SpellFire and the authored time have both landed");
}

void missing_annotation_falls_back_when_the_clip_ends()
{
	expect(classify_cast_delivery(false, true, false, false) == CastDelivery::deliver,
		"a clip that never raises SpellFire still delivers once it has ended past the floor");
}

void losing_the_state_before_the_floor_cancels()
{
	expect(classify_cast_delivery(false, false, false, false) == CastDelivery::cancel,
		"losing the cast state before the floor and before SpellFire cancels");
}

void already_delivered_does_not_fire_again()
{
	expect(classify_cast_delivery(true, true, true, true) == CastDelivery::wait,
		"a payload that already left the hand is not delivered twice");
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

void driver_cast_roots_while_the_shtb_state_is_live()
{
	expect(driver_cast_blocks_movement(true, true),
		"a live Driver Cast state roots the player");
	expect(driver_cast_blocks_movement(true, false),
		"consecutive clips keep the state without a new instance; they stay rooted");
}

void ticket_10_cut_unroots_even_if_the_instance_is_still_alive()
{
	expect(!driver_cast_blocks_movement(false, true),
		"SH2_CastExit / a ticket-10 cut ends rooting at the state, not when the instance dies");
	expect(!driver_cast_blocks_movement(false, false),
		"idle is not rooted");
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
	committed_press_after_winclose_is_refused();
	live_cast_without_commitment_is_refused();
	left_spellfire_opens_the_combo_window();
	winclose_tags_close_the_combo_window();
	shipped_exponential_curve_is_1_at_base_time();
	charge_mechanic_off_is_always_one();
	firebolt_charge_is_slower_than_base_on_the_shipped_curve();
	zero_charge_clamps_to_max_speed();
	two_second_charge_is_near_min_speed();
	linear_mode_is_1_at_its_base_time();
	chain_press_keeps_commitment_until_the_cut();
	handled_hotbar_press_is_captured_when_the_left_hand_holds_a_spell();
	driver_cast_isolates_the_left_caster_when_that_hand_holds_a_spell();
	driver_cast_isolates_before_vanilla_sees_left_spellfire();
	clip_4_does_not_deliver_during_its_windup();
	clip_4_delivers_when_spellfire_arrives_past_the_floor();
	clips_1_to_3_still_deliver_near_the_start();
	missing_annotation_falls_back_when_the_clip_ends();
	losing_the_state_before_the_floor_cancels();
	already_delivered_does_not_fire_again();
	left_hand_press_cuts_a_committed_hotbar_cast();
	driver_cast_roots_while_the_shtb_state_is_live();
	ticket_10_cut_unroots_even_if_the_instance_is_still_alive();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
