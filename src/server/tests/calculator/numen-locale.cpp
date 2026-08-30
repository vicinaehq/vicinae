#include "services/calculator-service/numen/numen-locale.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("does not forward a Qt locale name that std::locale rejects") {
  constexpr auto qtName = "en_US";
  if (calculator::isValidStdLocaleName(qtName)) { SKIP("en_US is a valid std::locale name"); }

  const auto resolved = calculator::numenLocaleName(qtName);
  REQUIRE(resolved != qtName);
  if (resolved) { REQUIRE(calculator::isValidStdLocaleName(*resolved)); }
}

TEST_CASE("prefers the UTF-8 variant of a Qt locale name") {
  if (!calculator::isValidStdLocaleName("en_US.UTF-8")) { SKIP("en_US.UTF-8 is not installed"); }

  REQUIRE(calculator::numenLocaleName("en_US") == "en_US.UTF-8");
}

TEST_CASE("C locale always resolves") {
  const auto resolved = calculator::numenLocaleName("C");
  REQUIRE(resolved);
  REQUIRE(calculator::isValidStdLocaleName(*resolved));
}

TEST_CASE("unknown locale names fall back instead of throwing") {
  REQUIRE_FALSE(calculator::numenLocaleName("definitely_not_a_locale_zz").has_value());
}
