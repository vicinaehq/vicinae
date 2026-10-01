#include <catch2/catch_test_macros.hpp>
#include <xdgpp/xdgpp.hpp>

TEST_CASE("encodes local paths as file uris", XDGPP_GROUP) {
  REQUIRE(xdgpp::toFileUri("/home/user/file.txt") == "file:///home/user/file.txt");
  REQUIRE(xdgpp::toFileUri("/home/user/my file.txt") == "file:///home/user/my%20file.txt");
  REQUIRE(xdgpp::toFileUri("/home/user/é.txt") == "file:///home/user/%C3%A9.txt");
  REQUIRE(xdgpp::toFileUri("/a/b#c?d") == "file:///a/b%23c%3Fd");
}

TEST_CASE("decodes file uris to local paths", XDGPP_GROUP) {
  REQUIRE(xdgpp::fromFileUri("file:///home/user/file.txt") == "/home/user/file.txt");
  REQUIRE(xdgpp::fromFileUri("file:///home/user/my%20file.txt") == "/home/user/my file.txt");
  REQUIRE(xdgpp::fromFileUri("file:///home/user/%c3%a9.txt") == "/home/user/é.txt");
  REQUIRE(xdgpp::fromFileUri("file://localhost/etc/hosts") == "/etc/hosts");
}

TEST_CASE("rejects non-local uris", XDGPP_GROUP) {
  REQUIRE_FALSE(xdgpp::fromFileUri("https://example.com/file.txt"));
  REQUIRE_FALSE(xdgpp::fromFileUri("file://nas/share/file.txt"));
  REQUIRE_FALSE(xdgpp::fromFileUri("trash:///file.txt"));
  REQUIRE_FALSE(xdgpp::fromFileUri("file:///bad%zz"));
  REQUIRE_FALSE(xdgpp::fromFileUri("file:///nul%00"));
}

TEST_CASE("file uri roundtrips", XDGPP_GROUP) {
  std::filesystem::path const path = "/tmp/dir with spaces/ünïcode & [brackets].png";
  REQUIRE(xdgpp::fromFileUri(xdgpp::toFileUri(path)) == path);
}
