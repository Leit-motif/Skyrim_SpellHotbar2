#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace SpellHotbar {

struct ArtDefinition {
	std::uint32_t id{0};
	std::string display_name;
	std::string icon;
	int selector{0};
	float stamina_cost{0.0f};
	float cooldown_days{-1.0f};
	float gcd{1.0f};
};

// Parse a tab-separated art catalogue. Header row required. Rows whose ArtID is 0 or whose
// DisplayName is empty are skipped. Cooldown uses the same 30s / 1.5h / 12m grammar as spell
// data; a non-positive value means no cooldown (-1).
std::vector<ArtDefinition> parse_art_tsv(std::string_view text);

std::optional<float> parse_art_duration_days(std::string_view time_str);

}  // namespace SpellHotbar
