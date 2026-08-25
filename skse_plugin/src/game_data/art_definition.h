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
	std::uint32_t icon_form{0};
	int selector{0};
	ArtClass art_class{ArtClass::Generic};
	float stamina_cost{0.0f};
	float magicka_cost{0.0f};
	float health_cost{0.0f};
	float cooldown_days{-1.0f};
	float gcd{1.0f};
	bool has_clip{true};
	std::uint32_t spell_local_form{0};
	std::string spell_plugin;
	bool self_target{false};
	std::string folder_path;
	std::string cooldown_text{"8s"};
};

constexpr std::uint32_t custom_art_id_base = 1000;

[[nodiscard]] constexpr bool is_custom_ability(std::uint32_t art_id) noexcept
{
	return art_id >= custom_art_id_base;
}

// Custom Ability catalogue row from a numbered drop-in folder. Sidecar ability.ini is the
// source of truth (ADR-0009). name.txt / icon.txt remain a fallback when no sidecar exists.
// ArtID and selector are custom_art_id_base + folder number (not a hotbar slot).
ArtDefinition custom_art_from_folder(int folder_number, std::string_view folder_name,
	std::string_view name_file, std::string_view icon_file, bool has_clip,
	std::string_view sidecar_text = {});

std::optional<int> parse_custom_art_folder_number(std::string_view folder_name);

// Parse a tab-separated art catalogue. Header row required. Rows whose ArtID is 0 or whose
// DisplayName is empty are skipped. Cooldown uses the same 30s / 1.5h / 12m grammar as spell
// data; a non-positive value means no cooldown (-1). ArtClass values are 1H, 2H, Dual, Generic;
// an empty cell is Generic.
std::vector<ArtDefinition> parse_art_tsv(std::string_view text);

std::optional<float> parse_art_duration_days(std::string_view time_str);

ArtClass parse_art_class(std::string_view text);

struct ArtPlayerOverlay {
	std::string display_name;
	std::string icon;
	std::uint32_t icon_form{ 0 };
	ArtClass art_class{ ArtClass::Generic };
	float stamina_cost{ 0.0f };
	float magicka_cost{ 0.0f };
	float health_cost{ 0.0f };
	std::string cooldown;
	float gcd{ 1.0f };
};

void apply_art_player_overlay(ArtDefinition& art, const ArtPlayerOverlay& overlay);

[[nodiscard]] ArtPlayerOverlay art_player_overlay_from(const ArtDefinition& art);

[[nodiscard]] bool art_matches_catalogue_tuning(const ArtDefinition& live, const ArtDefinition& catalogue) noexcept;

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

// Bind-menu gray-out (ticket 13). Bar ids are Bars::* fourCCs from hotbars.h.
// Sneak bars are parent+1. Live if any EquippedType that opens this bar (or, for Main/Melee,
// any child bar) would accept the class. Magic / Ranged / Vampire Lord / Werewolf: none live.
[[nodiscard]] constexpr std::uint32_t art_bar_stance_root(std::uint32_t bar_id) noexcept
{
	switch (bar_id) {
	case static_cast<std::uint32_t>('MAIN') + 1:
		return static_cast<std::uint32_t>('MAIN');
	case static_cast<std::uint32_t>('MELE') + 1:
		return static_cast<std::uint32_t>('MELE');
	case static_cast<std::uint32_t>('1HSD') + 1:
		return static_cast<std::uint32_t>('1HSD');
	case static_cast<std::uint32_t>('1HSP') + 1:
		return static_cast<std::uint32_t>('1HSP');
	case static_cast<std::uint32_t>('1HDW') + 1:
		return static_cast<std::uint32_t>('1HDW');
	case static_cast<std::uint32_t>('2HND') + 1:
		return static_cast<std::uint32_t>('2HND');
	case static_cast<std::uint32_t>('RNGD') + 1:
		return static_cast<std::uint32_t>('RNGD');
	case static_cast<std::uint32_t>('MAGC') + 1:
		return static_cast<std::uint32_t>('MAGC');
	default:
		return bar_id;
	}
}

[[nodiscard]] constexpr bool art_class_is_live_on_bar(ArtClass art_class, std::uint32_t bar_id) noexcept
{
	using GameData::EquippedType;
	const std::uint32_t bar = art_bar_stance_root(bar_id);
	const bool dual_wield_bar = art_class_is_live(art_class, EquippedType::ONEHAND_EMPTY) ||
								art_class_is_live(art_class, EquippedType::DUAL_WIELD);
	const bool shield_bar = art_class_is_live(art_class, EquippedType::ONEHAND_SHIELD);
	const bool spell_bar = art_class_is_live(art_class, EquippedType::ONEHAND_SPELL);
	const bool two_hand_bar = art_class_is_live(art_class, EquippedType::TWOHAND);
	const bool melee_parent = dual_wield_bar || shield_bar || spell_bar || two_hand_bar;
	switch (bar) {
	case static_cast<std::uint32_t>('1HDW'):
		return dual_wield_bar;
	case static_cast<std::uint32_t>('1HSD'):
		return shield_bar;
	case static_cast<std::uint32_t>('1HSP'):
		return spell_bar;
	case static_cast<std::uint32_t>('2HND'):
		return two_hand_bar;
	case static_cast<std::uint32_t>('MELE'):
		return melee_parent;
	case static_cast<std::uint32_t>('MAIN'):
		return art_class_is_live(art_class, EquippedType::FIST) || melee_parent;
	default:
		return false;
	}
}

// Bind-menu direct match (ticket 14). A class is direct when the selected bar *is* its stance, so
// the row is specific to that bar rather than merely live on it. Generic runs on every stance and
// is therefore never direct; parent bars union their children, so nothing is direct on them.
[[nodiscard]] constexpr bool art_class_is_direct_on_bar(ArtClass art_class, std::uint32_t bar_id) noexcept
{
	if (art_class == ArtClass::Generic) {
		return false;
	}
	switch (art_bar_stance_root(bar_id)) {
	case static_cast<std::uint32_t>('1HSD'):
	case static_cast<std::uint32_t>('1HSP'):
		return art_class == ArtClass::OneHand;
	case static_cast<std::uint32_t>('1HDW'):
		return art_class == ArtClass::Dual;
	case static_cast<std::uint32_t>('2HND'):
		return art_class == ArtClass::TwoHand;
	default:
		return false;
	}
}

}  // namespace SpellHotbar
