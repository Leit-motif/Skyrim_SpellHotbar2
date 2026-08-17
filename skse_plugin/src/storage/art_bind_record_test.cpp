#include "art_bind_record.h"

#include <cstdlib>
#include <iostream>

using SpellHotbar::Storage::ArtBind;
using SpellHotbar::Storage::decode_art_binds;
using SpellHotbar::Storage::encode_art_binds;

namespace {

int g_failures = 0;

void expect(bool cond, const char* msg)
{
	if (!cond) {
		std::cerr << "FAIL: " << msg << '\n';
		++g_failures;
	}
}

void a_weapon_art_bind_survives_encode_decode()
{
	const std::vector<ArtBind> original{ ArtBind{
		.bar_id = '1HSP',
		.slot = 0,
		.modifier = 0,
		.art_id = 1,
	} };
	const auto bytes = encode_art_binds(original);
	std::vector<ArtBind> restored;
	expect(decode_art_binds(bytes, restored), "decode accepts a well-formed WART blob");
	expect(restored.size() == 1, "one bind comes back");
	expect(restored.size() == 1 && restored[0].bar_id == '1HSP', "bar id is the 1h+spell bar");
	expect(restored.size() == 1 && restored[0].slot == 0, "slot 0");
	expect(restored.size() == 1 && restored[0].modifier == 0, "no modifier");
	expect(restored.size() == 1 && restored[0].art_id == 1, "Test Art id 1");
}

void an_empty_list_is_a_valid_blob()
{
	const auto bytes = encode_art_binds({});
	std::vector<ArtBind> restored;
	expect(decode_art_binds(bytes, restored), "empty list decodes");
	expect(restored.empty(), "empty list stays empty");
}

void a_truncated_blob_is_rejected()
{
	std::vector<ArtBind> restored;
	const uint8_t truncated[] = { 1, 0, 0 };
	expect(!decode_art_binds(truncated, restored), "truncated blob is not a bind list");
}

}  // namespace

int main()
{
	a_weapon_art_bind_survives_encode_decode();
	an_empty_list_is_a_valid_blob();
	a_truncated_blob_is_rejected();
	if (g_failures != 0) {
		std::cerr << g_failures << " failure(s)\n";
		return EXIT_FAILURE;
	}
	std::cout << "ok\n";
	return EXIT_SUCCESS;
}
