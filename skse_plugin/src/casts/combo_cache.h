#pragma once

#include <mutex>
#include <optional>

namespace SpellHotbar::casts {

struct McoCombo {
	int nextAttack = 1;
	int nextPowerAttack = 1;
};

// Last MCO combo position taken at an attack-time event. The value is preserved, never
// derived: a read of 3 is written back as 3. Age is the discriminator that a live read
// at cast start cannot be, because the ready-state payload has already stomped the
// variable to 1 by then.
//
// Capture (animation thread) and arm (main thread, on cancel/finish) share this object,
// so every mutation takes the mutex. consume() is one-shot: the first ready pass after
// a Driver Cast writes the sampled index, and a later ready/PIE cannot replay it.
class RollingMcoCombo {
public:
	static constexpr double kMaxAgeMs = 5000.0;

	void record(McoCombo sample, double nowMs)
	{
		std::lock_guard lock{ mutex_ };
		valid_ = true;
		takenAtMs_ = nowMs;
		sample_ = sample;
		pending_ = false;
	}

	[[nodiscard]] std::optional<McoCombo> usable(double nowMs) const
	{
		std::lock_guard lock{ mutex_ };
		return usable_locked(nowMs);
	}

	std::optional<McoCombo> arm(double nowMs)
	{
		std::lock_guard lock{ mutex_ };
		const auto combo = usable_locked(nowMs);
		if (!combo) {
			pending_ = false;
			return std::nullopt;
		}
		restore_ = *combo;
		pending_ = true;
		return restore_;
	}

	[[nodiscard]] std::optional<McoCombo> peek() const
	{
		std::lock_guard lock{ mutex_ };
		if (!pending_) {
			return std::nullopt;
		}
		return restore_;
	}

	[[nodiscard]] std::optional<McoCombo> consume()
	{
		std::lock_guard lock{ mutex_ };
		if (!pending_) {
			return std::nullopt;
		}
		pending_ = false;
		return restore_;
	}

	void disarm()
	{
		std::lock_guard lock{ mutex_ };
		pending_ = false;
	}

	void clear()
	{
		std::lock_guard lock{ mutex_ };
		valid_ = false;
		pending_ = false;
	}

private:
	[[nodiscard]] std::optional<McoCombo> usable_locked(double nowMs) const
	{
		if (!valid_) {
			return std::nullopt;
		}
		if (nowMs - takenAtMs_ > kMaxAgeMs) {
			return std::nullopt;
		}
		return sample_;
	}

	mutable std::mutex mutex_;
	bool valid_ = false;
	bool pending_ = false;
	double takenAtMs_ = 0.0;
	McoCombo sample_{};
	McoCombo restore_{};
};

// SH2's own cast combo. Independent of MCO_nextattack: advances on a cast, wraps at the
// clip-set length, and is not reset by an attack.
class CastComboIndex {
public:
	static constexpr int kLength = 4;

	[[nodiscard]] int current() const { return index_; }

	void advance() { index_ = index_ % kLength + 1; }

private:
	int index_ = 1;
};

}  // namespace SpellHotbar::casts
