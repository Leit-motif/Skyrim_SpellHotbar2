#pragma once

namespace RE
{
	class Actor;
	class hkbClipGenerator;
	class PlayerCharacter;
}

namespace SpellHotbar::casts::ClipTranslationDriver {

	void install();

	// Called from the main loop while an shtb state is live. Applies one frame of
	// clip animmotion as actor translation. No-op when no bound clip has keys.
	void apply(RE::PlayerCharacter* pc);

	// Drop the bound clip (save load / new game / shtb exit).
	void reset();
}
