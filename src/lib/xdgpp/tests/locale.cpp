#include "xdgpp/locale/locale.hpp"
#include <xdgpp/xdgpp.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("should parse LANG") { REQUIRE(xdgpp::Locale::parse("en").toString() == "en"); }

TEST_CASE("should parse LANG_COUNTRY") { REQUIRE(xdgpp::Locale::parse("en_US").toString() == "en_US"); }

TEST_CASE("should parse LANG_COUNTRY.ENCODING") {
  REQUIRE(xdgpp::Locale::parse("en_US.utf8").toString() == "en_US.utf8");
}

TEST_CASE("should parse LANG_COUNTRY.ENCODING@MODIFIER") {
  REQUIRE(xdgpp::Locale::parse("en_US.utf8@latin").toString() == "en_US.utf8@latin");
}

TEST_CASE("should parse LANG.ENCODING") { REQUIRE(xdgpp::Locale::parse("en.utf8").toString() == "en.utf8"); }

TEST_CASE("should parse LANG@MODIFIER") {
  REQUIRE(xdgpp::Locale::parse("en@latin").toString() == "en@latin");
}

TEST_CASE("lang_COUNTRY.ENCODING == should match lang_COUNTRY") {
  REQUIRE(xdgpp::Locale("en_US.utf8")
              .matchesOnly(xdgpp::Locale("en_US"), xdgpp::Locale::COUNTRY | xdgpp::Locale::LANG));
}
