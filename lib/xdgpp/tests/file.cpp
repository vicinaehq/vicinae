#include <xdgpp/xdgpp.hpp>
#include <catch2/catch_test_macros.hpp>

constexpr const char *GROUP = XDGPP_GROUP;

static const fs::path FIXTURES = XDGPP_FIXTURE_DIR;

TEST_CASE("desktop file id should default to its filename", GROUP) {
  auto file = xdgpp::DesktopFile::fromFile(FIXTURES / "firefox-bin.desktop", {});

  REQUIRE(file.isValid());
  REQUIRE(file.id() == "firefox-bin.desktop");
}

TEST_CASE("desktop file should save the correct path", GROUP) {
  auto file = xdgpp::DesktopFile::fromFile(FIXTURES / "firefox-bin.desktop", {});

  REQUIRE(file.isValid());
  REQUIRE(file.path() == FIXTURES / "firefox-bin.desktop");
}

TEST_CASE("should compute id relative to data directory", GROUP) {
  {
    auto file = xdgpp::DesktopFile::fromFile(FIXTURES / "firefox-bin.desktop", FIXTURES);

    REQUIRE(file.isValid());
    REQUIRE(file.id() == "firefox-bin.desktop");
  }

  {
    auto id = xdgpp::DesktopFile::relativeId("./assets/nested/nested2/firefox-bin.desktop", "./assets");

    REQUIRE(id == "nested.nested2.firefox-bin.desktop");
  }
}

TEST_CASE("fromId should return valid desktop file if found", GROUP) {
  {
    auto file = xdgpp::DesktopFile::fromId("firefox-bin", {FIXTURES});
    REQUIRE(file.has_value());
  }

  {
    auto file = xdgpp::DesktopFile::fromId("firefox-bin.desktop", {FIXTURES});
    REQUIRE(file.has_value());
  }
}
