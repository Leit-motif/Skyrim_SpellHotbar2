#include "clip_translation.h"

#include <cstdlib>
#include <iostream>
#include <numbers>
#include <vector>

using SpellHotbar::casts::ClipTranslation;
using SpellHotbar::casts::ClipTranslationKey;
using SpellHotbar::casts::interpolate_clip_translation;
using SpellHotbar::casts::local_to_world;
using SpellHotbar::casts::nearly_equal;
using SpellHotbar::casts::parse_animmotion_text;
using SpellHotbar::casts::translation_delta;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

void expect_xyz(ClipTranslation got, float x, float y, float z, const char* msg)
{
	expect(nearly_equal(got.x, x) && nearly_equal(got.y, y) && nearly_equal(got.z, z), msg);
}

void empty_keys_are_zero()
{
	expect_xyz(interpolate_clip_translation({}, 0.5f), 0.0f, 0.0f, 0.0f,
		"no keys means no translation");
}

void exact_key_returns_that_sample()
{
	const std::vector<ClipTranslationKey> keys{
		{ .time = 0.0167f, .x = 0.0f, .y = 4.375f, .z = 0.0f },
		{ .time = 1.0667f, .x = 0.0f, .y = -318.549f, .z = 0.0f },
	};
	expect_xyz(interpolate_clip_translation(keys, 1.0667f), 0.0f, -318.549f, 0.0f,
		"Disengage peak key is the cumulative back-leap");
}

void midway_is_linear()
{
	const std::vector<ClipTranslationKey> keys{
		{ .time = 0.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		{ .time = 1.0f, .x = 0.0f, .y = -100.0f, .z = 0.0f },
	};
	expect_xyz(interpolate_clip_translation(keys, 0.5f), 0.0f, -50.0f, 0.0f,
		"halfway between cumulative keys is the midpoint");
}

void past_the_last_key_clamps()
{
	const std::vector<ClipTranslationKey> keys{
		{ .time = 1.0667f, .x = 0.0f, .y = -318.549f, .z = 0.0f },
	};
	expect_xyz(interpolate_clip_translation(keys, 2.333f), 0.0f, -318.549f, 0.0f,
		"after the last key the body holds the last sample");
}

void before_the_first_key_lerps_from_origin()
{
	const std::vector<ClipTranslationKey> keys{
		{ .time = 0.2f, .x = 0.0f, .y = 10.0f, .z = 0.0f },
	};
	expect_xyz(interpolate_clip_translation(keys, 0.1f), 0.0f, 5.0f, 0.0f,
		"time before the first key interpolates from the origin");
}

void parse_reads_animmotion_and_ignores_other_text()
{
	const auto motion = parse_animmotion_text("animmotion 0.0 -318.549 0.0");
	expect(motion.has_value(), "animmotion text parses");
	expect(motion && nearly_equal(motion->y, -318.549f), "Y is the back-leap");
	expect(!parse_animmotion_text("HitFrame").has_value(), "HitFrame is not translation");
	expect(!parse_animmotion_text("animmotion").has_value(), "prefix alone is not a key");
}

void yaw_zero_keeps_local_y_as_world_y()
{
	const auto world = local_to_world(ClipTranslation{ .x = 0.0f, .y = -318.549f, .z = 0.0f }, 0.0f);
	expect_xyz(world, 0.0f, -318.549f, 0.0f, "facing north, clip -Y is world -Y");
}

void yaw_east_sends_forward_along_world_x()
{
	const float east = std::numbers::pi_v<float> / 2.0f;
	const auto world = local_to_world(ClipTranslation{ .x = 0.0f, .y = -100.0f, .z = 0.0f }, east);
	expect_xyz(world, -100.0f, 0.0f, 0.0f, "facing east, clip -Y (back) is world -X");
}

void frame_delta_is_the_step_between_samples()
{
	const auto from = ClipTranslation{ .x = 0.0f, .y = 4.375f, .z = 0.0f };
	const auto to = ClipTranslation{ .x = 0.0f, .y = 15.183f, .z = 0.0f };
	const auto d = translation_delta(from, to);
	expect_xyz(d, 0.0f, 10.808f, 0.0f, "one annotation step is the frame delta to apply");
}

}  // namespace

int main()
{
	empty_keys_are_zero();
	exact_key_returns_that_sample();
	midway_is_linear();
	past_the_last_key_clamps();
	before_the_first_key_lerps_from_origin();
	parse_reads_animmotion_and_ignores_other_text();
	yaw_zero_keeps_local_y_as_world_y();
	yaw_east_sends_forward_along_world_x();
	frame_delta_is_the_step_between_samples();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
