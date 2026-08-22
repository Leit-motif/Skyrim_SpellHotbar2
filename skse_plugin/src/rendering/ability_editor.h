#pragma once

#include <cstdint>

namespace SpellHotbar::AbilityEditor {

	bool is_open();
	void open(uint32_t art_id);
	void close();
	void draw();

}  // namespace SpellHotbar::AbilityEditor
