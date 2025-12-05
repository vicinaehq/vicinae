#include <algorithm>
#include <xdgpp/xdgpp.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>

namespace fs = std::filesystem;

static fs::path homeDir() {
  if (auto home = getenv("HOME")) { return home; }

  return {};
}

TEST_CASE("should have non empty dataDirs", XDGPP_GROUP) { REQUIRE_FALSE(xdgpp::dataDirs().empty()); }

TEST_CASE("should have non empty configDirs", XDGPP_GROUP) { REQUIRE_FALSE(xdgpp::configDirs().empty()); }

TEST_CASE("XDG_DATA_DIRS should provide spec-compliant defaults", XDGPP_GROUP) {
  unsetenv("XDG_DATA_DIRS");
  auto dirs = xdgpp::dataDirs();

  REQUIRE(std::ranges::contains(dirs, fs::path("/usr/local/share")));
  REQUIRE(std::ranges::contains(dirs, fs::path("/usr/share")));
}

TEST_CASE("XDG_CONFIG_DIRS should provide spec-compliant defaults", XDGPP_GROUP) {
  unsetenv("XDG_CONFIG_DIRS");
  auto dirs = xdgpp::configDirs();

  REQUIRE(std::ranges::contains(dirs, fs::path("/etc/xdg")));
}

TEST_CASE("XDG_CONFIG_HOME should provide a spec-compliant default", XDGPP_GROUP) {
  unsetenv("XDG_CONFIG_HOME");
  REQUIRE(xdgpp::configHome() == homeDir() / ".config");
}

TEST_CASE("XDG_DATA_HOME should provide a spec-compliant default", XDGPP_GROUP) {
  unsetenv("XDG_DATA_HOME");
  REQUIRE(xdgpp::dataHome() == homeDir() / ".local" / "share");
}

TEST_CASE("should return nullopt if no XDG_RUNTIME_DIR is set", XDGPP_GROUP) {
  unsetenv("XDG_RUNTIME_DIR");
  REQUIRE_FALSE(xdgpp::runtimeDir().has_value());
}
