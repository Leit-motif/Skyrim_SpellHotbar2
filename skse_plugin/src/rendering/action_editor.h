#pragma once

#include <cstdint>

namespace SpellHotbar::ActionEditor {

	bool is_open();
	void open(std::uint32_t action_id);
	void close();
	void draw();

}  // namespace SpellHotbar::ActionEditor
