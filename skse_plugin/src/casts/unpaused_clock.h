#pragma once

#include <atomic>

namespace SpellHotbar::casts::UnpausedClock {

	/**
	 * Milliseconds of gameplay, not of wall time. Advanced once per unpaused frame from
	 * `update_cast`, which is itself gated on `!GameIsPaused()`, so menu time never accrues.
	 *
	 * TICKET 37. Every other timer in this subsystem already works this way -- `advance_time`,
	 * the art's `m_cast_timer`, the GCDs all accumulate `delta` -- so they are pause-correct by
	 * construction. The two deadlines ticket 36 added were the only wall-clock ones left, and a
	 * wall clock kept counting through an inventory visit: a healthy cast paused mid-clip was cut
	 * on unpause, and the log gained a `state watchdog expired` line with nothing wedged. A cap
	 * that manufactures false wedge reports poisons the next diagnosis, which is the worse cost.
	 *
	 * Written on the main thread and read from the animation thread (`observe_graph_event`), so
	 * the counter is atomic. Relaxed is enough: nothing is published through it, and a reader one
	 * frame behind measures an age off by one frame.
	 */
	// A stamp of exactly zero is the "nothing recorded" sentinel in `cast_state_watchdog_expired`
	// and in `channel_started_ms`. Starting above zero means the first frame of a session can
	// never mint a stamp that reads as absent.
	inline constexpr double kOrigin = 1.0;

	inline std::atomic<double> g_elapsed_ms{ kOrigin };

	inline void advance(float delta_seconds) noexcept
	{
		g_elapsed_ms.store(g_elapsed_ms.load(std::memory_order_relaxed) +
							   static_cast<double>(delta_seconds) * 1000.0,
			std::memory_order_relaxed);
	}

	[[nodiscard]] inline double now_ms() noexcept
	{
		return g_elapsed_ms.load(std::memory_order_relaxed);
	}
}
