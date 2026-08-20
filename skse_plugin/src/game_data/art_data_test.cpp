#include "art_definition.h"

#include <cstdlib>
#include <iostream>
#include <string>

using SpellHotbar::ArtClass;
using SpellHotbar::ArtDefinition;
using SpellHotbar::GameData::EquippedType;
using SpellHotbar::art_class_is_live;
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
		"ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"1\tTest Art\tGREATER_POWER\t1\tGeneric\t25\t8s\t1.0\n";
	const auto arts = parse_art_tsv(tsv);
	expect(arts.size() == 1, "one data row loads");
	expect(arts.size() == 1 && arts[0].id == 1, "ArtID is 1");
	expect(arts.size() == 1 && arts[0].display_name == "Test Art", "DisplayName is Test Art");
	expect(arts.size() == 1 && arts[0].icon == "GREATER_POWER", "Icon is GREATER_POWER");
	expect(arts.size() == 1 && arts[0].selector == 1, "Selector is 1");
	expect(arts.size() == 1 && arts[0].art_class == ArtClass::Generic, "ArtClass Generic is Generic");
	expect(arts.size() == 1 && arts[0].stamina_cost == 25.0f, "StaminaCost is 25");
	expect(arts.size() == 1 && arts[0].gcd == 1.0f, "GlobalCooldown is 1.0");
	expect(arts.size() == 1 && arts[0].cooldown_days > 0.0f, "8s cooldown is positive days");
}

void art_class_column_loads_each_legal_value()
{
	const char* tsv =
		"ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"2\tBlood Flurry\tGREATER_POWER\t2\t2H\t25\t8s\t1.0\n"
		"3\tIai Slash\tGREATER_POWER\t3\t1H\t25\t8s\t1.0\n"
		"4\tDual Flurry\tGREATER_POWER\t4\tDual\t25\t8s\t1.0\n"
		"5\tDisengage\tGREATER_POWER\t5\tGeneric\t25\t8s\t1.0\n";
	const auto arts = parse_art_tsv(tsv);
	expect(arts.size() == 4, "four class rows load");
	expect(arts.size() >= 1 && arts[0].art_class == ArtClass::TwoHand, "2H is TwoHand");
	expect(arts.size() >= 2 && arts[1].art_class == ArtClass::OneHand, "1H is OneHand");
	expect(arts.size() >= 3 && arts[2].art_class == ArtClass::Dual, "Dual is Dual");
	expect(arts.size() >= 4 && arts[3].art_class == ArtClass::Generic, "Generic is Generic");
}

void empty_art_class_defaults_generic()
{
	const char* tsv =
		"ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"8\tCustom Art\tGREATER_POWER\t8\t\t25\t8s\t1.0\n";
	const auto arts = parse_art_tsv(tsv);
	expect(arts.size() == 1 && arts[0].art_class == ArtClass::Generic,
		"an empty ArtClass cell is Generic");
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
	const auto arts = parse_art_tsv("1\tTest Art\tGREATER_POWER\t1\tGeneric\t25\t8s\t1.0\n");
	expect(arts.empty(), "a file without the required header is not an art catalogue");
}

void header_without_art_class_loads_nothing()
{
	const auto arts = parse_art_tsv(
		"ArtID\tDisplayName\tIcon\tSelector\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"1\tTest Art\tGREATER_POWER\t1\t25\t8s\t1.0\n");
	expect(arts.empty(), "a catalogue without an ArtClass column is not loaded");
}

void zero_id_and_empty_name_are_skipped()
{
	const char* tsv =
		"ArtID\tDisplayName\tIcon\tSelector\tArtClass\tStaminaCost\tCooldown\tGlobalCooldown\n"
		"0\tGhost\tGREATER_POWER\t1\tGeneric\t0\t-1\t1.0\n"
		"2\t\tGREATER_POWER\t2\tGeneric\t0\t-1\t1.0\n"
		"3\tReal\tGREATER_POWER\t3\tGeneric\t0\t-1\t1.0\n";
	const auto arts = parse_art_tsv(tsv);
	expect(arts.size() == 1, "only the named non-zero row remains");
	expect(arts.size() == 1 && arts[0].id == 3, "surviving row is ArtID 3");
}

void two_hand_art_is_live_only_on_two_hand()
{
	expect(art_class_is_live(ArtClass::TwoHand, EquippedType::TWOHAND), "2H live on greatsword");
	expect(!art_class_is_live(ArtClass::TwoHand, EquippedType::ONEHAND_EMPTY), "2H dead on rapier");
	expect(!art_class_is_live(ArtClass::TwoHand, EquippedType::DUAL_WIELD), "2H dead on dual 1H");
	expect(!art_class_is_live(ArtClass::TwoHand, EquippedType::FIST), "2H dead on fists");
	expect(!art_class_is_live(ArtClass::TwoHand, EquippedType::BOW), "2H dead on bow");
	expect(!art_class_is_live(ArtClass::TwoHand, EquippedType::SPELL), "2H dead on magic");
	expect(!art_class_is_live(ArtClass::TwoHand, EquippedType::STAFF_SHIELD), "2H dead on staff");
}

void one_hand_art_is_live_on_one_hand_variants()
{
	expect(art_class_is_live(ArtClass::OneHand, EquippedType::ONEHAND_EMPTY), "1H live on rapier");
	expect(art_class_is_live(ArtClass::OneHand, EquippedType::ONEHAND_SHIELD), "1H live with shield");
	expect(art_class_is_live(ArtClass::OneHand, EquippedType::ONEHAND_SPELL), "1H live with spell");
	expect(!art_class_is_live(ArtClass::OneHand, EquippedType::TWOHAND), "1H dead on greatsword");
	expect(!art_class_is_live(ArtClass::OneHand, EquippedType::DUAL_WIELD), "1H dead on dual 1H");
	expect(!art_class_is_live(ArtClass::OneHand, EquippedType::BOW), "1H dead on bow");
}

void dual_art_is_live_only_on_dual_wield()
{
	expect(art_class_is_live(ArtClass::Dual, EquippedType::DUAL_WIELD), "Dual live on two melee 1H");
	expect(!art_class_is_live(ArtClass::Dual, EquippedType::ONEHAND_EMPTY), "Dual dead on rapier");
	expect(!art_class_is_live(ArtClass::Dual, EquippedType::TWOHAND), "Dual dead on greatsword");
}

void generic_art_is_live_on_melee_and_fists_only()
{
	expect(art_class_is_live(ArtClass::Generic, EquippedType::ONEHAND_EMPTY), "Generic live on rapier");
	expect(art_class_is_live(ArtClass::Generic, EquippedType::TWOHAND), "Generic live on greatsword");
	expect(art_class_is_live(ArtClass::Generic, EquippedType::DUAL_WIELD), "Generic live on dual 1H");
	expect(art_class_is_live(ArtClass::Generic, EquippedType::FIST), "Generic live on fists");
	expect(!art_class_is_live(ArtClass::Generic, EquippedType::BOW), "Generic dead on bow");
	expect(!art_class_is_live(ArtClass::Generic, EquippedType::CROSSBOW), "Generic dead on crossbow");
	expect(!art_class_is_live(ArtClass::Generic, EquippedType::SPELL), "Generic dead on magic");
	expect(!art_class_is_live(ArtClass::Generic, EquippedType::STAFF_SHIELD), "Generic dead on staff");
}

}  // namespace

int main()
{
	a_row_loads_every_art_field();
	art_class_column_loads_each_legal_value();
	empty_art_class_defaults_generic();
	eight_seconds_is_spell_data_days();
	non_positive_cooldown_means_none();
	missing_header_loads_nothing();
	header_without_art_class_loads_nothing();
	zero_id_and_empty_name_are_skipped();
	two_hand_art_is_live_only_on_two_hand();
	one_hand_art_is_live_on_one_hand_variants();
	dual_art_is_live_only_on_dual_wield();
	generic_art_is_live_on_melee_and_fists_only();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
