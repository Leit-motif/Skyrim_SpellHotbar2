#pragma once
#include "../bar/hotbar.h"
#include "../input/input.h"

/**
 * Consumer side of ShoutMCO's optional cast-intent driver API (ADR-0005).
 *
 * ShoutMCO owns release timing: whether a hotbar press may start now, or must wait for a
 * confirmed legal graph state, and when that state arrives. Spell Hotbar 2 owns the payload —
 * what the slot means and whether it can still cast when the release comes. Nothing here
 * observes MCO animation events, gates on `HitFrame`, polls, or runs a timer; all of that
 * lives in ShoutMCO.
 *
 * The whole module fails open. With no ShoutMCO, no export, or no shared major version, every
 * call is a no-op and Direct Cast behaves exactly as it did before this existed.
 *
 * Everything here runs on the main thread: the input hook, the papyrus `castSlot` task, and
 * ShoutMCO's callback all do. No locking, no atomics.
 */
namespace SpellHotbar::casts::CastIntent {

	/**
	 * The read-only compatibility status, reported once and never as a popup.
	 */
	enum class Status {
		unavailable,  // no ShoutMCO, or it does not export the API
		active,       // negotiated and taking intents
		incompatible  // present, but implements no major version we know
	};

	/**
	 * Resolve ShoutMCO's export and negotiate the API version. Call once, after every SKSE
	 * plugin DLL is loaded (`kPostLoad`). Logs the resulting status one time.
	 */
	void negotiate();

	Status get_status();

	/**
	 * The status as the one word the ticket names: `active`, `unavailable`, `incompatible`.
	 */
	const char* status_name();

	/**
	 * Offer the pressed slot as a cast intent, at the seam where the graph refused entry.
	 *
	 * Snapshots the slot's current assignment as the payload and returns true only when
	 * ShoutMCO deferred it, meaning exactly one release or abandon callback follows and the
	 * press must be reported as accepted rather than failed. Any other answer — no API, the
	 * request rejected, or nothing to wait for — returns false, and the caller reports the
	 * refusal it already had.
	 */
	bool offer(size_t slot, const Input::KeyBind& keybind);

	/**
	 * Withdraw a pending intent, if this mod owns one. Safe to call at any time; a stale or
	 * absent intent is not an error. The abandon callback still clears the payload.
	 */
	void cancel();

	/**
	 * Is a payload retained right now, waiting for ShoutMCO to release it?
	 */
	bool has_pending_intent();
}
