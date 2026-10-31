#include <catch2/catch_test_macros.hpp>
#include <array>
#include <cstdlib>
#include <fstream>
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
  REQUIRE(!jpegAssociations.empty());
  REQUIRE(jpegAssociations.at(0) == "swayimg.desktop");
}

TEST_CASE("should create mimeapps.list with the default application", GROUP) {
  const auto path = fs::temp_directory_path() / "xdgpp-mimeapps.list";
  fs::remove(path);

  const std::array mimes = {std::string_view("x-scheme-handler/https"), std::string_view("text/html")};

  REQUIRE(xdgpp::setDefaultApplication(mimes, "firefox.desktop", path));
  REQUIRE(slurp(path) == "[Default Applications]\n"
                         "x-scheme-handler/https=firefox.desktop\n"
                         "text/html=firefox.desktop\n"
                         "\n"
                         "[Added Associations]\n"
                         "x-scheme-handler/https=firefox.desktop;\n"
                         "text/html=firefox.desktop;\n");
}

TEST_CASE("should update the default application in an existing mimeapps.list", GROUP) {
  const auto path = fs::temp_directory_path() / "xdgpp-mimeapps.list";
  fs::remove(path);

  {
    std::ofstream ofs{path};
    ofs << "# user config\n"
           "[Default Applications]\n"
           "text/html=chromium.desktop\n"
           "image/png=swayimg.desktop\n"
           "\n"
           "[Added Associations]\n"
           "text/html=chromium.desktop;firefox.desktop;\n"
           "image/png=swayimg.desktop;\n"
           "\n"
           "[Removed Associations]\n"
           "image/png=gimp.desktop;\n";
  }

  const std::array mimes = {std::string_view("text/html"), std::string_view("x-scheme-handler/https")};

  REQUIRE(xdgpp::setDefaultApplication(mimes, "firefox.desktop", path));
  REQUIRE(slurp(path) == "# user config\n"
                         "[Default Applications]\n"
                         "text/html=firefox.desktop\n"
                         "image/png=swayimg.desktop\n"
                         "x-scheme-handler/https=firefox.desktop\n"
                         "\n"
                         "[Added Associations]\n"
                         "text/html=chromium.desktop;firefox.desktop;\n"
                         "image/png=swayimg.desktop;\n"
                         "x-scheme-handler/https=firefox.desktop;\n"
                         "\n"
                         "[Removed Associations]\n"
                         "image/png=gimp.desktop;\n");

  auto list = xdgpp::MimeAppsList::fromFile(path);

  REQUIRE(list.defaultAssociations("text/html") == std::vector<std::string>{"firefox.desktop"});
  REQUIRE(list.addedAssociations("x-scheme-handler/https") == std::vector<std::string>{"firefox.desktop"});
}

TEST_CASE("should drop duplicate keys when setting the default application", GROUP) {
  const auto path = fs::temp_directory_path() / "xdgpp-mimeapps.list";
  fs::remove(path);

  {
    std::ofstream ofs{path};
    ofs << "[Default Applications]\n"
           "text/html=chromium.desktop\n"
           "image/png=swayimg.desktop\n"
           "text/html=helium.desktop\n"
           "\n"
           "[Added Associations]\n"
           "text/html=chromium.desktop;\n"
           "text/html=helium.desktop\n";
  }

  const std::array mimes = {std::string_view("text/html")};

  REQUIRE(xdgpp::setDefaultApplication(mimes, "firefox.desktop", path));
  REQUIRE(slurp(path) == "[Default Applications]\n"
                         "text/html=firefox.desktop\n"
                         "image/png=swayimg.desktop\n"
                         "\n"
                         "[Added Associations]\n"
                         "text/html=firefox.desktop;chromium.desktop;\n");
}
