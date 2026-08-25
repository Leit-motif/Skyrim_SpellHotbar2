#pragma once
#include "../bar/hotbar.h"
#include "../input/input.h"

/**
 * One last-wins Cast Intent (ADR-0005, Ability queue ticket 10).
 *
 * Spell Hotbar 2 owns the payload — slot, type, and FormID or Ability id — and
 * revalidates it once on fire. Two clocks decide the legal frame:
 *
 * - ShoutMCO, when the player is in someone else's MCO swing or a real shout.
 * - This mod's latch, when the player is in a Driver Cast or Ability we started.
 *
 * ShoutMCO is optional. Mid-swing with no API fails open (dead press). SH2-owned
 * latches still buffer.
 *
 * Everything here runs on the main thread: the input hook, the papyrus `castSlot`
 * task, ShoutMCO's callback, and the game-loop poll. No locking, no atomics.
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
	 * Offer the pressed slot as a Cast Intent.
	 *
	 * Snapshots the slot's current assignment. Returns true when the press is retained —
	 * either locally until this mod's latch opens, or by ShoutMCO until its release
	 * callback. The caller reports that as accepted rather than failed. Any other
	 * answer — no API for a ShoutMCO clock, the request rejected, or nothing to wait
	 * for — returns false, and the caller reports the refusal it already had.
	 */
	bool offer(size_t slot, const Input::KeyBind& keybind);

	/**
	 * True while a retained payload is being fired. Start paths must not offer
	 * again, or a still-true IsShouting bit would drop a hotbar shout on release.
	 */
	bool is_firing();

	/**
	 * True while this mod retains a payload that has not yet fired. A hotbar shout
	 * must not inject a `"Shout"` ButtonEvent until that payload is released.
	 */
	bool is_pending();

	/**
	 * True when our Driver Cast or Ability is live and its latch is still closed.
	 */
	bool should_retain_now();

	/**
	 * If a locally retained payload's latch is now open (or our shtb state has ended),
	 * fire it once. No-op for a ShoutMCO-owned handle.
	 */
	void poll_local_release();

	/**
	 * Withdraw a pending intent, if this mod owns one, and drop the payload immediately. Safe to
	 * call at any time; a stale or absent intent is not an error.
	 */
	void cancel();
}
