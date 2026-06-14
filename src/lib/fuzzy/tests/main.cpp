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

TEST_CASE("fuzzy_match_v2: diacritic-insensitive matching") {
  const auto &m = fzf::threadLocalMatcher();

  REQUIRE(m.fuzzy_match_v2("Café Society", "cafe").matched());
  REQUIRE(m.fuzzy_match_v2("Mañana", "manana").matched());
  REQUIRE(m.fuzzy_match_v2("Tomáš Brzobohatý", "tomas").matched());
  REQUIRE(m.fuzzy_match_v2_score_query("Łódź Express", "lodz"));

  // folding is symmetric: accented query matches ASCII text
  REQUIRE(m.fuzzy_match_v2("cafe society", "café").matched());

  REQUIRE_FALSE(m.fuzzy_match_v2("Café", "xyz").matched());
}

TEST_CASE("fuzzy_match_v2: non-Latin scripts are unaffected by folding") {
  const auto &m = fzf::threadLocalMatcher();

  constexpr std::pair<std::string_view, std::string_view> cases[] = {
      {"Привет мир", "мир"}, {"Москва", "оск"}, {"日本語入力", "本語"}, {"中文搜索", "搜索"}};

  for (const auto &[text, pattern] : cases) {
    const auto folded = m.fuzzy_match_v2(text, pattern, false, true);
    const auto raw = m.fuzzy_match_v2_ascii(text, pattern, false, true);

    REQUIRE(folded.matched());
    REQUIRE(folded.score == raw.score);
    REQUIRE(folded.start == raw.start);
    REQUIRE(folded.end == raw.end);
    REQUIRE(folded.positions == raw.positions);
  }
}

TEST_CASE("fuzzy_match_v2: match offsets map back to original bytes") {
  const auto &m = fzf::threadLocalMatcher();
  // "Café Bar": C(0) a(1) f(2) é(bytes 3-4) space(5) B(6) a(7) r(8)
  const auto r = m.fuzzy_match_v2("Café Bar", "bar", false, true);

  REQUIRE(r.matched());
  REQUIRE(r.start == 6);
  REQUIRE(r.end == 9);
  REQUIRE(r.positions == std::vector<int>{6, 7, 8});
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
