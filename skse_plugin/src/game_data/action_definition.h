#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace SpellHotbar {

enum class ActionKind : std::uint8_t {
	physical_scancode = 0,
};

// The persisted device is deliberately separate from the DX scancode.  The latter is the
// device-independent value SH2 already uses for ordinary keybinds, while the device tells the
// injection seam which native ButtonEvent device should receive the decoded code.
enum class ActionInputDevice : std::uint8_t {
	keyboard = 0,
	mouse,
	gamepad,
};

enum class ActionTargetSource : std::uint8_t {
	ocpa_power = 0,
	dodge_hotkey,
	captured,
};

struct ActionTargetKeys {
	std::uint32_t ocpa_power{ 0 };
	std::uint32_t ocpa_dual{ 0 };
	std::uint32_t dodge_hotkey{ 0 };
};

struct ActionInput {
	ActionInputDevice device{ ActionInputDevice::keyboard };
	std::uint32_t dx_scancode{ 0 };

	[[nodiscard]] constexpr bool is_bound() const noexcept { return dx_scancode != 0; }
};

// Legacy Action overlays stored only the combined DX value.  Keep those overlays loadable while
// new saves record both pieces explicitly.
[[nodiscard]] constexpr ActionInputDevice action_input_device_from_dx_scancode(
	std::uint32_t dx_scancode) noexcept
{
	if (dx_scancode >= 266U) {
		return ActionInputDevice::gamepad;
	}
	if (dx_scancode >= 256U) {
		return ActionInputDevice::mouse;
	}
	return ActionInputDevice::keyboard;
}

struct ActionDefinition {
	std::uint32_t id{ 0 };
	std::string display_name;
	std::string icon;
	std::uint32_t icon_form{ 0 };
	ActionKind kind{ ActionKind::physical_scancode };
	ActionTargetSource target{ ActionTargetSource::captured };
	ActionInputDevice captured_device{ ActionInputDevice::keyboard };
	std::uint32_t captured_scancode{ 0 };
	float stamina_cost{ 0.0f };
	float magicka_cost{ 0.0f };
	float health_cost{ 0.0f };
	float cooldown_days{ 0.0f };
	float gcd{ 0.0f };

	[[nodiscard]] bool is_costed() const noexcept;
	/** The static form: does this Action name the OCPA power-attack target by source? A captured
	 *  key that happens to be OCPA's own hotkey is an attack too -- see
	 *  `action_input_is_attack`, which is what a live press should ask. */
	[[nodiscard]] bool is_attack() const noexcept;
};

struct ActionPlayerOverlay {
	std::string display_name;
	std::string icon;
	std::uint32_t icon_form{ 0 };
	ActionKind kind{ ActionKind::physical_scancode };
	ActionTargetSource target{ ActionTargetSource::captured };
	ActionInputDevice captured_device{ ActionInputDevice::keyboard };
	std::uint32_t captured_scancode{ 0 };
	float stamina_cost{ 0.0f };
	float magicka_cost{ 0.0f };
	float health_cost{ 0.0f };
	float cooldown_days{ 0.0f };
	float gcd{ 0.0f };
};

inline constexpr std::uint32_t power_attack_action_id = 1;
inline constexpr std::uint32_t dodge_action_id = 2;
inline constexpr std::uint32_t custom_action_id_base = 100;
inline constexpr std::uint32_t custom_action_count = 12;

[[nodiscard]] constexpr bool is_visible_action_id(std::uint32_t id) noexcept
{
	return id >= custom_action_id_base && id < custom_action_id_base + custom_action_count;
}

[[nodiscard]] std::vector<ActionDefinition> default_action_catalogue();

[[nodiscard]] ActionInput resolve_action_input(
	const ActionDefinition& action, const ActionTargetKeys& live_targets) noexcept;

/**
 * Is this press attack-shaped? The declared `ocpa_power` target always is. So is a captured
 * keyboard key that happens to be one of OCPA's own hotkeys: the target it drives is identical,
 * so the press should cut a committed cast the same way the declared target does. `live` carries
 * the keys read from OCPA's config at press time; a zero entry means unconfigured and matches
 * nothing.
 */
[[nodiscard]] bool action_input_is_attack(
	const ActionDefinition& action, const ActionInput& resolved,
	const ActionTargetKeys& live) noexcept;

[[nodiscard]] constexpr bool action_would_recurse(
	std::uint32_t target_scancode, int triggering_scancode) noexcept
{
	return target_scancode != 0 && triggering_scancode >= 0 &&
		target_scancode == static_cast<std::uint32_t>(triggering_scancode);
}

/**
 * Decide whether an Action press may enter dispatch before its ordinary cost, cooldown, and
 * resource checks. A live cast protects the graph until the committed cuttable span; the sole
 * exception is a costless Power Attack, which is allowed to cut that span. A retired
 * follow-through has no live casting instance and therefore does not reopen the whole-instance
 * gate; the existing attack cut seam still limits who can end it. Keeping this pure makes the
 * safety rule testable without constructing the native casting controller.
 */
[[nodiscard]] constexpr bool action_press_is_admitted(
	bool action_costed,
	bool action_attack,
	bool has_live_cast,
	bool committed_cuttable) noexcept
{
	if (!has_live_cast) {
		return true;
	}
	return !action_costed && action_attack && committed_cuttable;
}

void apply_action_player_overlay(ActionDefinition& action, const ActionPlayerOverlay& overlay);

[[nodiscard]] ActionPlayerOverlay action_player_overlay_from(const ActionDefinition& action);

[[nodiscard]] bool action_matches_catalogue(
	const ActionDefinition& live, const ActionDefinition& catalogue) noexcept;

[[nodiscard]] constexpr const char* action_target_label(ActionTargetSource target) noexcept
{
	switch (target) {
	case ActionTargetSource::ocpa_power:
		return "OCPA power attack";
	case ActionTargetSource::dodge_hotkey:
		return "Dodge hotkey";
	case ActionTargetSource::captured:
		return "Captured scancode";
	}
	return "Captured scancode";
}

}  // namespace SpellHotbar
