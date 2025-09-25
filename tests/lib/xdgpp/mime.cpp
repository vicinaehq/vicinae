#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <xdgpp/xdgpp.hpp>

namespace fs = std::filesystem;

constexpr const char *GROUP = XDGPP_GROUP;
const fs::path FIXTURES = XDGPP_FIXTURE_DIR;

static fs::path homeDir() {
  if (auto home = getenv("HOME")) { return home; }

  return {};
}

TEST_CASE("should parse added associations", GROUP) {
  auto list = xdgpp::MimeAppsList::fromData(R"(
[Added Associations]
x-scheme-handler/http=firefox-esr.desktop;
	)");

  auto associations = list.addedAssociations("x-scheme-handler/http");

  REQUIRE(associations.size() == 1);
  REQUIRE(associations.at(0) == "firefox-esr.desktop");
}

TEST_CASE("should parse list of added associations", GROUP) {
  auto list = xdgpp::MimeAppsList::fromData(R"(
[Added Associations]
x-scheme-handler/http=firefox-esr.desktop;chromium.desktop;
	)");

  auto associations = list.addedAssociations("x-scheme-handler/http");

  REQUIRE(associations.size() == 2);
  REQUIRE(associations.at(0) == "firefox-esr.desktop");
  REQUIRE(associations.at(1) == "chromium.desktop");
}

TEST_CASE("should parse removed associations", GROUP) {
  auto list = xdgpp::MimeAppsList::fromData(R"(
[Removed Associations]
x-scheme-handler/http=firefox-esr.desktop;
	)");

  auto associations = list.removedAssociations("x-scheme-handler/http");

  REQUIRE(associations.size() == 1);
  REQUIRE(associations.at(0) == "firefox-esr.desktop");
}

TEST_CASE("should parse list of removed associations", GROUP) {
  auto list = xdgpp::MimeAppsList::fromData(R"(
[Removed Associations]
x-scheme-handler/http=firefox-esr.desktop;chromium.desktop;
	)");

  auto associations = list.removedAssociations("x-scheme-handler/http");

  REQUIRE(associations.size() == 2);
  REQUIRE(associations.at(0) == "firefox-esr.desktop");
  REQUIRE(associations.at(1) == "chromium.desktop");
}

TEST_CASE("should parse default applications", GROUP) {
  auto list = xdgpp::MimeAppsList::fromData(R"(
[Default Applications]
x-scheme-handler/http=firefox-esr.desktop;
	)");

  auto associations = list.defaultAssociations("x-scheme-handler/http");

  REQUIRE(associations.size() == 1);
  REQUIRE(associations.at(0) == "firefox-esr.desktop");
}

TEST_CASE("should parse list of default applications", GROUP) {
  auto list = xdgpp::MimeAppsList::fromData(R"(
[Default Applications]
x-scheme-handler/http=firefox-esr.desktop;chromium.desktop;
	)");

  auto associations = list.defaultAssociations("x-scheme-handler/http");

  REQUIRE(associations.size() == 2);
  REQUIRE(associations.at(0) == "firefox-esr.desktop");
  REQUIRE(associations.at(1) == "chromium.desktop");
}

TEST_CASE("parse mimeapps.list from file", GROUP) {
  auto file = xdgpp::MimeAppsListFile::fromFile(FIXTURES / "mimeapps.list");
  auto jpegAssociations = file.addedAssociations("image/jpeg");

  REQUIRE(file.path() == FIXTURES / "mimeapps.list");
  REQUIRE(jpegAssociations.size() > 0);
  REQUIRE(jpegAssociations.at(0) == "swayimg.desktop");
}
