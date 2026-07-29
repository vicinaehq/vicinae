#include <catch2/catch_test_macros.hpp>
#include "src/emoji-properties.hpp"

using namespace emoji;

TEST_CASE("lookupProperties: plain text has no emoji properties") {
  REQUIRE(lookupProperties(U'A') == 0);
  REQUIRE(lookupProperties(U'z') == 0);
  REQUIRE(lookupProperties(U' ') == 0);
  REQUIRE(lookupProperties(0) == 0);
  REQUIRE(lookupProperties(U'é') == 0);
  REQUIRE(lookupProperties(0x4E00) == 0);
}

TEST_CASE("lookupProperties: digits and # are Emoji but not Emoji_Presentation") {
  for (char32_t cp : {U'0', U'5', U'9', U'#', U'*'}) {
    REQUIRE((lookupProperties(cp) & Emoji));
    REQUIRE_FALSE((lookupProperties(cp) & EmojiPresentation));
  }
}

TEST_CASE("lookupProperties: presentation emoji") {
  for (char32_t cp : {U'😀', U'🚗', U'🍎'}) {
    std::uint8_t const p = lookupProperties(cp);
    REQUIRE((p & Emoji));
    REQUIRE((p & EmojiPresentation));
  }
}

TEST_CASE("lookupProperties: skinnable emoji carry Emoji_Modifier_Base") {
  for (char32_t cp : {U'👋', U'✊', U'☝'}) {
    std::uint8_t const p = lookupProperties(cp);
    REQUIRE((p & Emoji));
    REQUIRE((p & EmojiModifierBase));
    REQUIRE_FALSE((p & EmojiModifier));
  }
}

TEST_CASE("lookupProperties: skin tone modifiers are Emoji_Modifier") {
  for (char32_t cp = 0x1F3FB; cp <= 0x1F3FF; ++cp) {
    std::uint8_t const p = lookupProperties(cp);
    REQUIRE((p & EmojiModifier));
    REQUIRE_FALSE((p & EmojiModifierBase));
  }
}

TEST_CASE("lookupProperties: binary search boundaries") {
  REQUIRE(lookupProperties(0x22) == 0);
  REQUIRE((lookupProperties(0x23) & Emoji));
  REQUIRE(lookupProperties(0x24) == 0);
  REQUIRE(lookupProperties(0x10FFFF) == 0);
  REQUIRE(lookupProperties(0x110000) == 0);
}
