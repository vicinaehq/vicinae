#include <catch2/catch_test_macros.hpp>
#include "services/screenshots/screenshot.hpp"

TEST_CASE("Latest screenshot skips newer recordings and handles a recordings-only library") {
  ScreenshotResult result;
  REQUIRE_FALSE(result.latestImage());
  result.items = {{.path = "/tmp/newer.mov", .kind = Screenshot::Kind::Recording},
                  {.path = "/tmp/older.png"}};
  REQUIRE(result.latestImage());
  REQUIRE(result.latestImage()->path == "/tmp/older.png");
  result.items.pop_back();
  REQUIRE_FALSE(result.latestImage());
}
