#include "hotbar_serialization.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

using SpellHotbar::BarSerialization::SlotRecord;
using SpellHotbar::BarSerialization::read_slot_record;
using SpellHotbar::BarSerialization::slot_is_in_range;

namespace {

int g_failures = 0;

void expect(bool condition, const char* message)
{
	if (!condition) {
		std::cerr << "FAIL: " << message << '\n';
		++g_failures;
	}
}

class ByteReader {
public:
	explicit ByteReader(const std::array<std::uint8_t, 14>& bytes) : bytes_(bytes) {}

	bool read(void* destination, std::size_t size)
	{
		if (offset_ + size > bytes_.size()) {
			return false;
		}
		std::memcpy(destination, bytes_.data() + offset_, size);
		offset_ += size;
		return true;
	}

	[[nodiscard]] std::size_t offset() const { return offset_; }

private:
	const std::array<std::uint8_t, 14>& bytes_;
	std::size_t offset_{ 0 };
};

void an_out_of_range_slot_is_rejected_without_desynchronizing_the_next_record()
{
	// Version 6 record layout: slot, kind, 32-bit payload, hand.
	const std::array<std::uint8_t, 14> bytes{
		12, 1, 0x44, 0x33, 0x22, 0x11, 3,
		11, 0, 0xDD, 0xCC, 0xBB, 0xAA, 2,
	};
	ByteReader reader(bytes);
	auto read = [&reader](void* destination, std::size_t size) {
		return reader.read(destination, size);
	};

	SlotRecord invalid;
	expect(read_slot_record(read, 6, invalid), "the malformed record is still fully readable");
	expect(!slot_is_in_range(invalid, 12), "serialized slot 12 is outside a 12-slot bar");
	expect(reader.offset() == 7, "rejecting a slot does not leave its payload or hand unread");

	SlotRecord valid;
	expect(read_slot_record(read, 6, valid), "the record after an invalid slot remains aligned");
	expect(slot_is_in_range(valid, 12), "serialized slot 11 is the final valid slot");
	expect(valid.slot == 11, "the next record keeps its slot index");
	expect(valid.payload == 0xAABBCCDD, "the next record keeps its payload");
	expect(valid.hand == 2, "the next record keeps its hand");
}

}  // namespace

int main()
{
	an_out_of_range_slot_is_rejected_without_desynchronizing_the_next_record();

	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
