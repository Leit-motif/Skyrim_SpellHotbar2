#include "cast_anim_ids.h"

#include <cstdlib>
#include <iterator>
#include <iostream>

using SpellHotbar::GameData::anim_id;
using SpellHotbar::GameData::CastAnimSlot;
using SpellHotbar::GameData::concentration_family;
using SpellHotbar::GameData::fire_and_forget_family;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

std::uint16_t conc(bool two_handed, bool self, bool ward, CastAnimSlot slot)
{
	return anim_id(concentration_family(two_handed, self, ward), slot);
}

std::uint16_t fnf(bool two_handed, bool self, CastAnimSlot slot)
{
	return anim_id(fire_and_forget_family(two_handed, self), slot);
}

void aimed_concentration_keeps_its_own_ids()
{
	expect(conc(false, false, false, CastAnimSlot::primary) == 1001, "aimed conc primary is 1001");
	expect(conc(false, false, false, CastAnimSlot::variant) == 11003, "aimed conc dual is 11003");
}

void self_concentration_keeps_its_own_ids()
{
	expect(conc(false, true, false, CastAnimSlot::primary) == 1002, "self conc primary is 1002");
	expect(conc(false, true, false, CastAnimSlot::variant) == 11004, "self conc dual is 11004");
}

void ritual_concentration_stays_in_its_family_in_both_slots()
{
	expect(conc(true, false, false, CastAnimSlot::primary) == 11001, "ritual conc primary is 11001");
	expect(conc(true, false, false, CastAnimSlot::variant) == 11001,
		"ritual conc variant no longer borrows the plain dual conc loop (11003)");
}

void ritual_self_concentration_uses_the_ritual_family()
{
	expect(conc(true, true, false, CastAnimSlot::primary) == 11001,
		"vanilla has no ritual self concentration; the ritual loop is the mapping");
}

void ward_concentration_maps_both_slots_to_the_single_hand_ward_loop()
{
	expect(conc(false, false, true, CastAnimSlot::primary) == 1003, "ward conc primary is 1003");
	expect(conc(false, false, true, CastAnimSlot::variant) == 1003,
		"no dual ward submod exists; the ward loop is the mapping, not another family's clip");
}

void ward_wins_over_delivery_for_a_one_handed_channel()
{
	expect(conc(false, true, true, CastAnimSlot::primary) == 1003, "a self-delivery ward is still a ward");
}

void two_handed_wins_over_ward()
{
	expect(conc(true, false, true, CastAnimSlot::primary) == 11001,
		"a two-handed channel is a ritual before it is a ward, as it always was");
}

void concentration_families_never_share_a_primary_id()
{
	const std::uint16_t ids[]{
		conc(false, false, false, CastAnimSlot::primary),
		conc(false, true, false, CastAnimSlot::primary),
		conc(false, false, true, CastAnimSlot::primary),
		conc(true, false, false, CastAnimSlot::primary),
	};
	for (size_t i = 0; i < std::size(ids); ++i) {
		for (size_t j = i + 1; j < std::size(ids); ++j) {
			expect(ids[i] != ids[j], "aimed, self, ward and ritual concentration stay distinct");
		}
	}
}

void fire_and_forget_ids_are_unchanged()
{
	expect(fnf(false, false, CastAnimSlot::primary) == 1, "aimed cast is 1");
	expect(fnf(false, false, CastAnimSlot::variant) == 10016, "aimed fast cast is 10016");
	expect(fnf(false, true, CastAnimSlot::primary) == 2, "self cast is 2");
	expect(fnf(false, true, CastAnimSlot::variant) == 10017, "self fast cast is 10017");
	expect(fnf(true, false, CastAnimSlot::primary) == 10000, "ritual cast is 10000");
	expect(fnf(true, false, CastAnimSlot::variant) == 10016, "ritual shares the aimed fast cast");
	expect(fnf(true, true, CastAnimSlot::primary) == 10000, "ritual self shares the ritual cast");
	expect(fnf(true, true, CastAnimSlot::variant) == 10017, "ritual self shares the self fast cast");
}

}  // namespace

int main()
{
	aimed_concentration_keeps_its_own_ids();
	self_concentration_keeps_its_own_ids();
	ritual_concentration_stays_in_its_family_in_both_slots();
	ritual_self_concentration_uses_the_ritual_family();
	ward_concentration_maps_both_slots_to_the_single_hand_ward_loop();
	ward_wins_over_delivery_for_a_one_handed_channel();
	two_handed_wins_over_ward();
	concentration_families_never_share_a_primary_id();
	fire_and_forget_ids_are_unchanged();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
