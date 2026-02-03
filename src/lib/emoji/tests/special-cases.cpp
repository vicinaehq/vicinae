#include <catch2/catch_test_macros.hpp>
#include "emoji/emoji.hpp"

TEST_CASE("ZWJ sequence family emoji") { REQUIRE(emoji::isUtf8EncodedEmoji("👨‍👩‍👧‍👦")); }

TEST_CASE("ZWJ sequence person with computer") { REQUIRE(emoji::isUtf8EncodedEmoji("👨‍💻")); }

TEST_CASE("skin tone variation") {
  REQUIRE(emoji::isUtf8EncodedEmoji("👋🏽"));
  REQUIRE(emoji::isUtf8EncodedEmoji("👍🏿"));
}

TEST_CASE("flag sequence") {
  REQUIRE(emoji::isUtf8EncodedEmoji("🇺🇸"));
  REQUIRE(emoji::isUtf8EncodedEmoji("🇫🇷"));
}

TEST_CASE("keycap sequence") {
  REQUIRE(emoji::isUtf8EncodedEmoji("1️⃣"));
  REQUIRE(emoji::isUtf8EncodedEmoji("#️⃣"));
}

TEST_CASE("emoji with variation selector") {
  REQUIRE(emoji::isUtf8EncodedEmoji("❤️"));
  REQUIRE(emoji::isUtf8EncodedEmoji("☺️"));
}

TEST_CASE("plain text is not emoji") {
  REQUIRE_FALSE(emoji::isUtf8EncodedEmoji("hello"));
  REQUIRE_FALSE(emoji::isUtf8EncodedEmoji("123"));
}

TEST_CASE("text with emoji mixed") { REQUIRE_FALSE(emoji::isUtf8EncodedEmoji("hello 😀")); }
