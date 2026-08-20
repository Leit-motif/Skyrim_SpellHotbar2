#include "art_definition.h"

#include <algorithm>
#include <charconv>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace SpellHotbar {

namespace {

constexpr float hours_to_days = 1.0f / 24.0f;
constexpr float minutes_to_days = 1.0f / (24.0f * 60.0f);
constexpr float seconds_to_days = 1.0f / (24.0f * 60.0f * 60.0f);

const std::regex re_time_str(R"((-?\d+(\.\d*)?)([smh]?))");

std::vector<std::string> split_tabs(std::string_view line)
{
	std::vector<std::string> out;
	std::string current;
	for (char c : line) {
		if (c == '\t') {
			out.push_back(std::move(current));
			current.clear();
		} else if (c != '\r') {
			current.push_back(c);
		}
	}
	out.push_back(std::move(current));
	return out;
}

std::optional<int> to_int(std::string_view s)
{
	if (s.empty()) {
		return std::nullopt;
	}
	int value = 0;
	const auto* begin = s.data();
	const auto* end = s.data() + s.size();
	const auto [ptr, ec] = std::from_chars(begin, end, value);
	if (ec != std::errc{} || ptr != end) {
		return std::nullopt;
	}
	return value;
}

std::optional<float> to_float(std::string_view s)
{
	if (s.empty()) {
		return std::nullopt;
	}
	std::string copy{s};
	std::replace(copy.begin(), copy.end(), ',', '.');
	try {
		size_t idx = 0;
		const float value = std::stof(copy, &idx);
		if (idx != copy.size()) {
			return std::nullopt;
		}
		return value;
	} catch (const std::exception&) {
		return std::nullopt;
	}
}

std::string cell(const std::vector<std::string>& cols,
	const std::unordered_map<std::string, std::size_t>& index, const char* name)
{
	const auto it = index.find(name);
	if (it == index.end() || it->second >= cols.size()) {
		return {};
	}
	return cols[it->second];
}

std::string first_nonempty_line(std::string_view text)
{
	std::istringstream stream{std::string{text}};
	std::string line;
	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		const auto start = line.find_first_not_of(" \t");
		if (start == std::string::npos) {
			continue;
		}
		const auto end = line.find_last_not_of(" \t");
		return line.substr(start, end - start + 1);
	}
	return {};
}

}  // namespace

std::optional<float> parse_art_duration_days(std::string_view time_str)
{
	std::string input{time_str};
	std::replace(input.begin(), input.end(), ',', '.');

	std::smatch m;
	if (!std::regex_match(input, m, re_time_str) || m.size() < 2) {
		return std::nullopt;
	}

	float value = 0.0f;
	try {
		value = std::stof(m[1].str());
	} catch (const std::exception&) {
		return std::nullopt;
	}

	float factor = seconds_to_days;
	if (m.size() > 2) {
		const std::string dur = m[m.size() - 1].str();
		if (dur == "m") {
			factor = minutes_to_days;
		} else if (dur == "h") {
			factor = hours_to_days;
		}
	}

	if (value > 0.0f) {
		return value * factor;
	}
	return -1.0f;
}

ArtClass parse_art_class(std::string_view text)
{
	if (text == "1H") {
		return ArtClass::OneHand;
	}
	if (text == "2H") {
		return ArtClass::TwoHand;
	}
	if (text == "Dual") {
		return ArtClass::Dual;
	}
	return ArtClass::Generic;
}

std::vector<ArtDefinition> parse_art_tsv(std::string_view text)
{
	std::vector<ArtDefinition> arts;
	std::istringstream stream{std::string{text}};
	std::string line;
	if (!std::getline(stream, line)) {
		return arts;
	}
	if (!line.empty() && line.back() == '\r') {
		line.pop_back();
	}

	const auto headers = split_tabs(line);
	std::unordered_map<std::string, std::size_t> index;
	for (std::size_t i = 0; i < headers.size(); ++i) {
		index.emplace(headers[i], i);
	}

	const bool ok = index.contains("ArtID") && index.contains("DisplayName") &&
					index.contains("Icon") && index.contains("Selector") &&
					index.contains("ArtClass") && index.contains("StaminaCost") &&
					index.contains("Cooldown") && index.contains("GlobalCooldown");
	if (!ok) {
		return arts;
	}

	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (line.empty()) {
			continue;
		}
		const auto cols = split_tabs(line);
		ArtDefinition art;
		const auto id = to_int(cell(cols, index, "ArtID"));
		if (!id || *id <= 0) {
			continue;
		}
		art.id = static_cast<std::uint32_t>(*id);
		art.display_name = cell(cols, index, "DisplayName");
		if (art.display_name.empty()) {
			continue;
		}
		art.icon = cell(cols, index, "Icon");
		art.selector = to_int(cell(cols, index, "Selector")).value_or(0);
		art.art_class = parse_art_class(cell(cols, index, "ArtClass"));
		art.stamina_cost = to_float(cell(cols, index, "StaminaCost")).value_or(0.0f);
		art.gcd = to_float(cell(cols, index, "GlobalCooldown")).value_or(1.0f);
		if (const auto cd = parse_art_duration_days(cell(cols, index, "Cooldown"))) {
			art.cooldown_days = *cd;
		} else {
			art.cooldown_days = -1.0f;
		}
		arts.push_back(std::move(art));
	}
	return arts;
}

std::optional<int> parse_custom_art_folder_number(std::string_view folder_name)
{
	static const std::regex re(R"(^Weapon_Art_(\d+)$)", std::regex::icase);
	std::string input{folder_name};
	std::smatch m;
	if (!std::regex_match(input, m, re) || m.size() < 2) {
		return std::nullopt;
	}
	return to_int(m[1].str());
}

ArtDefinition custom_art_from_folder(int folder_number, std::string_view folder_name,
	std::string_view name_file, std::string_view icon_file, bool has_clip)
{
	ArtDefinition art;
	art.id = custom_art_id_base + static_cast<std::uint32_t>(folder_number);
	art.selector = static_cast<int>(art.id);
	art.art_class = ArtClass::Generic;
	art.stamina_cost = 25.0f;
	art.cooldown_days = parse_art_duration_days("8s").value_or(-1.0f);
	art.gcd = 1.0f;
	art.has_clip = has_clip;
	art.display_name = first_nonempty_line(name_file);
	if (art.display_name.empty()) {
		art.display_name = std::string{folder_name};
	}
	art.icon = first_nonempty_line(icon_file);
	if (art.icon.empty()) {
		art.icon = "GREATER_POWER";
	}
	return art;
}

}  // namespace SpellHotbar
