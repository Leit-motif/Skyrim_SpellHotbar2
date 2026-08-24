#include "combo_cache.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::casts::CastComboIndex;
using SpellHotbar::casts::CastShape;
using SpellHotbar::casts::CastDelivery;
using SpellHotbar::casts::HotbarCastPress;
using SpellHotbar::casts::McoCombo;
using SpellHotbar::casts::RollingMcoCombo;
using SpellHotbar::casts::capture_hotbar_press_to_prevent_dual_fire;
using SpellHotbar::casts::classify_cast_delivery;
using SpellHotbar::casts::classify_hotbar_cast_press;
using SpellHotbar::casts::cut_committed_cast_for_left_hand_press;
using SpellHotbar::casts::driver_cast_blocks_movement;
using SpellHotbar::casts::shtb_state_blocks_movement;
using SpellHotbar::casts::isolate_left_hand_caster_before_vanilla_spellfire;
using SpellHotbar::casts::isolate_left_hand_caster_for_driver_cast;
using SpellHotbar::casts::keep_commitment_until_cut;
using SpellHotbar::casts::MscoChargeCurve;
using SpellHotbar::casts::charge_time_to_anim_speed;
using SpellHotbar::casts::is_msco_combo_window_close_event;
using SpellHotbar::casts::is_msco_combo_window_open_event;
using SpellHotbar::casts::is_mco_combo_index_edge;
using SpellHotbar::casts::window_close_is_a_stomp_to_undo;
using SpellHotbar::casts::AbilityLatch;
using SpellHotbar::casts::classify_ability_latch;
using SpellHotbar::casts::is_ability_hit_frame_event;
using SpellHotbar::casts::is_ability_latch_event;
using SpellHotbar::casts::is_ability_win_open_event;
using SpellHotbar::casts::should_capture_attack_during_ability;
using SpellHotbar::casts::should_cut_ability_for_attack;
using SpellHotbar::casts::should_record_mco_combo_sample;
using SpellHotbar::casts::should_retain_local_cast_intent;
using SpellHotbar::casts::should_yield_shtb_before_hotbar_shout;
using SpellHotbar::casts::channel_chain_window_open;
using SpellHotbar::casts::channel_end_arms_combo_restore;
using SpellHotbar::casts::combo_sample_survives_hold;
using SpellHotbar::casts::should_cut_channel_for_attack;
using SpellHotbar::casts::cast_entry_walks_clip_set;
using SpellHotbar::casts::exit_without_spellfire_is_a_dropped_press;
using SpellHotbar::casts::spellfire_advances_cast_index;
using SpellHotbar::casts::spellfire_opens_combo_window;

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

void live_art_state_plants_wasd()
{
	expect(shtb_state_blocks_movement(true),
		"a live SH2_Art_State plants WASD the same way a Driver Cast does");
	expect(!shtb_state_blocks_movement(false),
		"idle does not plant");
}

void ability_latch_prefers_winopen_then_hitframe_then_artexit()
{
	expect(classify_ability_latch(true, true) == AbilityLatch::winOpen,
		"a clip with WinOpen latches there even if it also has HitFrame");
	expect(classify_ability_latch(true, false) == AbilityLatch::winOpen,
		"WinOpen is the Ability latch when present");
	expect(classify_ability_latch(false, true) == AbilityLatch::hitFrame,
		"HitFrame is the Ability latch when the clip has no WinOpen");
	expect(classify_ability_latch(false, false) == AbilityLatch::artExit,
		"SH2_ArtExit is the Ability latch when the clip has neither window");
}

void ability_latch_events_match_the_classified_kind()
{
	expect(is_ability_win_open_event("MCO_WinOpen"), "MCO_WinOpen opens the Ability latch");
	expect(is_ability_win_open_event("MSCO_WinOpen"), "MSCO_WinOpen opens the Ability latch");
	expect(is_ability_win_open_event("MCO_winopen"), "lowercase MCO WinOpen opens the Ability latch");
	expect(!is_ability_win_open_event("MCO_WinClose"), "WinClose is not the Ability latch");
	expect(is_ability_hit_frame_event("HitFrame"), "HitFrame is the fallback Ability latch event");
	expect(!is_ability_hit_frame_event("MCO_WinOpen"), "WinOpen is not HitFrame");
	expect(is_ability_latch_event(AbilityLatch::winOpen, "MCO_WinOpen"),
		"a WinOpen latch fires on WinOpen");
	expect(!is_ability_latch_event(AbilityLatch::winOpen, "HitFrame"),
		"a WinOpen latch does not fire early at HitFrame");
	expect(is_ability_latch_event(AbilityLatch::hitFrame, "HitFrame"),
		"a HitFrame latch fires on HitFrame");
	expect(is_ability_latch_event(AbilityLatch::artExit, "SH2_ArtExit"),
		"an ArtExit latch fires on SH2_ArtExit");
	expect(!is_ability_latch_event(AbilityLatch::artExit, "HitFrame"),
		"an ArtExit latch does not fire at HitFrame");
}

void a_press_behind_our_shtb_is_retained_until_the_latch_opens()
{
	expect(should_retain_local_cast_intent(true, false, false),
		"a hotbar press during our Driver Cast or Ability before the latch is the local queue");
	expect(!should_retain_local_cast_intent(true, true, false),
		"once the latch is open the press starts now rather than waiting");
	expect(!should_retain_local_cast_intent(false, false, false),
		"someone else's swing is ShoutMCO's clock, not a local retain");
	expect(!should_retain_local_cast_intent(true, false, true),
		"concentration is not this queue");
}

void attack_after_the_ability_latch_cuts_and_is_not_captured()
{
	expect(should_cut_ability_for_attack(true, true, true),
		"an attack after the Ability latch sends ArtExit and stays uncaptured");
	expect(!should_cut_ability_for_attack(true, false, true),
		"an attack before the Ability latch must not skip the current hit");
	expect(!should_cut_ability_for_attack(false, true, true),
		"idle is not an Ability cut");
	expect(should_capture_attack_during_ability(true, false, true),
		"an attack before the Ability latch is captured so it cannot mash through");
	expect(!should_capture_attack_during_ability(true, true, true),
		"after the latch the attack press is left uncaptured");
	expect(!should_capture_attack_during_ability(true, false, false),
		"an unrelated key is not captured as an Ability mash-through");
}

void ability_hitframe_does_not_replace_the_mco_combo_sample()
{
	expect(!should_record_mco_combo_sample(true),
		"HitFrame on our Ability or Driver Cast must not stomp the sampled combo");
	expect(should_record_mco_combo_sample(false),
		"someone else's MCO swing still samples combo position");
}

void a_legal_hotbar_shout_yields_the_live_shtb_clip()
{
	expect(should_yield_shtb_before_hotbar_shout(true, true),
		"once the latch is open a shout must stop the Ability or Driver Cast first");
	expect(!should_yield_shtb_before_hotbar_shout(true, false),
		"before the latch the shout is queued, not overlapped");
	expect(!should_yield_shtb_before_hotbar_shout(false, true),
		"idle has no shtb clip to stop");
}


void a_channel_does_not_walk_the_cast_clip_set()
{
	expect(spellfire_advances_cast_index(CastShape::fire_and_forget),
		"a fire-and-forget clip advances SH2's cast index at SpellFire");
	expect(!spellfire_advances_cast_index(CastShape::channel),
		"a channel is one cast held, not a chain step");
}

void a_channel_enters_by_its_own_notify()
{
	expect(cast_entry_walks_clip_set(CastShape::fire_and_forget),
		"a fire-and-forget press picks its entry from the MSCO_left1..left4 cast index");
	expect(!cast_entry_walks_clip_set(CastShape::channel),
		"a channel has one entry of its own and never walks the clip set");
}

void a_channel_exit_without_spellfire_is_not_a_dropped_press()
{
	expect(exit_without_spellfire_is_a_dropped_press(CastShape::fire_and_forget),
		"a throw clip cut before SpellFire produced no payload; the cast index resets");
	expect(!exit_without_spellfire_is_a_dropped_press(CastShape::channel),
		"a channel commits on the authored cast-time floor, so its exit must not reset the index");
}

void a_channel_does_not_open_the_follow_up_press_window()
{
	expect(spellfire_opens_combo_window(CastShape::fire_and_forget),
		"ticket 22: a committed Driver Cast opens its window at SpellFire");
	expect(!spellfire_opens_combo_window(CastShape::channel),
		"a second hotbar cast during a hold is a refusal, not a chain");
}

void the_combo_index_is_sampled_where_mco_writes_it()
{
	// Measured live: MCO_WinClose carries the NEXT swing's index; MCO_AttackInitiate and
	// HitFrame carry the swing already playing, which is why sampling there repeated an attack.
	expect(is_mco_combo_index_edge("MCO_WinClose"), "MCO advances the index at window close");
	expect(is_mco_combo_index_edge("MSCO_WinClose"), "the MSCO spelling counts too");
	expect(!is_mco_combo_index_edge("attackStop"),
		"attackStop is MCO's reset to 1 -- the stomp the rolling cache exists to outlive");
	expect(!is_mco_combo_index_edge("MCO_AttackInitiate"),
		"attack time reads the swing already playing, one step behind");
	expect(!is_mco_combo_index_edge("HitFrame"), "so does HitFrame");
	expect(!is_mco_combo_index_edge("MCO_WinOpen"), "window open is still pre-advance");
}

void a_pending_restore_survives_the_ready_reset()
{
	expect(window_close_is_a_stomp_to_undo(true),
		"with a restore pending, a window-close is the ready reset and must be put back");
	expect(!window_close_is_a_stomp_to_undo(false),
		"with nothing pending, a window-close is MCO's genuine next index and is sampled");

	// The live failure: restore 3, the reset writes 1, and sampling it dropped the pending
	// restore so the next swing started the chain over.
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 3, .nextPowerAttack = 3 }, 0.0);
	cache.arm(0.0);
	expect(cache.restore_pending(), "armed restore is pending");
	const auto put_back = cache.peek();
	expect(put_back && put_back->nextAttack == 3, "the reset is undone with our own index");
	expect(cache.restore_pending(), "putting it back does not consume it");
	const auto consumed = cache.consume();
	expect(consumed && consumed->nextAttack == 3, "the next real swing takes 3");
}

void a_hold_does_not_age_out_the_combo_position()
{
	// The failure this fixes: attack1, hold Flames five seconds, swing -- and the swing came
	// out as attack1 because the sample had aged past kMaxAgeMs during the hold.
	RollingMcoCombo cache;
	cache.record(McoCombo{ .nextAttack = 2, .nextPowerAttack = 2 }, 0.0);
	const double release = RollingMcoCombo::kMaxAgeMs + 3000.0;
	expect(!cache.usable(release).has_value(),
		"without crediting the hold, an 8s-old sample is stale and the chain resets");

	cache.credit_held_time(5000.0);
	const auto got = cache.usable(release);
	expect(got && got->nextAttack == 2,
		"crediting the 5s hold keeps the position, so the swing after it continues the chain");
}

void a_gap_before_the_hold_still_ages_out()
{
	// The cap still does its real job: a combo sampled in an earlier fight must not be
	// replayed into an unrelated swing just because a channel was held afterwards.
	expect(combo_sample_survives_hold(30000.0, 29000.0),
		"a long hold started right after the swing hands its position on");
	expect(!combo_sample_survives_hold(30000.0, 1000.0),
		"a swing 29s before the hold is a different fight and must not be restored");
	expect(combo_sample_survives_hold(RollingMcoCombo::kMaxAgeMs, 0.0),
		"a fire-and-forget cast inside the cap is unaffected");
}

void a_channel_is_chainable_out_only_once_it_streams()
{
	expect(!channel_chain_window_open(false, false), "no channel, no window");
	expect(!channel_chain_window_open(true, false),
		"a channel still in its charge has not committed anything to chain out of");
	expect(channel_chain_window_open(true, true), "a streaming channel may hand off");
}

void an_attack_during_a_streaming_channel_ends_it()
{
	expect(should_cut_channel_for_attack(channel_chain_window_open(true, true), true),
		"attack during a streaming channel ends the channel");
	expect(!should_cut_channel_for_attack(channel_chain_window_open(true, true), false),
		"a press that is not an attack leaves the channel alone");
	expect(!should_cut_channel_for_attack(channel_chain_window_open(true, false), true),
		"an attack before the channel streams keeps today's behaviour");
}

void a_channel_that_streamed_hands_its_combo_position_on()
{
	expect(channel_end_arms_combo_restore(true),
		"the swing after a hold continues the chain the hold interrupted");
	expect(!channel_end_arms_combo_restore(false),
		"a channel that never streamed has nothing to hand off");
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
	live_art_state_plants_wasd();
	ability_latch_prefers_winopen_then_hitframe_then_artexit();
	ability_latch_events_match_the_classified_kind();
	a_press_behind_our_shtb_is_retained_until_the_latch_opens();
	attack_after_the_ability_latch_cuts_and_is_not_captured();
	ability_hitframe_does_not_replace_the_mco_combo_sample();
	a_legal_hotbar_shout_yields_the_live_shtb_clip();
	a_channel_does_not_walk_the_cast_clip_set();
	a_channel_enters_by_its_own_notify();
	a_channel_exit_without_spellfire_is_not_a_dropped_press();
	a_channel_does_not_open_the_follow_up_press_window();
	the_combo_index_is_sampled_where_mco_writes_it();
	a_pending_restore_survives_the_ready_reset();
	a_hold_does_not_age_out_the_combo_position();
	a_gap_before_the_hold_still_ages_out();
	a_channel_is_chainable_out_only_once_it_streams();
	an_attack_during_a_streaming_channel_ends_it();
	a_channel_that_streamed_hands_its_combo_position_on();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
