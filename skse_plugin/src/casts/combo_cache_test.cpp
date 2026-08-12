#include "combo_cache.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::casts::CastComboIndex;
using SpellHotbar::casts::McoCombo;
using SpellHotbar::casts::RollingMcoCombo;

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

}  // namespace

int main()
{
	rolling_empty_is_not_usable();
	rolling_returns_the_recorded_value_not_plus_one();
	rolling_is_usable_at_the_age_cap();
	rolling_later_record_replaces_the_earlier();
	cast_index_starts_at_one_and_wraps_after_four();
	cast_index_is_unchanged_by_an_attack_gap();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
