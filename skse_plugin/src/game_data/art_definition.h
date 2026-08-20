#pragma once

#include "equipped_type.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace SpellHotbar {

enum class ArtClass {
	OneHand,
	TwoHand,
	Dual,
	Generic
};

struct ArtDefinition {
	std::uint32_t id{0};
	std::string display_name;
	std::string icon;
	int selector{0};
	ArtClass art_class{ArtClass::Generic};
	float stamina_cost{0.0f};
	float cooldown_days{-1.0f};
	float gcd{1.0f};
};

// Parse a tab-separated art catalogue. Header row required. Rows whose ArtID is 0 or whose
// DisplayName is empty are skipped. Cooldown uses the same 30s / 1.5h / 12m grammar as spell
// data; a non-positive value means no cooldown (-1). ArtClass values are 1H, 2H, Dual, Generic;
// an empty cell is Generic.
std::vector<ArtDefinition> parse_art_tsv(std::string_view text);

std::optional<float> parse_art_duration_days(std::string_view time_str);

ArtClass parse_art_class(std::string_view text);

[[nodiscard]] constexpr const char* art_class_label(ArtClass art_class) noexcept
{
	switch (art_class) {
	case ArtClass::OneHand:
		return "1H";
	case ArtClass::TwoHand:
		return "2H";
	case ArtClass::Dual:
		return "Dual";
	case ArtClass::Generic:
		return "Generic";
	}
	return "Generic";
}

[[nodiscard]] constexpr bool art_class_is_live(ArtClass art_class, GameData::EquippedType equipped) noexcept
{
	using GameData::EquippedType;
	switch (art_class) {
	case ArtClass::OneHand:
		return equipped == EquippedType::ONEHAND_EMPTY || equipped == EquippedType::ONEHAND_SHIELD ||
			   equipped == EquippedType::ONEHAND_SPELL;
	case ArtClass::TwoHand:
		return equipped == EquippedType::TWOHAND;
	case ArtClass::Dual:
		return equipped == EquippedType::DUAL_WIELD;
	case ArtClass::Generic:
		return equipped == EquippedType::FIST || art_class_is_live(ArtClass::OneHand, equipped) ||
			   art_class_is_live(ArtClass::TwoHand, equipped) ||
			   art_class_is_live(ArtClass::Dual, equipped);
	}
	return false;
}

}  // namespace SpellHotbar
