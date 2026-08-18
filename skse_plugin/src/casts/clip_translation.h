#pragma once

#include <charconv>
#include <cmath>
#include <optional>
#include <span>
#include <string_view>

namespace SpellHotbar::casts {

// One animmotion key: cumulative actor-local translation at a clip time.
// Havok Y is character-forward; X is right; Z is up. Units match Skyrim world
// units (the Disengage dump peaks near -318 on Y, ~3 m back).
struct ClipTranslationKey {
	float time = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct ClipTranslation {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

[[nodiscard]] inline bool nearly_equal(float a, float b, float eps = 0.001f) noexcept
{
	return std::fabs(a - b) <= eps;
}

// Parse "animmotion x y z". Anything else is ignored (HitFrame, PIE, …).
[[nodiscard]] inline std::optional<ClipTranslation> parse_animmotion_text(
	std::string_view text) noexcept
{
	constexpr std::string_view kPrefix = "animmotion ";
	if (!text.starts_with(kPrefix)) {
		return std::nullopt;
	}
	text.remove_prefix(kPrefix.size());

	ClipTranslation out{};
	float* slots[3] = { &out.x, &out.y, &out.z };
	for (float* slot : slots) {
		while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
			text.remove_prefix(1);
		}
		if (text.empty()) {
			return std::nullopt;
		}
		float value = 0.0f;
		const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
		if (parsed.ec != std::errc{}) {
			return std::nullopt;
		}
		*slot = value;
		text.remove_prefix(static_cast<std::size_t>(parsed.ptr - text.data()));
	}
	return out;
}

// AMR's segment interpolation: keys are cumulative, t is clamped to the last key.
[[nodiscard]] inline ClipTranslation interpolate_clip_translation(
	std::span<const ClipTranslationKey> keys, float time) noexcept
{
	if (keys.empty()) {
		return {};
	}
	const float end = keys.back().time;
	const float t = time > end ? end : time;

	for (std::size_t i = 0; i < keys.size(); ++i) {
		if (t > keys[i].time) {
			continue;
		}
		const ClipTranslationKey& cur = keys[i];
		const float prevTime = i == 0 ? 0.0f : keys[i - 1].time;
		const ClipTranslation prev = i == 0
			? ClipTranslation{}
			: ClipTranslation{ keys[i - 1].x, keys[i - 1].y, keys[i - 1].z };
		const float duration = cur.time - prevTime;
		float progress = 1.0f;
		if (duration > 1.0e-6f) {
			progress = (t - prevTime) / duration;
		}
		const float inv = 1.0f - progress;
		return ClipTranslation{
			cur.x * progress + prev.x * inv,
			cur.y * progress + prev.y * inv,
			cur.z * progress + prev.z * inv,
		};
	}
	const auto& last = keys.back();
	return ClipTranslation{ last.x, last.y, last.z };
}

// Rotate a character-local XY delta into world XY. yaw=0 faces world +Y.
[[nodiscard]] inline ClipTranslation local_to_world(
	ClipTranslation local, float yawRadians) noexcept
{
	const float c = std::cos(yawRadians);
	const float s = std::sin(yawRadians);
	return ClipTranslation{
		local.x * c + local.y * s,
		-local.x * s + local.y * c,
		local.z,
	};
}

[[nodiscard]] inline ClipTranslation translation_delta(
	ClipTranslation from, ClipTranslation to) noexcept
{
	return ClipTranslation{ to.x - from.x, to.y - from.y, to.z - from.z };
}

}  // namespace SpellHotbar::casts
