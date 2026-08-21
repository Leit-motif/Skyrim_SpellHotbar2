#pragma once

#include <cstdint>

namespace SpellHotbar::ArtIconEditor {

	bool is_open();
	void open(uint32_t art_id);
	void close();
	void draw();

}  // namespace SpellHotbar::ArtIconEditor
