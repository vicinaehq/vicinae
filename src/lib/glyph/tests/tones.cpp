#include "catch2/catch_test_macros.hpp"
#include "glyph/emoji.hpp"

TEST_CASE("has 6 skin tones") { REQUIRE(emoji::skinTones().size() == 6); }

TEST_CASE("apply all skin tones") {
  REQUIRE(emoji::applySkinTone("👋", emoji::SkinTone::Light) == "👋🏻");
  REQUIRE(emoji::applySkinTone("👋", emoji::SkinTone::MediumLight) == "👋🏼");
  REQUIRE(emoji::applySkinTone("👋", emoji::SkinTone::Medium) == "👋🏽");
  REQUIRE(emoji::applySkinTone("👋", emoji::SkinTone::MediumDark) == "👋🏾");
  REQUIRE(emoji::applySkinTone("👋", emoji::SkinTone::Dark) == "👋🏿");
}

TEST_CASE("apply skin tone strips VS16") {
  REQUIRE(emoji::applySkinTone("🖐️", emoji::SkinTone::Dark) == "🖐🏿");
  REQUIRE(emoji::applySkinTone("🖐️", emoji::SkinTone::Medium) == "🖐🏽");
}

TEST_CASE("apply skin tone to ZWJ sequence strips VS16") {
  REQUIRE(emoji::applySkinTone("💆‍♂️", emoji::SkinTone::Dark) == "💆🏿‍♂");
  REQUIRE(emoji::applySkinTone("💆‍♂️", emoji::SkinTone::Light) == "💆🏻‍♂");
}
