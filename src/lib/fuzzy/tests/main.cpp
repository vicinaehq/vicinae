#include <catch2/catch_test_macros.hpp>
#include "fuzzy/fzf.hpp"
#include "order-helpers.hpp"

TEST_CASE("fuzzy_match_v2: exact substring scores positive and matches range") {
  const auto &m = fzf::threadLocalMatcher();
  const auto r = m.fuzzy_match_v2("Open File Manager", "file");

  REQUIRE(r.matched());
  REQUIRE(r.score > 0);
  REQUIRE(r.start == 5);
  REQUIRE(r.end == 9);
}

TEST_CASE("fuzzy_match_v2: missing characters return non-match") {
  const auto &m = fzf::threadLocalMatcher();
  const auto r = m.fuzzy_match_v2("Open File Manager", "xyz");

  REQUIRE_FALSE(r.matched());
  REQUIRE(r.score == 0);
}

TEST_CASE("ordering: word-boundary match outranks scattered match") {
  fuzzy::test::expectRankedOrder({"File Manager", "profile editor"}, "file");
}

TEST_CASE("sparse matches") {
  const auto &m = fzf::threadLocalMatcher();
  REQUIRE(m.fuzzy_match_v2_score_query("Obsidian Wayland", "obs wld"));
  REQUIRE(m.fuzzy_match_v2_score_query("Search Emojis", "emoji srch"));
  REQUIRE(m.fuzzy_match_v2_score_query("Keyboard Settings", "kbd stg"));
  REQUIRE(m.fuzzy_match_v2_score_query("System Info Event Log", "evlog sinfo"));
  REQUIRE(m.fuzzy_match_v2_score_query("Minecraft", "mcft"));
  REQUIRE(m.fuzzy_match_v2_score_query("Create Issue For Myself", "cisfmyslf"));

  REQUIRE_FALSE(m.fuzzy_match_v2_score_query("Minecraft", "avi"));
  REQUIRE_FALSE(m.fuzzy_match_v2_score_query("System Info Event Log", "kbd"));
}

// yeah, not feeling very inspired...
TEST_CASE("ordering: various interesting cases") {
  fuzzy::test::expectRankedOrder(
      {"Clipboard History", "Clear Current Clipboard Data", "Clear Clipboard History"}, "clip");
  fuzzy::test::expectRankedOrder({"pkg", "Packages", "Arch Packages"}, "pkg");
}

TEST_CASE("ordering: github issue #1326 examples") {
  fuzzy::test::expectRankedOrder({"Espanso.code-workspace", "Rofi.code-workspace"}, "Esp");
  fuzzy::test::expectRankedOrder({"b", "bm"}, "b");
}

TEST_CASE("ordering: github issue #946 examples") {
  fuzzy::test::expectRankedOrder({"Konsole", "OpenJDK Java 17 Console"}, "konsole");
  fuzzy::test::expectRankedOrder({"eos-update", "Configure EOS Update Notifier"}, "eos");
  fuzzy::test::expectRankedOrder({"Avidemux", "Donate to vicinae"}, "avi");
  fuzzy::test::expectRankedOrder({"Spotify", "Reload Script Directories", "Sysprog"}, "Spo");
}
