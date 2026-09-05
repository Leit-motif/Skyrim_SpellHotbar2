#include "action_definition.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::ActionDefinition;
using SpellHotbar::ActionInputDevice;
using SpellHotbar::ActionKind;
using SpellHotbar::ActionTargetKeys;
using SpellHotbar::ActionTargetSource;
using SpellHotbar::ActionPlayerOverlay;
using SpellHotbar::action_matches_catalogue;
using SpellHotbar::action_player_overlay_from;
using SpellHotbar::apply_action_player_overlay;
using SpellHotbar::custom_action_id_base;
using SpellHotbar::action_would_recurse;
using SpellHotbar::default_action_catalogue;
using SpellHotbar::resolve_action_input;
using SpellHotbar::resolve_action_scancode;

namespace {

int g_failures = 0;

void expect(bool condition, const char* message)
{
	if (!condition) {
		std::cerr << "FAIL: " << message << '\n';
		++g_failures;
	}
}

void shipped_catalogue_has_live_targets_and_custom_rows()
{
	const auto actions = default_action_catalogue();
	expect(actions.size() >= 3, "the default catalogue has combat and custom rows");
	expect(actions.size() >= 1 && actions[0].id == 1, "Power Attack keeps stable id 1");
	expect(actions.size() >= 1 && actions[0].target == ActionTargetSource::ocpa_power,
		"Power Attack resolves through OCPA");
	expect(actions.size() >= 2 && actions[1].id == 2, "Dodge keeps stable id 2");
	expect(actions.size() >= 2 && actions[1].target == ActionTargetSource::dodge_hotkey,
		"Dodge resolves through its live hotkey");
	expect(actions.size() >= 3 && actions[2].id == custom_action_id_base,
		"the first custom Action starts at the reserved id base");
	expect(actions.size() >= 3 && actions[2].target == ActionTargetSource::captured,
		"custom rows use captured scancodes");
	expect(actions.size() >= 3 && actions[2].captured_scancode == 0,
		"custom rows start unbound");
}

void target_resolution_uses_the_values_supplied_at_press()
{
	const ActionTargetKeys live{ .ocpa_power = 79, .dodge_hotkey = 81 };
	const auto actions = default_action_catalogue();
	const auto power = resolve_action_input(actions[0], live);
	expect(power.device == ActionInputDevice::keyboard && power.dx_scancode == 79,
		"Power Attack resolves as a keyboard dynamic target");
	const auto dodge = resolve_action_input(actions[1], live);
	expect(dodge.device == ActionInputDevice::keyboard && dodge.dx_scancode == 81,
		"Dodge resolves as a keyboard dynamic target");
	expect(resolve_action_scancode(actions[0], live) == 79,
		"Power Attack reads the current OCPA power scancode");
	expect(resolve_action_scancode(actions[1], live) == 81,
		"Dodge reads the current DodgeHotkey scancode");

	ActionDefinition custom = actions[2];
	custom.captured_scancode = 42;
	expect(resolve_action_scancode(custom, live) == 42,
		"a captured Action resolves its authored scancode");
}

void recursion_is_rejected_only_for_the_triggering_bind()
{
	expect(action_would_recurse(79, 79), "an Action cannot inject its own triggering bind");
	expect(!action_would_recurse(79, 80), "a different bind is not recursion");
	expect(!action_would_recurse(0, 0), "an unbound target is not reported as recursion");
	expect(!action_would_recurse(79, -1), "an unbound triggering slot cannot recurse");
}

void only_nonzero_meter_gcd_or_cooldown_makes_an_action_costed()
{
	ActionDefinition action{};
	expect(!action.is_costed(), "default Action is costless");
	action.stamina_cost = 1.0f;
	expect(action.is_costed(), "stamina makes an Action costed");
	action.stamina_cost = 0.0f;
	action.gcd = 0.1f;
	expect(action.is_costed(), "GCD makes an Action costed");
	action.gcd = 0.0f;
	action.cooldown_days = 0.1f;
	expect(action.is_costed(), "cooldown makes an Action costed");
}

void player_overlay_round_trips_the_action_fields()
{
	const auto actions = default_action_catalogue();
	ActionDefinition edited = actions.front();
	edited.display_name = "Heavy Power Attack";
	edited.icon = "DESTRUCTION_FIRE_ADEPT";
	edited.icon_form = 0x1234;
	edited.stamina_cost = 12.5f;
	edited.magicka_cost = 2.0f;
	edited.health_cost = 1.0f;
	edited.cooldown_days = 0.25f;
	edited.gcd = 1.25f;

	const ActionPlayerOverlay overlay = action_player_overlay_from(edited);
	ActionDefinition restored = actions.front();
	apply_action_player_overlay(restored, overlay);
	expect(restored.id == actions.front().id, "an overlay never changes the stable Action id");
	expect(restored.display_name == edited.display_name, "overlay restores the Action name");
	expect(restored.icon == edited.icon && restored.icon_form == edited.icon_form,
		"overlay restores the Action icon fields");
	expect(restored.stamina_cost == edited.stamina_cost && restored.magicka_cost == edited.magicka_cost &&
		restored.health_cost == edited.health_cost, "overlay restores all meter costs");
	expect(restored.cooldown_days == edited.cooldown_days && restored.gcd == edited.gcd,
		"overlay restores cooldown and GCD");
	expect(!action_matches_catalogue(restored, actions.front()), "an edited Action differs from its catalogue row");
}

void captured_device_and_target_round_trip_through_an_overlay()
{
	const auto actions = default_action_catalogue();
	ActionDefinition edited = actions[2];
	edited.target = ActionTargetSource::captured;
	edited.captured_device = ActionInputDevice::gamepad;
	edited.captured_scancode = 277; // a supported gamepad DX range value

	const ActionPlayerOverlay overlay = action_player_overlay_from(edited);
	ActionDefinition restored = actions[2];
	apply_action_player_overlay(restored, overlay);
	expect(restored.target == ActionTargetSource::captured,
		"overlay preserves a custom Action's captured target");
	expect(restored.captured_device == ActionInputDevice::gamepad,
		"overlay preserves the captured input device");
	expect(restored.captured_scancode == 277,
		"overlay preserves the captured DX scancode");
	const auto resolved = resolve_action_input(restored, {});
	expect(resolved.device == ActionInputDevice::gamepad && resolved.dx_scancode == 277,
		"captured target resolves both device and DX scancode");
}

}  // namespace

int main()
{
	shipped_catalogue_has_live_targets_and_custom_rows();
	target_resolution_uses_the_values_supplied_at_press();
	recursion_is_rejected_only_for_the_triggering_bind();
	only_nonzero_meter_gcd_or_cooldown_makes_an_action_costed();
	player_overlay_round_trips_the_action_fields();
	captured_device_and_target_round_trip_through_an_overlay();
	return g_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
