#include <catch2/catch_test_macros.hpp>
#include "fuzzy/fzf.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "order-helpers.hpp"

TEST_CASE("match: exact substring scores positive and matches range") {
  const auto &m = fzf::threadLocalMatcher();
  const auto r = m.match("Open File Manager", "file");

  REQUIRE(r.matched());
  REQUIRE(r.score > 0);
  REQUIRE(r.start == 5);
  REQUIRE(r.end == 9);
}

TEST_CASE("match: missing characters return non-match") {
  const auto &m = fzf::threadLocalMatcher();
  const auto r = m.match("Open File Manager", "xyz");

  REQUIRE_FALSE(r.matched());
  REQUIRE(r.score == 0);
}

TEST_CASE("match: diacritic-insensitive matching") {
  const auto &m = fzf::threadLocalMatcher();

  REQUIRE(m.match("Café Society", "cafe").matched());
  REQUIRE(m.match("Mañana", "manana").matched());
  REQUIRE(m.match("Tomáš Brzobohatý", "tomas").matched());
  REQUIRE(m.score_query("Łódź Express", fzf::Query{"lodz"}).weighted);

  // folding is symmetric: accented query matches ASCII text
  REQUIRE(m.match("cafe society", "café").matched());

  REQUIRE_FALSE(m.match("Café", "xyz").matched());
}

TEST_CASE("match: non-Latin scripts are unaffected by folding") {
  const auto &m = fzf::threadLocalMatcher();

  constexpr std::pair<std::string_view, std::string_view> cases[] = {
      {"Привет мир", "мир"}, {"Москва", "оск"}, {"日本語入力", "本語"}, {"中文搜索", "搜索"}};

  for (const auto &[text, pattern] : cases) {
    const auto folded = m.match(text, pattern, true);
    const auto raw = m.match_ascii(text, pattern, true);

    REQUIRE(folded.matched());
    REQUIRE(folded.score == raw.score);
    REQUIRE(folded.start == raw.start);
    REQUIRE(folded.end == raw.end);
    REQUIRE(folded.positions == raw.positions);
  }
}

TEST_CASE("match: Cyrillic and Greek matching is case-insensitive") {
  const auto &m = fzf::threadLocalMatcher();

  REQUIRE(m.match("Открыть настройки", "открыть").matched());
  REQUIRE(m.match("ТЕЛЕГРАМ", "телеграм").matched());
  REQUIRE(m.match("ΤΕΡΜΙΝΑΛ", "τερμιναλ").matched());
}

TEST_CASE("match: match offsets map back to original bytes") {
  const auto &m = fzf::threadLocalMatcher();
  // "Café Bar": C(0) a(1) f(2) é(bytes 3-4) space(5) B(6) a(7) r(8)
  const auto r = m.match("Café Bar", "bar", true);

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
  REQUIRE(m.score_query("Obsidian Wayland", fzf::Query{"obs wld"}).weighted);
  REQUIRE(m.score_query("Search Emojis", fzf::Query{"emoji srch"}).weighted);
  REQUIRE(m.score_query("Keyboard Settings", fzf::Query{"kbd stg"}).weighted);
  REQUIRE(m.score_query("System Info Event Log", fzf::Query{"evlog sinfo"}).weighted);
  REQUIRE(m.score_query("Minecraft", fzf::Query{"mcft"}).weighted);
  REQUIRE(m.score_query("Create Issue For Myself", fzf::Query{"cisfmyslf"}).weighted);

  REQUIRE_FALSE(m.score_query("Minecraft", fzf::Query{"avi"}).weighted);
  REQUIRE_FALSE(m.score_query("System Info Event Log", fzf::Query{"kbd"}).weighted);
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

TEST_CASE("transliterate: non-latin scripts map to ascii") {
  REQUIRE(fzf::transliterate("телеграм") == "telegram");
  REQUIRE(fzf::transliterate("ТЕЛЕГРАМ") == "telegram");
  REQUIRE(fzf::transliterate("Ёж") == "ezh");
  REQUIRE(fzf::transliterate("тел egram") == "tel egram");
  REQUIRE(fzf::transliterate("ΤΕΡΜΙΝΑΛ") == "terminal");
  REQUIRE(fzf::transliterate("Ελλάδα") == "ellada");

  REQUIRE(fzf::transliterate("telegram") == std::nullopt);
  REQUIRE(fzf::transliterate("ьъ") == std::nullopt);
}

TEST_CASE("needsTransliteration: only supported scripts need extra matching") {
  REQUIRE(fzf::needsTransliteration("телеграм"));
  REQUIRE(fzf::needsTransliteration("ΤΕΡΜΙΝΑΛ"));
  REQUIRE_FALSE(fzf::needsTransliteration("telegram"));
  REQUIRE_FALSE(fzf::needsTransliteration("日本語"));
}

TEST_CASE("transliteration: matching across scripts") {
  const auto &matcher = fzf::threadLocalMatcher();

  REQUIRE(matcher.score_query("Telegram", fzf::Query{"телеграм"}).weighted > 0);
  REQUIRE(matcher.score_query("Discord", fzf::Query{"дискорд"}).weighted > 0);
  REQUIRE(matcher.score_query("Konsole", fzf::Query{"консоль"}).weighted > 0);
  REQUIRE(matcher.score_query("Terminal", fzf::Query{"τερμιναλ"}).weighted > 0);
  REQUIRE(matcher.score_query("Telegram", fzf::Query{"музыка"}).weighted == 0);

  REQUIRE(matcher.score_query("Привет мир", fzf::Query{"мир"}).weighted > 0);
  REQUIRE(matcher.score_query("Открыть Discord", fzf::Query{"открыть дискорд"}).weighted > 0);
  REQUIRE(matcher.score_query("Telegram", fzf::Query{"teleg"}).weighted ==
          matcher.match_ascii("Telegram", "teleg").score);
}

TEST_CASE("query score: quality is the worst per-word match, weighted respects field weights") {
  const auto &m = fzf::threadLocalMatcher();
  using WS = fzf::WeightedString;

  std::initializer_list<WS> anki = {
      {"Anki", 1.0f}, {"An intelligent spaced-repetition memory training program", 0.5f}};
  auto empty = std::views::empty<WS>;

  auto perfect = m.score_query(std::initializer_list<WS>{{"Firefox", 1.0f}}, empty, fzf::Query{"fire"});
  REQUIRE(perfect.quality == 100);
  REQUIRE(perfect.score == 100);
  REQUIRE(perfect.weighted == m.match("Firefox", "fire").score);

  auto substring = m.score_query("Firefox", fzf::Query{"fox"});
  REQUIRE(substring.quality == substring.score);
  REQUIRE(substring.quality >= fuzzy::MIN_QUALITY);
  REQUIRE(substring.quality < 100);

  auto scattered = m.score_query(anki, empty, fzf::Query{"time in"});
  REQUIRE(scattered.weighted > 0);
  REQUIRE(scattered.quality == 0);

  auto keywordOnly = m.score_query(anki, empty, fzf::Query{"memory"});
  REQUIRE(keywordOnly.quality == 100);
  REQUIRE(keywordOnly.score == 50);
  REQUIRE(keywordOnly.weighted == static_cast<int>(m.match("memory", "memory").score * 0.5f));

  REQUIRE(m.score_query(anki, empty, fzf::Query{"xyz"}).quality == 0);
}

TEST_CASE("fuzzy::scoreWeighted: normalized match with quality gate") {
  REQUIRE(fuzzy::scoreWeighted({{"Firefox", 1.0}}, fuzzy::Query{"fire"}).accepted());
  REQUIRE(fuzzy::scoreWeighted({{"Firefox", 1.0}}, fuzzy::Query{"fire"}).score == 100);
  REQUIRE(fuzzy::scoreWeighted({{"Thunderbird", 1.0}}, fuzzy::Query{"bird"}).accepted());
  REQUIRE(fuzzy::scoreWeighted({{"Keyboard Settings", 1.0}}, fuzzy::Query{"kbd stg"}).accepted());
  REQUIRE_FALSE(fuzzy::scoreWeighted({{"Firefox", 1.0}}, fuzzy::Query{""}).accepted());
  REQUIRE_FALSE(fuzzy::scoreWeighted({{"Firefox", 1.0}}, fuzzy::Query{"xyz"}).accepted());
  REQUIRE_FALSE(fuzzy::scoreWeighted({{"An intelligent spaced-repetition memory training program", 1.0}}, fuzzy::Query{"ny"}).accepted());

  auto const weighted = fuzzy::scoreWeighted({{"Firefox", 1.0}, {"Web Browser", 0.5}}, fuzzy::Query{"browser"});
  REQUIRE(weighted.quality == 100);
  REQUIRE(weighted.score == 50);
}

TEST_CASE("fuzzy::frecency: bounded and monotonic") {
  constexpr std::int64_t now = 1'800'000'000;
  REQUIRE(fuzzy::frecency(0, std::nullopt, now) == 0.0);
  REQUIRE(fuzzy::frecency(1, now, now) > fuzzy::frecency(1, now - 30 * 86400, now));
  REQUIRE(fuzzy::frecency(50, now, now) > fuzzy::frecency(5, now, now));
  REQUIRE(fuzzy::frecency(100000, now, now) == 1.0);
}

TEST_CASE("match: coherence separates substrings/abbreviations/acronyms from scattered matches") {
  const auto &m = fzf::threadLocalMatcher();

  REQUIRE(m.match("Runtime Settings", "time").coherent);
  REQUIRE(m.match("Keyboard", "kbd").coherent);
  REQUIRE(m.match("Start Input Method", "sim").coherent);
  REQUIRE(m.match("Event Log", "evlog").coherent);
  REQUIRE(m.match("Firefox Developer Edition", "fdev").coherent);
  REQUIRE(m.match("Café Bar", "cafba").coherent);

  REQUIRE_FALSE(m.match("Play this game on Steam", "time").coherent);
  REQUIRE_FALSE(m.match("Start Input Method", "time").coherent);
  REQUIRE_FALSE(m.match("An intelligent spaced-repetition memory training program", "time").coherent);

  REQUIRE_FALSE(fuzzy::scoreWeighted({{"Play this game on Steam", 1.0}}, fuzzy::Query{"time"}).accepted());
  REQUIRE(fuzzy::scoreWeighted({{"Play this game on Steam", 1.0}}, fuzzy::Query{"steam"}).accepted());
}
