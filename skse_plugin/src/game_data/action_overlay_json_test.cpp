#include "action_overlay_json.h"

#include <cstdlib>
#include <iostream>

#include <rapidjson/document.h>

using SpellHotbar::ActionInputDevice;
using SpellHotbar::ActionKind;
using SpellHotbar::ActionPlayerOverlay;
using SpellHotbar::ActionTargetSource;
using SpellHotbar::parse_action_overlay_entry;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

// Parse one JSON object literal and hand it to the entry parser.
SpellHotbar::ActionOverlayParse parse(const char* json, const ActionPlayerOverlay& base = {})
{
	rapidjson::Document document;
	document.Parse(json);
	if (document.HasParseError() || !document.IsObject()) {
		std::cerr << "FAIL: test fixture is not valid JSON: " << json << '\n';
		++g_failures;
		return {};
	}
	return parse_action_overlay_entry(document, base);
}

void an_out_of_range_kind_target_or_device_is_rejected()
{
	const auto kind = parse(R"({"action_id": 100, "kind": 1})");
	expect(!kind.overlay && kind.error_member == "kind" && !kind.error_is_type,
		"kind 1 is outside the one shipped ActionKind");

	const auto target = parse(R"({"action_id": 100, "target": 3})");
	expect(!target.overlay && target.error_member == "target" && !target.error_is_type,
		"target 3 is outside the three shipped target sources");

	const auto device = parse(R"({"action_id": 100, "captured_device": 3})");
	expect(!device.overlay && device.error_member == "captured_device" && !device.error_is_type,
		"captured_device 3 is outside keyboard/mouse/gamepad");
}

void a_present_member_of_the_wrong_type_is_rejected_not_ignored()
{
	const auto scancode = parse(R"({"action_id": 100, "captured_scancode": 48.0})");
	expect(!scancode.overlay, "a float captured_scancode does not silently load as unbound");
	expect(scancode.error_member == "captured_scancode" && scancode.error_is_type,
		"the rejection names captured_scancode as a type error");
	expect(scancode.action_id == 100, "the rejection still reports which Action it was");

	const auto cost = parse(R"({"action_id": 100, "stamina_cost": "12"})");
	expect(!cost.overlay && cost.error_member == "stamina_cost" && cost.error_is_type,
		"a string cost is a type error, not a zero cost");

	const auto id = parse(R"({"action_id": 0})");
	expect(!id.overlay && id.error_member == "action_id", "action id 0 is not an Action");
}

void an_absent_member_keeps_the_base_value()
{
	ActionPlayerOverlay base;
	base.display_name = "Block";
	base.icon = "GREATER_POWER";
	base.stamina_cost = 5.0f;

	const auto parsed = parse(R"({"action_id": 100, "gcd": 0.5})", base);
	expect(parsed.overlay.has_value(), "an entry naming only gcd is accepted");
	if (parsed.overlay) {
		expect(parsed.overlay->display_name == "Block", "an absent name keeps the base name");
		expect(parsed.overlay->stamina_cost == 5.0f, "an absent cost keeps the base cost");
		expect(parsed.overlay->gcd == 0.5f, "the named member is read");
	}
}

void a_legacy_entry_without_a_device_infers_it_from_the_scancode()
{
	const auto parsed = parse(R"({"action_id": 100, "captured_scancode": 261})");
	expect(parsed.overlay.has_value(), "a version-1 entry without captured_device still loads");
	if (parsed.overlay) {
		expect(parsed.overlay->captured_device == ActionInputDevice::mouse,
			"DX 261 is a mouse button, so the inferred device is the mouse");
		expect(parsed.overlay->captured_scancode == 261, "the DX value survives the inference");
	}

	const auto explicit_device =
		parse(R"({"action_id": 100, "captured_device": 0, "captured_scancode": 261})");
	expect(explicit_device.overlay.has_value(), "an explicit device is accepted");
	if (explicit_device.overlay) {
		expect(explicit_device.overlay->captured_device == ActionInputDevice::keyboard,
			"an explicit device is never overwritten by the legacy inference");
	}
}

void an_accepted_entry_carries_every_edited_field()
{
	const auto parsed = parse(R"({
		"action_id": 101,
		"name": "Timed Block",
		"icon": "DESTRUCTION_FIRE_ADEPT",
		"icon_form": 4660,
		"kind": 0,
		"target": 2,
		"captured_device": 0,
		"captured_scancode": 47,
		"stamina_cost": 12.5,
		"magicka_cost": 2,
		"health_cost": 1,
		"cooldown_days": 0.25,
		"gcd": 1.25
	})");
	expect(parsed.overlay.has_value() && parsed.action_id == 101, "a full entry is accepted");
	if (parsed.overlay) {
		const auto& overlay = *parsed.overlay;
		expect(overlay.display_name == "Timed Block", "the name is read");
		expect(overlay.icon == "DESTRUCTION_FIRE_ADEPT" && overlay.icon_form == 0x1234,
			"both icon fields are read");
		expect(overlay.kind == ActionKind::physical_scancode, "the kind is read");
		expect(overlay.target == ActionTargetSource::captured, "the target is read");
		expect(overlay.captured_device == ActionInputDevice::keyboard &&
				overlay.captured_scancode == 47,
			"the captured input is read");
		expect(overlay.stamina_cost == 12.5f && overlay.magicka_cost == 2.0f &&
				overlay.health_cost == 1.0f,
			"the meter costs are read");
		expect(overlay.cooldown_days == 0.25f && overlay.gcd == 1.25f,
			"cooldown and GCD are read");
	}
}

}  // namespace

int main()
{
	an_out_of_range_kind_target_or_device_is_rejected();
	a_present_member_of_the_wrong_type_is_rejected_not_ignored();
	an_absent_member_keeps_the_base_value();
	a_legacy_entry_without_a_device_infers_it_from_the_scancode();
	an_accepted_entry_carries_every_edited_field();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
