#pragma once

#include <array>
#include <charconv>
#include <cstdint>
#include <string_view>
#include <system_error>

namespace SpellHotbar::Input {

struct KeyboardTapPhase {
	float value{ 0.0f };
	float held_duration{ 0.0f };
};

// VoiceCastDriver: a down-only ButtonEvent leaves a phantom held key. A tap is
// always down then up; the up carries a tiny hold so the engine treats it as a
// release rather than an instant no-op.
inline constexpr float kKeyboardTapReleaseHeldSecs = 0.001f;

constexpr std::array<KeyboardTapPhase, 2> keyboard_tap_phases()
{
	return {{
		{ 1.0f, 0.0f },
		{ 0.0f, kKeyboardTapReleaseHeldSecs },
	}};
}

struct OcpaKeys {
	uint32_t power{ 0 };
	uint32_t dual{ 0 };
};

namespace action_inject_detail {

inline std::string_view ltrim(std::string_view value)
{
	while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
		value.remove_prefix(1);
	}
	return value;
}

inline uint32_t parse_positive_int(std::string_view value)
{
	value = ltrim(value);
	int code{ 0 };
	const auto* first = value.data();
	const auto* last = first + value.size();
	if (std::from_chars(first, last, code).ec != std::errc{}) {
		return 0;
	}
	return code > 0 ? static_cast<uint32_t>(code) : 0U;
}

inline bool line_is_comment(std::string_view line)
{
	line = ltrim(line);
	return !line.empty() && line.front() == ';';
}

}  // namespace action_inject_detail

inline OcpaKeys parse_ocpa_keys(std::string_view text)
{
	OcpaKeys found{};
	int seen{ 0 };
	while (!text.empty() && seen < 2) {
		const auto nl = text.find_first_of("\r\n");
		const auto line = nl == std::string_view::npos ? text : text.substr(0, nl);
		if (nl == std::string_view::npos) {
			text = {};
		} else {
			text.remove_prefix(nl + 1);
		}
		const auto eq = line.find('=');
		if (eq == std::string_view::npos || line.find("iKeycode") == std::string_view::npos) {
			continue;
		}
		const auto code = action_inject_detail::parse_positive_int(line.substr(eq + 1));
		(seen == 0 ? found.power : found.dual) = code;
		++seen;
	}
	return found;
}

inline uint32_t parse_dodge_hotkey(std::string_view text)
{
	while (!text.empty()) {
		const auto nl = text.find_first_of("\r\n");
		const auto line = nl == std::string_view::npos ? text : text.substr(0, nl);
		if (nl == std::string_view::npos) {
			text = {};
		} else {
			text.remove_prefix(nl + 1);
		}
		if (action_inject_detail::line_is_comment(line)) {
			continue;
		}
		const auto key = line.find("DodgeHotkey");
		const auto eq = line.find('=');
		if (key == std::string_view::npos || eq == std::string_view::npos || eq < key) {
			continue;
		}
		return action_inject_detail::parse_positive_int(line.substr(eq + 1));
	}
	return 0;
}

}  // namespace SpellHotbar::Input
