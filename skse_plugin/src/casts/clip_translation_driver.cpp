#include "clip_translation_driver.h"
#include "clip_translation.h"
#include "art_driver.h"
#include "msco_cast_driver.h"
#include "combo_cache.h"
#include "../logger/logger.h"
#include "../game_data/game_data.h"
#include "../game_data/custom_ability_runtime.h"

#include <algorithm>
#include <mutex>
#include <string_view>
#include <vector>

using namespace std::literals;

namespace SpellHotbar::casts::ClipTranslationDriver {

	namespace {
		std::mutex mutex;
		RE::hkbClipGenerator* bound_clip = nullptr;
		std::vector<ClipTranslationKey> keys;
		ClipTranslation last_applied{};
		bool primed = false;

		void Activate(RE::hkbClipGenerator* a_this, const RE::hkbContext& a_context);
		void Deactivate(RE::hkbClipGenerator* a_this, const RE::hkbContext& a_context);
		REL::Relocation<decltype(Activate)> _Activate;
		REL::Relocation<decltype(Deactivate)> _Deactivate;

		[[nodiscard]] bool is_shtb_clip(std::string_view name) noexcept
		{
			return name == "SH2_Art_Clip"sv || name == "SH2_CastRight_Clip"sv ||
				   name == "SH2_Cast2_Clip"sv || name == "SH2_Cast3_Clip"sv ||
				   name == "SH2_Cast4_Clip"sv;
		}

		[[nodiscard]] RE::hkaAnimation* bound_animation(RE::hkbClipGenerator* clip)
		{
			if (!clip || !clip->binding || !clip->binding->animation) {
				return nullptr;
			}
			return clip->binding->animation.get();
		}

		std::vector<ClipTranslationKey> parse_keys(const RE::hkaAnimation* animation)
		{
			std::vector<ClipTranslationKey> out;
			if (!animation) {
				return out;
			}
			for (const auto& track : animation->annotationTracks) {
				bool found = false;
				for (std::int32_t i = 0; i < track.annotations.size(); ++i) {
					const auto& annotation = track.annotations[i];
					const char* text = annotation.text.c_str();
					if (!text) {
						continue;
					}
					const auto parsed = parse_animmotion_text(text);
					if (!parsed) {
						continue;
					}
					out.push_back(ClipTranslationKey{
						.time = annotation.time,
						.x = parsed->x,
						.y = parsed->y,
						.z = parsed->z,
					});
					found = true;
				}
				if (found) {
					break;
				}
			}
			std::sort(out.begin(), out.end(),
				[](const ClipTranslationKey& a, const ClipTranslationKey& b) { return a.time < b.time; });
			return out;
		}

		void bind_clip(RE::hkbClipGenerator* clip)
		{
			const char* raw = clip && clip->name.c_str() ? clip->name.c_str() : "";
			if (!is_shtb_clip(raw)) {
				return;
			}
			auto* animation = bound_animation(clip);
			auto parsed = parse_keys(animation);
			if (std::string_view{ raw } == "SH2_Art_Clip"sv) {
				bool has_win_open{ false };
				bool has_hit_frame{ false };
				if (animation) {
					for (const auto& track : animation->annotationTracks) {
						for (std::int32_t i = 0; i < track.annotations.size(); ++i) {
							const char* text = track.annotations[i].text.c_str();
							if (!text) {
								continue;
							}
							const std::string_view tag{ text };
							if (is_ability_win_open_event(tag)) {
								has_win_open = true;
							}
							if (is_ability_hit_frame_event(tag)) {
								has_hit_frame = true;
							}
						}
					}
					if (const int selector = GameData::get_art_selector(); selector > 0) {
						const auto art_id = static_cast<std::uint32_t>(selector);
						if (const ArtDefinition* art = GameData::get_art(art_id)) {
							inject_custom_ability_pie(animation, *art);
						}
					}
				}
				ArtDriver::bind_latch(has_win_open, has_hit_frame);
			}
			std::lock_guard lock{ mutex };
			bound_clip = clip;
			keys = std::move(parsed);
			last_applied = {};
			primed = false;
			if (keys.empty()) {
				logger::warn("SH2 motion: {} activated with no animmotion keys", raw);
			} else {
				logger::info("SH2 motion: bound {} ({} animmotion keys)", raw, keys.size());
			}
		}

		void unbind_clip(RE::hkbClipGenerator* clip)
		{
			std::lock_guard lock{ mutex };
			if (bound_clip == clip) {
				bound_clip = nullptr;
				keys.clear();
				last_applied = {};
				primed = false;
			}
		}

		void Activate(RE::hkbClipGenerator* a_this, const RE::hkbContext& a_context)
		{
			_Activate(a_this, a_context);
			bind_clip(a_this);
		}

		void Deactivate(RE::hkbClipGenerator* a_this, const RE::hkbContext& a_context)
		{
			unbind_clip(a_this);
			_Deactivate(a_this, a_context);
		}
	}

	void install()
	{
		REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_hkbClipGenerator[0] };
		_Activate = vtbl.write_vfunc(0x4, Activate);
		_Deactivate = vtbl.write_vfunc(0x7, Deactivate);
		logger::info("SH2 motion: clip translation hooks installed");
	}

	void apply(RE::PlayerCharacter* pc)
	{
		if (!pc) {
			return;
		}
		if (!ArtDriver::is_active() && !MscoCastDriver::is_active()) {
			return;
		}

		ClipTranslation worldDelta{};
		{
			std::lock_guard lock{ mutex };
			if (!bound_clip || keys.empty()) {
				return;
			}
			const auto cumulative = interpolate_clip_translation(keys, bound_clip->localTime);
			if (!primed) {
				last_applied = cumulative;
				primed = true;
				return;
			}
			const auto localDelta = translation_delta(last_applied, cumulative);
			last_applied = cumulative;
			if (localDelta.x == 0.0f && localDelta.y == 0.0f) {
				return;
			}
			worldDelta = local_to_world(localDelta, pc->GetAngleZ());
		}

		auto pos = pc->GetPosition();
		pos.x += worldDelta.x;
		pos.y += worldDelta.y;
		pc->SetPosition(pos, true);
	}

	void reset()
	{
		std::lock_guard lock{ mutex };
		bound_clip = nullptr;
		keys.clear();
		last_applied = {};
		primed = false;
	}
}
