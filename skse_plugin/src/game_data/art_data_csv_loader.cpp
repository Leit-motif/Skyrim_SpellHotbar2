#include "art_data_csv_loader.h"
#include "art_definition.h"
#include "csv_loader.h"
#include "game_data.h"
#include "../logger/logger.h"
#include <fstream>
#include <sstream>

namespace SpellHotbar::ArtDataCSVLoader {

	void load_art_file(const std::string& path)
	{
		std::ifstream in(path);
		if (!in) {
			logger::error("Could not open art data '{}'", path);
			return;
		}
		std::ostringstream buf;
		buf << in.rdbuf();
		auto arts = parse_art_tsv(buf.str());
		if (arts.empty()) {
			logger::warn("Could not parse '{}', skipping", path);
			return;
		}
		for (auto& art : arts) {
			logger::info("Loaded weapon art {} '{}' selector={} class={}", art.id, art.display_name, art.selector,
				art_class_label(art.art_class));
			GameData::set_art(std::move(art));
		}
	}

	void load_art_data(std::filesystem::path folder)
	{
		if (!std::filesystem::exists(folder)) {
			logger::warn("Art data folder '{}' is missing; no Weapon Arts loaded", folder.string());
			return;
		}
		csv::load_folder(folder, "artdata", load_art_file);
	}
}
