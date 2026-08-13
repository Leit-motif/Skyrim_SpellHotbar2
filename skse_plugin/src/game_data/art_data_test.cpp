#include "art_definition.h"

#include <cstdlib>
#include <iostream>
#include <string>

using SpellHotbar::ArtDefinition;
using SpellHotbar::parse_art_duration_days;
using SpellHotbar::parse_art_tsv;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

void a_row_loads_every_art_field()
{
	const char* tsv =
		"ArtID\tDisplayName\tIcon\tSelector\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"1\tTest Art\tGREATER_POWER\t1\t25\t8s\t1.0\n";
	const auto arts = parse_art_tsv(tsv);
	expect(arts.size() == 1, "one data row loads");
	expect(arts.size() == 1 && arts[0].id == 1, "ArtID is 1");
	expect(arts.size() == 1 && arts[0].display_name == "Test Art", "DisplayName is Test Art");
	expect(arts.size() == 1 && arts[0].icon == "GREATER_POWER", "Icon is GREATER_POWER");
	expect(arts.size() == 1 && arts[0].selector == 1, "Selector is 1");
	expect(arts.size() == 1 && arts[0].stamina_cost == 25.0f, "StaminaCost is 25");
	expect(arts.size() == 1 && arts[0].gcd == 1.0f, "GlobalCooldown is 1.0");
	expect(arts.size() == 1 && arts[0].cooldown_days > 0.0f, "8s cooldown is positive days");
}

void eight_seconds_is_spell_data_days()
{
	const auto days = parse_art_duration_days("8s");
	expect(days.has_value(), "8s parses");
	const float expected = 8.0f / (24.0f * 60.0f * 60.0f);
	expect(days.has_value() && *days > expected * 0.99f && *days < expected * 1.01f,
		"8s is 8 / 86400 days, same grammar as spell cooldowns");
}

void non_positive_cooldown_means_none()
{
	const auto zero = parse_art_duration_days("0");
	const auto neg = parse_art_duration_days("-1");
	expect(zero.has_value() && *zero < 0.0f, "0 cooldown is none");
	expect(neg.has_value() && *neg < 0.0f, "negative cooldown is none");
}

void missing_header_loads_nothing()
{
	const auto arts = parse_art_tsv("1\tTest Art\tGREATER_POWER\t1\t25\t8s\t1.0\n");
	expect(arts.empty(), "a file without the required header is not an art catalogue");
}

void zero_id_and_empty_name_are_skipped()
{
	const char* tsv =
		"ArtID\tDisplayName\tIcon\tSelector\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"0\tGhost\tGREATER_POWER\t1\t0\t-1\t1.0\n"
		"2\t\tGREATER_POWER\t2\t0\t-1\t1.0\n"
		"3\tReal\tGREATER_POWER\t3\t0\t-1\t1.0\n";
	const auto arts = parse_art_tsv(tsv);
	expect(arts.size() == 1, "only the named non-zero row remains");
	expect(arts.size() == 1 && arts[0].id == 3, "surviving row is ArtID 3");
}

}  // namespace

int main()
{
	a_row_loads_every_art_field();
	eight_seconds_is_spell_data_days();
	non_positive_cooldown_means_none();
	missing_header_loads_nothing();
	zero_id_and_empty_name_are_skipped();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
