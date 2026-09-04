#include "art_icon_edit_dialog.h"

#include "../game_data/game_data.h"
#include "../game_data/localization.h"
#include "render_manager.h"
#include "texture_csv_loader.h"

namespace SpellHotbar::ArtIconEditor {

	namespace {

		bool show_dialog{ false };
		uint32_t editing_art_id{ 0 };
		std::string draft_icon;
		std::uint32_t draft_icon_form{ 0 };
		std::string saved_icon;
		std::uint32_t saved_icon_form{ 0 };
		std::string catalogue_icon;
		std::uint32_t catalogue_icon_form{ 0 };

		void draw_draft_icon(ImVec2 iconpos, int ic_size)
		{
			RenderManager::draw_art_icon_fields_in_editor(draft_icon_form, draft_icon, iconpos, ic_size,
				IM_COL32_WHITE);
		}

		bool draft_differs_from_saved()
		{
			return draft_icon != saved_icon || draft_icon_form != saved_icon_form;
		}

	}  // namespace

	bool is_open()
	{
		return show_dialog;
	}

	void open(uint32_t art_id)
	{
		const ArtDefinition* art = GameData::get_art(art_id);
		if (art == nullptr) {
			return;
		}
		editing_art_id = art_id;
		draft_icon = art->icon;
		draft_icon_form = art->icon_form;
		saved_icon = art->icon;
		saved_icon_form = art->icon_form;
		catalogue_icon = art->icon;
		catalogue_icon_form = art->icon_form;
		GameData::get_art_catalogue_icon(art_id, catalogue_icon, catalogue_icon_form);
		show_dialog = true;
	}

	void close()
	{
		show_dialog = false;
		editing_art_id = 0;
	}

	void draw()
	{
		if (!show_dialog || editing_art_id == 0) {
			return;
		}

		const ArtDefinition* art = GameData::get_art(editing_art_id);
		if (art == nullptr) {
			close();
			return;
		}

		static constexpr ImGuiWindowFlags window_flag =
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

		auto& io = ImGui::GetIO();
		io.MouseDrawCursor = true;

		RenderManager::calculate_frame_size(0.775f);
		RenderManager::draw_frame_bg(nullptr);

		auto [screen_size_x, screen_size_y, window_width] = RenderManager::calculate_frame_size(0.75f);
		(void)screen_size_x;
		(void)window_width;
		ImGui::SetNextWindowBgAlpha(0.0F);

		const float scale_factor = screen_size_y / 1080.0f;

		RenderManager::ImGui_push_title_style();
		ImGui::Begin(translate_c("$EDIT_ICON"), nullptr, window_flag);
		RenderManager::ImGui_pop_title_style();
		ImGui::BeginChild("##art_icon_editor", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_None);

		static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
									   ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody |
									   ImGuiTableFlags_ScrollY;

		RenderManager::set_large_font();
		const float button_height =
			ImGui::CalcTextSize("Cancel").y + ImGui::GetStyle().FramePadding.y * 2.0f + ImGui::GetStyle().ItemSpacing.y * 2.0f;
		RenderManager::revert_font();
		const float child_window_height = ImGui::GetContentRegionAvail().y - button_height;

		ImGui::BeginChild("LeftTab", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, child_window_height), false,
			ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::BeginTable("ArtIconData", 2, flags, ImVec2(0.0f, 0.0f), 0.0f)) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 0);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(translate_c("$COLUMN_NAME"));
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(art->display_name.c_str());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(translate_c("$COLUMN_ICON"));
			ImGui::TableNextColumn();
			const ImVec2 iconpos = ImGui::GetCursorScreenPos();
			const float ic_size = std::round(60.0f * scale_factor);
			ImGui::Dummy(ImVec2(ic_size, ic_size));
			draw_draft_icon(iconpos, static_cast<int>(ic_size));

			const bool show_reset =
				draft_icon != catalogue_icon || draft_icon_form != catalogue_icon_form;
			if (show_reset) {
				ImGui::SameLine();
				if (ImGui::Button((translate("$RESET") + "##reset_art_icon").c_str())) {
					draft_icon = catalogue_icon;
					draft_icon_form = catalogue_icon_form;
				}
			}

			ImGui::EndTable();
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("RightTab", ImVec2(0, child_window_height), false, ImGuiWindowFlags_HorizontalScrollbar);

		const ImGuiStyle& style = ImGui::GetStyle();
		const float icon_button_size = 60.0f * scale_factor;
		const float inner_pad = std::max(icon_button_size * 0.02f, 1.0f);
		const ImVec2 button_sz(icon_button_size, icon_button_size);
		const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		auto icon_list = RenderManager::get_editor_icon_list();
		int file_id = 0;
		for (auto& file : icon_list) {
			auto& name = std::get<0>(file);
			auto& file_icons = std::get<1>(file);
			if (ImGui::CollapsingHeader(name.c_str())) {
				for (int n = 0; n < static_cast<int>(file_icons.size()); n++) {
					ImGui::PushID(file_id + n);
					const RE::FormID formid = std::get<0>(file_icons.at(n));
					const std::string& icon_str = std::get<1>(file_icons.at(n));
					const ImVec2 bpos = ImGui::GetCursorScreenPos();
					const std::string button_label = "##" + name + std::to_string(n);
					if (ImGui::Button(button_label.c_str(), button_sz)) {
						if (formid > 0) {
							draft_icon_form = formid;
							draft_icon.clear();
						} else if (!icon_str.empty()) {
							draft_icon = icon_str;
							draft_icon_form = 0;
						}
					}
					const ImVec2 inner_pos{ bpos.x + inner_pad, bpos.y + inner_pad };
					const int icon_size = static_cast<int>(icon_button_size - 2.0f * inner_pad);
					if (formid > 0) {
						RenderManager::draw_skill_in_editor(formid, inner_pos, icon_size, IM_COL32_WHITE);
					} else if (!icon_str.empty()) {
						if (TextureCSVLoader::default_icon_names.contains(icon_str)) {
							auto type = TextureCSVLoader::default_icon_names.at(icon_str);
							RenderManager::draw_default_icon_in_editor(type, inner_pos, icon_size, IM_COL32_WHITE);
						} else {
							RenderManager::draw_extra_icon_in_editor(icon_str, inner_pos, icon_size, IM_COL32_WHITE);
						}
					}
					const float last_button_x2 = ImGui::GetItemRectMax().x;
					const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x;
					if (n + 1 < static_cast<int>(file_icons.size()) && next_button_x2 < window_visible_x2) {
						ImGui::SameLine();
					}
					ImGui::PopID();
				}
			}
			file_id += 100000;
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		const bool save_enabled = draft_differs_from_saved();
		RenderManager::set_large_font();
		if (!save_enabled) {
			ImGui::BeginDisabled();
		}
		if (ImGui::Button(translate_id("$SAVE").c_str())) {
			if (draft_icon == catalogue_icon && draft_icon_form == catalogue_icon_form) {
				GameData::reset_art_icon(editing_art_id);
			} else {
				GameData::set_art_icon(editing_art_id, draft_icon, draft_icon_form);
			}
			GameData::persist_user_art_icons();
			close();
		}
		if (!save_enabled) {
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		if (ImGui::Button(translate_id("$CANCEL").c_str())) {
			close();
		}
		RenderManager::revert_font();

		ImGui::EndChild();
		ImGui::End();

		RenderManager::draw_custom_mouse_cursor();
	}

}  // namespace SpellHotbar::ArtIconEditor
