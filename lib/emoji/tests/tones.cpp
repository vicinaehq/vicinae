#include "catch2/catch_test_macros.hpp"
#include "emoji/emoji.hpp"

TEST_CASE("has 5 skin tones") { REQUIRE(emoji::skinTones().size() == 5); }

TEST_CASE("can apply light skin tone") {
  // TODO: do for all tones

  REQUIRE(emoji::applySkinTone("👌", emoji::SkinTone::Light) == "👌🏻");
}
