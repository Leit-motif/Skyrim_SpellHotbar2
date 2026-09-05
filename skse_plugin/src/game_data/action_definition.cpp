#include "action_definition.h"

#include <algorithm>

namespace SpellHotbar {

namespace {

constexpr float positive_or_zero(float value) noexcept
{
	return value > 0.0f ? value : 0.0f;
}

ActionDefinition make_custom_action(std::uint32_t id)
{
	ActionDefinition action;
	action.id = id;
	action.display_name = "Action " + std::to_string(id - custom_action_id_base + 1);
	action.icon = "GREATER_POWER";
	action.target = ActionTargetSource::captured;
	action.captured_device = ActionInputDevice::keyboard;
	return action;
}

}  // namespace

bool ActionDefinition::is_costed() const noexcept
{
	return positive_or_zero(stamina_cost) > 0.0f ||
		positive_or_zero(magicka_cost) > 0.0f ||
		positive_or_zero(health_cost) > 0.0f ||
		positive_or_zero(cooldown_days) > 0.0f ||
		positive_or_zero(gcd) > 0.0f;
}

bool ActionDefinition::is_attack() const noexcept
{
	return target == ActionTargetSource::ocpa_power;
}

std::vector<ActionDefinition> default_action_catalogue()
{
	std::vector<ActionDefinition> actions;
	actions.reserve(2 + custom_action_count);

	ActionDefinition power_attack;
	power_attack.id = power_attack_action_id;
	power_attack.display_name = "Power Attack";
	power_attack.icon = "GREATER_POWER";
	power_attack.target = ActionTargetSource::ocpa_power;
	actions.push_back(power_attack);

	ActionDefinition dodge;
	dodge.id = dodge_action_id;
	dodge.display_name = "Dodge";
	dodge.icon = "GREATER_POWER";
	dodge.target = ActionTargetSource::dodge_hotkey;
	actions.push_back(dodge);

	// Keep a small, stable set of editable rows available before the Actions editor exists. The
	// rows are intentionally unbound: an editor or future data source must opt a target in rather
	// than accidentally making a newly-created Action inject an unrelated key.
	for (std::uint32_t id = custom_action_id_base; id < custom_action_id_base + custom_action_count; ++id) {
		actions.push_back(make_custom_action(id));
	}
	return actions;
}

ActionInput resolve_action_input(
	const ActionDefinition& action, const ActionTargetKeys& live_targets) noexcept
{
	if (action.kind != ActionKind::physical_scancode) {
		return {};
	}
	switch (action.target) {
	case ActionTargetSource::ocpa_power:
		// OCPA and TK Dodge are external keyboard hotkeys in this product slice. Their values are
		// resolved on every press, so changing either mod's setting does not require rewriting the
		// Action overlay.
		return ActionInput{ ActionInputDevice::keyboard, live_targets.ocpa_power };
	case ActionTargetSource::dodge_hotkey:
		return ActionInput{ ActionInputDevice::keyboard, live_targets.dodge_hotkey };
	case ActionTargetSource::captured:
		return ActionInput{ action.captured_device, action.captured_scancode };
	}
	return {};
}

std::uint32_t resolve_action_scancode(
	const ActionDefinition& action, const ActionTargetKeys& live_targets) noexcept
{
	return resolve_action_input(action, live_targets).dx_scancode;
}

void apply_action_player_overlay(ActionDefinition& action, const ActionPlayerOverlay& overlay)
{
	if (!overlay.display_name.empty()) {
		action.display_name = overlay.display_name;
	}
	action.icon = overlay.icon;
	action.icon_form = overlay.icon_form;
	action.kind = overlay.kind;
	action.target = overlay.target;
	action.captured_device = overlay.captured_device;
	action.captured_scancode = overlay.captured_scancode;
	action.stamina_cost = positive_or_zero(overlay.stamina_cost);
	action.magicka_cost = positive_or_zero(overlay.magicka_cost);
	action.health_cost = positive_or_zero(overlay.health_cost);
	action.cooldown_days = positive_or_zero(overlay.cooldown_days);
	action.gcd = positive_or_zero(overlay.gcd);
}

ActionPlayerOverlay action_player_overlay_from(const ActionDefinition& action)
{
	return ActionPlayerOverlay{
		.display_name = action.display_name,
		.icon = action.icon,
		.icon_form = action.icon_form,
		.kind = action.kind,
		.target = action.target,
		.captured_device = action.captured_device,
		.captured_scancode = action.captured_scancode,
		.stamina_cost = positive_or_zero(action.stamina_cost),
		.magicka_cost = positive_or_zero(action.magicka_cost),
		.health_cost = positive_or_zero(action.health_cost),
		.cooldown_days = positive_or_zero(action.cooldown_days),
		.gcd = positive_or_zero(action.gcd),
	};
}

bool action_matches_catalogue(
	const ActionDefinition& live, const ActionDefinition& catalogue) noexcept
{
	return live.display_name == catalogue.display_name &&
		live.icon == catalogue.icon &&
		live.icon_form == catalogue.icon_form &&
		live.kind == catalogue.kind &&
		live.target == catalogue.target &&
		live.captured_device == catalogue.captured_device &&
		live.captured_scancode == catalogue.captured_scancode &&
		positive_or_zero(live.stamina_cost) == positive_or_zero(catalogue.stamina_cost) &&
		positive_or_zero(live.magicka_cost) == positive_or_zero(catalogue.magicka_cost) &&
		positive_or_zero(live.health_cost) == positive_or_zero(catalogue.health_cost) &&
		positive_or_zero(live.cooldown_days) == positive_or_zero(catalogue.cooldown_days) &&
		positive_or_zero(live.gcd) == positive_or_zero(catalogue.gcd);
}

}  // namespace SpellHotbar
