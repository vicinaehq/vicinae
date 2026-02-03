#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include "emoji/emoji.hpp"

TEST_CASE("find smile emoji") {
  const auto info = emoji::findStaticEmoji("😄");

  REQUIRE(info);
}

TEST_CASE("make sure emojis are skin tone tagged") {
  const auto info = emoji::findStaticEmoji("✋");

  REQUIRE(info);
  REQUIRE(info->skinToneSupport);
}

TEST_CASE("car emoji should be identified has emoji") { REQUIRE(emoji::isUtf8EncodedEmoji("🚗")); }

TEST_CASE("skin toned emoji should identify as emoji") { REQUIRE(emoji::isUtf8EncodedEmoji("👌🏻")); }

TEST_CASE("all emojis in database pass isEmoji test") {
  const auto &emojis = emoji::emojis();
  for (const auto &emojiData : emojis) {
    bool ok = emoji::isUtf8EncodedEmoji(emojiData.emoji);
    if (!ok) { std::cout << emojiData.emoji << std::endl; }
    REQUIRE(ok);
  }
}
