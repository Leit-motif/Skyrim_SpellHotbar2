#include "action_inject.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::Input::parse_dodge_hotkey;
using SpellHotbar::Input::parse_ocpa_keys;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

void ocpa_power_is_the_first_ikeycode()
{
	constexpr std::string_view ini =
		"[General]\n"
		"iKeycode = 79\n"
		"bOnlyDuringAttack = 0\n"
		"\n"
		"[DualAttack]\n"
		"iKeycode = 47\n";
	const auto keys = parse_ocpa_keys(ini);
	expect(keys.power == 79, "OCPA power is the first iKeycode");
	expect(keys.dual == 47, "OCPA dual is the second iKeycode");
}

void ocpa_unbound_ikeycode_is_zero()
{
	constexpr std::string_view ini =
		"[General]\n"
		"iKeycode = -1\n"
		"[DualAttack]\n"
		"iKeycode = 47\n";
	const auto keys = parse_ocpa_keys(ini);
	expect(keys.power == 0, "negative OCPA iKeycode is unbound");
	expect(keys.dual == 47, "dual still parses after an unbound power key");
}

void dodge_hotkey_skips_comments_and_lookalikes()
{
	constexpr std::string_view ini =
		"[Main]\n"
		";DodgeHotkey = 56\n"
		"DodgeHotkey = 81\n"
		";numpad3\n"
		"EnableSprintKeyDodge = false\n";
	expect(parse_dodge_hotkey(ini) == 81, "DodgeHotkey is 81, not the commented 56");
}

}  // namespace

int main()
{
	ocpa_power_is_the_first_ikeycode();
	ocpa_unbound_ikeycode_is_zero();
	dodge_hotkey_skips_comments_and_lookalikes();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
