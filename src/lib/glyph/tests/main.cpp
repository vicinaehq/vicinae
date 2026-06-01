#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include "glyph/emoji.hpp"
#include "glyph/glyph.hpp"

TEST_CASE("find smile emoji") {
  const auto *info = glyph::lookup("😄");

  REQUIRE(info);
}

TEST_CASE("make sure emojis are skin tone tagged") {
  const auto *info = glyph::lookup("✋");

  REQUIRE(info);
  REQUIRE(info->skinnable);
}

TEST_CASE("car emoji should be identified has emoji") { REQUIRE(emoji::isUtf8EncodedEmoji("🚗")); }

TEST_CASE("skin toned emoji should identify as emoji") { REQUIRE(emoji::isUtf8EncodedEmoji("👌🏻")); }

TEST_CASE("all emojis in database pass isEmoji test") {
  for (const auto &item : glyph::items()) {
    if (item.kind != glyph::Kind::Emoji) continue;
    const bool ok = emoji::isUtf8EncodedEmoji(item.character);
    if (!ok) { std::cout << item.character << std::endl; }
    REQUIRE(ok);
  }
}
