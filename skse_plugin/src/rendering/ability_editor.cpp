#include "ability_editor.h"

#include "../game_data/custom_ability_config.h"
#include "../game_data/custom_ability_runtime.h"
#include "../game_data/game_data.h"
#include "../game_data/localization.h"
#include "render_manager.h"
#include "texture_csv_loader.h"

#include <algorithm>
#include <array>
#include <string>

namespace SpellHotbar::AbilityEditor {

	namespace {

		bool show_dialog{ false };
		uint32_t editing_art_id{ 0 };
		std::array<char, 128> name_buf{};
		std::array<char, 32> cooldown_buf{};
		std::string draft_icon;
		std::uint32_t draft_icon_form{ 0 };
		ArtClass draft_class{ ArtClass::Generic };
		float draft_stamina{ 25.0f };
		float draft_magicka{ 0.0f };
		float draft_health{ 0.0f };
		float draft_gcd{ 1.0f };
		std::uint32_t draft_spell_local{ vanilla_firebolt_local_form };
		std::string draft_spell_plugin{ vanilla_firebolt_plugin };
		bool draft_self_target{ false };

		constexpr std::array<ArtClass, 4> class_values{
			ArtClass::OneHand, ArtClass::TwoHand, ArtClass::Dual, ArtClass::Generic };

		void load_from_art(const ArtDefinition& art)
		{
			name_buf.fill('\0');
			const auto name = art.display_name.substr(0, name_buf.size() - 1);
			std::copy(name.begin(), name.end(), name_buf.begin());
			cooldown_buf.fill('\0');
			const auto cd = art.cooldown_text.empty() ? std::string{ "8s" } : art.cooldown_text;
			const auto cd_clip = cd.substr(0, cooldown_buf.size() - 1);
			std::copy(cd_clip.begin(), cd_clip.end(), cooldown_buf.begin());
			draft_icon = art.icon;
			draft_icon_form = art.icon_form;
			draft_class = art.art_class;
			draft_stamina = art.stamina_cost;
			draft_magicka = art.magicka_cost;
			draft_health = art.health_cost;
			draft_gcd = art.gcd;
			draft_spell_local = art.spell_local_form == 0 ? vanilla_firebolt_local_form : art.spell_local_form;
			draft_spell_plugin = art.spell_plugin.empty() ? vanilla_firebolt_plugin : art.spell_plugin;
			draft_self_target = art.self_target;
		}

		void apply_to_art(ArtDefinition& art)
		{
			art.display_name = name_buf.data();
			if (art.display_name.empty()) {
				if (is_custom_ability(art.id)) {
					art.display_name = "Custom Ability " + std::to_string(art.id - custom_art_id_base);
				} else if (const ArtDefinition* catalogue = GameData::get_art_catalogue(art.id)) {
					art.display_name = catalogue->display_name;
				}
			}
			art.icon = draft_icon;
			art.icon_form = draft_icon_form;
			art.art_class = draft_class;
			art.stamina_cost = draft_stamina;
			art.magicka_cost = draft_magicka;
			art.health_cost = draft_health;
			art.gcd = draft_gcd;
			art.cooldown_text = cooldown_buf.data();
			if (art.cooldown_text.empty()) {
				art.cooldown_text = "8s";
			}
			if (const auto days = parse_art_duration_days(art.cooldown_text)) {
				art.cooldown_days = *days;
			}
			art.spell_local_form = draft_spell_local;
			art.spell_plugin = draft_spell_plugin;
			art.self_target = draft_self_target;
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
		load_from_art(*art);
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

		ArtDefinition* art = GameData::get_art_mut(editing_art_id);
		if (art == nullptr) {
			close();
			return;
		}

		static constexpr ImGuiWindowFlags window_flag =
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

		auto& io = ImGui::GetIO();
		io.MouseDrawCursor = true;

		RenderManager::draw_frame_bg(nullptr);
		auto [screen_size_x, screen_size_y, window_width] = RenderManager::calculate_frame_size(0.82f);
		(void)screen_size_x;
		(void)window_width;
		ImGui::SetNextWindowBgAlpha(0.0F);
		const float scale_factor = screen_size_y / 1080.0f;

		RenderManager::ImGui_push_title_style();
		ImGui::Begin(translate_c("$ABILITY_EDITOR"), nullptr, window_flag);
		RenderManager::ImGui_pop_title_style();
		ImGui::BeginChild("##ability_editor", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_None);

		RenderManager::set_large_font();
		const float button_height =
			ImGui::CalcTextSize("Cancel").y + ImGui::GetStyle().FramePadding.y * 2.0f + ImGui::GetStyle().ItemSpacing.y * 2.0f;
		RenderManager::revert_font();
		const float child_window_height = ImGui::GetContentRegionAvail().y - button_height;

		ImGui::BeginChild("AbilityLeft", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, child_window_height), true);

		ImGui::TextUnformatted(translate_c("$COLUMN_NAME"));
		ImGui::InputText("##ability_name", name_buf.data(), name_buf.size());

		ImGui::TextUnformatted(translate_c("$COLUMN_ICON"));
		const ImVec2 iconpos = ImGui::GetCursorScreenPos();
		const float ic_size = std::round(60.0f * scale_factor);
		ImGui::Dummy(ImVec2(ic_size, ic_size));
		RenderManager::draw_art_icon_fields_in_editor(draft_icon_form, draft_icon, iconpos, static_cast<int>(ic_size),
			IM_COL32_WHITE);

		ImGui::TextUnformatted(translate_c("$ART_CLASS"));
		if (ImGui::BeginCombo("##art_class", art_class_label(draft_class))) {
			for (int i = 0; i < static_cast<int>(class_values.size()); ++i) {
				const ArtClass value = class_values[static_cast<std::size_t>(i)];
				const bool selected = draft_class == value;
				if (ImGui::Selectable(art_class_label(value), selected)) {
					draft_class = value;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::InputFloat(translate_c("$STAMINA_COST"), &draft_stamina, 1.0f, 5.0f, "%.0f");
		ImGui::InputFloat(translate_c("$MAGICKA_COST"), &draft_magicka, 1.0f, 5.0f, "%.0f");
		ImGui::InputFloat(translate_c("$HEALTH_COST"), &draft_health, 1.0f, 5.0f, "%.0f");
		ImGui::InputText(translate_c("$COOLDOWN"), cooldown_buf.data(), cooldown_buf.size());
		ImGui::InputFloat(translate_c("$GLOBAL_COOLDOWN"), &draft_gcd, 0.1f, 0.5f, "%.1f");

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("AbilityRight", ImVec2(0.0f, child_window_height), true, ImGuiWindowFlags_HorizontalScrollbar);
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
					const std::string button_label = "##abicon" + name + std::to_string(n);
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

		RenderManager::set_large_font();
		if (ImGui::Button(translate_id("$SAVE").c_str())) {
			apply_to_art(*art);
			persist_ability(*art);
			close();
		}
		ImGui::SameLine();
		if (ImGui::Button(translate_id("$RESET").c_str())) {
			if (is_custom_ability(art->id)) {
				ArtDefinition fresh = custom_art_from_folder(static_cast<int>(art->id - custom_art_id_base),
					"", "", "", art->has_clip);
				fresh.folder_path = art->folder_path;
				fresh.has_clip = art->has_clip;
				load_from_art(fresh);
			} else if (const ArtDefinition* catalogue = GameData::get_art_catalogue(art->id)) {
				load_from_art(*catalogue);
			}
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

}  // namespace SpellHotbar::AbilityEditor
