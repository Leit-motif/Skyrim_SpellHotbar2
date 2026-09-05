#include "action_editor.h"

#include "render_manager.h"
#include "texture_csv_loader.h"
#include "../game_data/action_definition.h"
#include "../game_data/game_data.h"
#include "../game_data/localization.h"
#include "../mcp/bind_capture.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>

namespace SpellHotbar::ActionEditor {

	namespace {

		bool show_dialog{ false };
		std::uint32_t editing_action_id{ 0 };
		std::array<char, 128> name_buf{};
		std::string draft_icon;
		std::uint32_t draft_icon_form{ 0 };
		ActionKind draft_kind{ ActionKind::physical_scancode };
		ActionTargetSource draft_target{ ActionTargetSource::captured };
		ActionInputDevice draft_captured_device{ ActionInputDevice::keyboard };
		std::uint32_t draft_captured_scancode{ 0 };
		float draft_stamina{ 0.0f };
		float draft_magicka{ 0.0f };
		float draft_health{ 0.0f };
		float draft_cooldown_days{ 0.0f };
		float draft_gcd{ 0.0f };
		bool persist_failed{ false };

		constexpr std::array<ActionKind, 1> kind_values{ ActionKind::physical_scancode };
		constexpr std::array<ActionTargetSource, 3> target_values{
			ActionTargetSource::ocpa_power,
			ActionTargetSource::dodge_hotkey,
			ActionTargetSource::captured,
		};

		const char* action_kind_key(ActionKind kind)
		{
			switch (kind) {
			case ActionKind::physical_scancode:
				return "$ACTION_KIND_PHYSICAL";
			}
			return "$ACTION_KIND_PHYSICAL";
		}

		const char* action_target_key(ActionTargetSource target)
		{
			switch (target) {
			case ActionTargetSource::ocpa_power:
				return "$ACTION_TARGET_OCPA";
			case ActionTargetSource::dodge_hotkey:
				return "$ACTION_TARGET_DODGE";
			case ActionTargetSource::captured:
				return "$ACTION_TARGET_CAPTURED";
			}
			return "$ACTION_TARGET_CAPTURED";
		}

		const char* action_input_device_key(ActionInputDevice device)
		{
			switch (device) {
			case ActionInputDevice::keyboard:
				return "$ACTION_DEVICE_KEYBOARD";
			case ActionInputDevice::mouse:
				return "$ACTION_DEVICE_MOUSE";
			case ActionInputDevice::gamepad:
				return "$ACTION_DEVICE_GAMEPAD";
			}
			return "$ACTION_DEVICE_KEYBOARD";
		}

		std::string captured_input_label()
		{
			if (draft_captured_scancode == 0) {
				return translate("$ACTION_UNBOUND");
			}
			return translate(action_input_device_key(draft_captured_device)) + " (DX " +
				std::to_string(draft_captured_scancode) + ")";
		}

		void load_from_action(const ActionDefinition& action)
		{
			name_buf.fill('\0');
			const auto name = action.display_name.substr(0, name_buf.size() - 1);
			std::copy(name.begin(), name.end(), name_buf.begin());
			draft_icon = action.icon;
			draft_icon_form = action.icon_form;
			draft_kind = action.kind;
			draft_target = action.target;
			draft_captured_device = action.captured_device;
			draft_captured_scancode = action.captured_scancode;
			draft_stamina = std::max(action.stamina_cost, 0.0f);
			draft_magicka = std::max(action.magicka_cost, 0.0f);
			draft_health = std::max(action.health_cost, 0.0f);
			draft_cooldown_days = std::max(action.cooldown_days, 0.0f);
			draft_gcd = std::max(action.gcd, 0.0f);
		}

		void apply_to_action(ActionDefinition& action)
		{
			action.display_name = name_buf.data();
			if (action.display_name.empty()) {
				if (const ActionDefinition* catalogue = GameData::get_action_catalogue(action.id)) {
					action.display_name = catalogue->display_name;
				}
			}
			action.icon = draft_icon;
			action.icon_form = draft_icon_form;
			action.kind = draft_kind;
			action.target = draft_target;
			action.captured_device = draft_captured_device;
			action.captured_scancode = draft_captured_scancode;
			action.stamina_cost = std::max(draft_stamina, 0.0f);
			action.magicka_cost = std::max(draft_magicka, 0.0f);
			action.health_cost = std::max(draft_health, 0.0f);
			action.cooldown_days = std::max(draft_cooldown_days, 0.0f);
			action.gcd = std::max(draft_gcd, 0.0f);
		}

		void draw_icon_picker(float scale_factor)
		{
			const ImGuiStyle& style = ImGui::GetStyle();
			const float icon_button_size = 60.0f * scale_factor;
			const float inner_pad = std::max(icon_button_size * 0.02f, 1.0f);
			const ImVec2 button_size(icon_button_size, icon_button_size);
			const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			auto& icon_list = RenderManager::get_editor_icon_list();
			int file_id = 0;
			for (auto& file : icon_list) {
				auto& name = std::get<0>(file);
				auto& file_icons = std::get<1>(file);
				if (ImGui::CollapsingHeader(name.c_str())) {
					for (int n = 0; n < static_cast<int>(file_icons.size()); ++n) {
						ImGui::PushID(file_id + n);
						const RE::FormID formid = std::get<0>(file_icons.at(n));
						const std::string& icon_str = std::get<1>(file_icons.at(n));
						const ImVec2 button_pos = ImGui::GetCursorScreenPos();
						const std::string button_label = "##action_icon" + name + std::to_string(n);
						if (ImGui::Button(button_label.c_str(), button_size)) {
							if (formid > 0) {
								draft_icon_form = formid;
								draft_icon.clear();
							} else if (!icon_str.empty()) {
								draft_icon = icon_str;
								draft_icon_form = 0;
							}
						}
						const ImVec2 inner_pos{ button_pos.x + inner_pad, button_pos.y + inner_pad };
						const int icon_size = static_cast<int>(icon_button_size - 2.0f * inner_pad);
						if (formid > 0) {
							RenderManager::draw_skill_in_editor(formid, inner_pos, icon_size, IM_COL32_WHITE);
						} else if (!icon_str.empty()) {
							if (TextureCSVLoader::default_icon_names.contains(icon_str)) {
								const auto type = TextureCSVLoader::default_icon_names.at(icon_str);
								RenderManager::draw_default_icon_in_editor(type, inner_pos, icon_size, IM_COL32_WHITE);
							} else {
								RenderManager::draw_extra_icon_in_editor(icon_str, inner_pos, icon_size, IM_COL32_WHITE);
							}
						}
						const float last_button_x2 = ImGui::GetItemRectMax().x;
						const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size.x;
						if (n + 1 < static_cast<int>(file_icons.size()) && next_button_x2 < window_visible_x2) {
							ImGui::SameLine();
						}
						ImGui::PopID();
					}
				}
				file_id += 100000;
			}
		}

		void update_captured_input_from_event()
		{
			if (const auto result = Mcp::bind_capture().take_action_capture_result(editing_action_id)) {
				draft_captured_device = result->input.device;
				draft_captured_scancode = result->input.dx_scancode;
			}
		}

	}  // namespace

	bool is_open()
	{
		return show_dialog;
	}

	void open(std::uint32_t action_id)
	{
		const ActionDefinition* action = GameData::get_action(action_id);
		if (action == nullptr) {
			return;
		}
		editing_action_id = action_id;
		load_from_action(*action);
		persist_failed = false;
		show_dialog = true;
	}

	void close()
	{
		if (editing_action_id != 0) {
			auto& capture = Mcp::bind_capture();
			if (capture.action_armed() && capture.pending_action_id() == editing_action_id) {
				capture.cancel();
			}
			capture.discard_action_capture_result(editing_action_id);
		}
		show_dialog = false;
		editing_action_id = 0;
		persist_failed = false;
	}

	void draw()
	{
		if (!show_dialog || editing_action_id == 0) {
			return;
		}

		ActionDefinition* action = GameData::get_action_mut(editing_action_id);
		if (action == nullptr) {
			close();
			return;
		}
		update_captured_input_from_event();

		static constexpr ImGuiWindowFlags window_flag =
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

		auto& io = ImGui::GetIO();
		io.MouseDrawCursor = true;

		RenderManager::draw_frame_bg(nullptr);
		const auto [screen_size_x, screen_size_y, window_width] = RenderManager::calculate_frame_size(0.82f);
		(void)screen_size_x;
		(void)window_width;
		ImGui::SetNextWindowBgAlpha(0.0F);
		const float scale_factor = screen_size_y / 1080.0f;

		RenderManager::ImGui_push_title_style();
		ImGui::Begin(translate_c("$ACTION_EDITOR"), nullptr, window_flag);
		RenderManager::ImGui_pop_title_style();
		ImGui::BeginChild("##action_editor", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_None);

		RenderManager::set_large_font();
		const float button_height = ImGui::CalcTextSize("Cancel").y +
			ImGui::GetStyle().FramePadding.y * 2.0f + ImGui::GetStyle().ItemSpacing.y * 2.0f;
		RenderManager::revert_font();
		const float child_window_height = ImGui::GetContentRegionAvail().y - button_height;

		ImGui::BeginChild("ActionLeft", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, child_window_height), true);
		ImGui::TextUnformatted(translate_c("$COLUMN_NAME"));
		ImGui::InputText("##action_name", name_buf.data(), name_buf.size());

		ImGui::TextUnformatted(translate_c("$COLUMN_ICON"));
		const ImVec2 icon_pos = ImGui::GetCursorScreenPos();
		const float icon_size = std::round(60.0f * scale_factor);
		ImGui::Dummy(ImVec2(icon_size, icon_size));
		RenderManager::draw_art_icon_fields_in_editor(draft_icon_form, draft_icon, icon_pos,
			static_cast<int>(icon_size), IM_COL32_WHITE);

		ImGui::TextUnformatted(translate_c("$ACTION_KIND"));
		if (ImGui::BeginCombo("##action_kind", translate_c(action_kind_key(draft_kind)))) {
			for (const ActionKind value : kind_values) {
				const bool selected = draft_kind == value;
				if (ImGui::Selectable(translate_c(action_kind_key(value)), selected)) {
					draft_kind = value;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::TextUnformatted(translate_c("$ACTION_TARGET"));
		if (ImGui::BeginCombo("##action_target", translate_c(action_target_key(draft_target)))) {
			for (const ActionTargetSource value : target_values) {
				const bool selected = draft_target == value;
				if (ImGui::Selectable(translate_c(action_target_key(value)), selected)) {
					draft_target = value;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		const bool captured_target = draft_target == ActionTargetSource::captured;
		if (captured_target) {
			ImGui::Text("%s: %s", translate_c("$ACTION_SCANCODE"), captured_input_label().c_str());
			ImGui::SameLine();
			auto& capture = Mcp::bind_capture();
			const bool capture_armed = capture.action_armed() &&
				capture.pending_action_id() == editing_action_id;
			const char* capture_key = capture_armed ? "$ACTION_CAPTURE_CANCEL" : "$ACTION_CAPTURE";
			if (ImGui::Button(translate_id(capture_key).c_str())) {
				if (capture_armed) {
					capture.cancel();
				} else {
					capture.arm_action(editing_action_id);
				}
			}
			if (capture_armed) {
				ImGui::TextUnformatted(translate_c("$ACTION_CAPTURE_ARMED"));
			}
		} else {
			ImGui::BeginDisabled();
			ImGui::Text("%s: %s", translate_c("$ACTION_SCANCODE"), captured_input_label().c_str());
			ImGui::EndDisabled();
		}

		ImGui::InputFloat(translate_id("$STAMINA_COST").c_str(), &draft_stamina, 1.0f, 5.0f, "%.0f");
		ImGui::InputFloat(translate_id("$MAGICKA_COST").c_str(), &draft_magicka, 1.0f, 5.0f, "%.0f");
		ImGui::InputFloat(translate_id("$HEALTH_COST").c_str(), &draft_health, 1.0f, 5.0f, "%.0f");
		ImGui::InputFloat(translate_id("$COOLDOWN_DAYS").c_str(), &draft_cooldown_days, 0.01f, 0.1f, "%.4f");
		ImGui::InputFloat(translate_id("$GLOBAL_COOLDOWN").c_str(), &draft_gcd, 0.1f, 0.5f, "%.1f");

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("ActionRight", ImVec2(0.0f, child_window_height), true, ImGuiWindowFlags_HorizontalScrollbar);
		draw_icon_picker(scale_factor);
		ImGui::EndChild();

		RenderManager::set_large_font();
		if (ImGui::Button(translate_id("$SAVE").c_str())) {
			ActionDefinition candidate = *action;
			apply_to_action(candidate);
			if (GameData::persist_action_player_overlay(candidate)) {
				*action = std::move(candidate);
				close();
			} else {
				persist_failed = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(translate_id("$RESET").c_str())) {
			if (const ActionDefinition* catalogue = GameData::get_action_catalogue(action->id)) {
				load_from_action(*catalogue);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(translate_id("$CANCEL").c_str())) {
			close();
		}
		if (persist_failed) {
			ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s",
				translate_c("$ACTION_SAVE_FAILED"));
		}
		RenderManager::revert_font();

		ImGui::EndChild();
		ImGui::End();
		RenderManager::draw_custom_mouse_cursor();
	}

}  // namespace SpellHotbar::ActionEditor
