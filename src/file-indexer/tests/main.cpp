#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include "file-indexer/io-pacer.hpp"
#include "file-indexer/util.hpp"
#include "file-indexer/vocabulary.hpp"
#include "file-indexer/file-indexer-query-engine.hpp"

using file_indexer::vocab::basenameView;
using file_indexer::vocab::isJunkToken;
using file_indexer::vocab::tokenizeFilename;
using namespace file_indexer::query;
using Suggestion = FileIndexerDatabase::SpellfixSuggestion;

static std::vector<std::string> correctionWords(const std::vector<Suggestion> &picked) {
  return picked | std::views::transform(&Suggestion::word) | std::ranges::to<std::vector>();
}

TEST_CASE("basenameView extracts the last path component") {
  CHECK(basenameView("/home/user/docs/report.pdf") == "report.pdf");
  CHECK(basenameView("/home/user/docs") == "docs");
  CHECK(basenameView("report.pdf") == "report.pdf");
  CHECK(basenameView("/") == "");
}

TEST_CASE("tokenizeFilename splits on non-alphanumeric characters") {
  CHECK(tokenizeFilename("kube-config.yaml") == std::vector<std::string>{"kube", "config", "yaml"});
  CHECK(tokenizeFilename("my_report (final).pdf") == std::vector<std::string>{"report", "final", "pdf"});
}

TEST_CASE("tokenizeFilename splits camelCase boundaries") {
  CHECK(tokenizeFilename("fileIndexerService") == std::vector<std::string>{"file", "indexer", "service"});
  CHECK(tokenizeFilename("HTTPServer") == std::vector<std::string>{"http", "server"});
  CHECK(tokenizeFilename("v2Final") == std::vector<std::string>{"final"});
}

TEST_CASE("tokenizeFilename lowercases tokens") {
  CHECK(tokenizeFilename("REPORT.PDF") == std::vector<std::string>{"report", "pdf"});
}

TEST_CASE("tokenizeFilename drops short and all-digit tokens") {
  CHECK(tokenizeFilename("a_to_b.md") == std::vector<std::string>{});
  CHECK(tokenizeFilename("IMG_20240115.jpg") == std::vector<std::string>{"img", "jpg"});
  CHECK(tokenizeFilename("...") == std::vector<std::string>{});
  CHECK(tokenizeFilename("") == std::vector<std::string>{});
}

TEST_CASE("tokenizeFilename keeps digits inside mixed tokens") {
  CHECK(tokenizeFilename("k8s-notes.txt") == std::vector<std::string>{"k8s", "notes", "txt"});
  CHECK(tokenizeFilename("mp3gain") == std::vector<std::string>{"mp3gain"});
}

TEST_CASE("tokenizeFilename keeps non-ascii bytes in tokens") {
  CHECK(tokenizeFilename("résumé.pdf") == std::vector<std::string>{"résumé", "pdf"});
}

TEST_CASE("isJunkToken drops hash-like and overlong tokens") {
  // git object names, content hashes
  CHECK(isJunkToken("fa7086f94e22ede4670185890453f911"));
  CHECK(isJunkToken("d6a2e953560029757dba39b9303e5b20"));
  // hex fragments under the length cap
  CHECK(isJunkToken("abcdef123456"));
  // overlong non-hex tokens (minified assets, base64 names)
  CHECK(isJunkToken("zxqwjkrtyplmbvcxzqwjkrtypl"));
}

TEST_CASE("isJunkToken keeps human vocabulary") {
  CHECK_FALSE(isJunkToken("downloads"));
  CHECK_FALSE(isJunkToken("qsystemsemaphore"));
  CHECK_FALSE(isJunkToken("internationalization"));
  // hex-only letters but too short to be a hash
  CHECK_FALSE(isJunkToken("deadbeef"));
  CHECK_FALSE(isJunkToken("facade"));
  // mixed alpha tokens at hash-like lengths are not hex
  CHECK_FALSE(isJunkToken("modernisation"));
}

TEST_CASE("tokenizeFilename drops junk tokens") {
  CHECK(tokenizeFilename("fa7086f94e22ede4670185890453f911_backup.tar") ==
        std::vector<std::string>{"backup", "tar"});
}

TEST_CASE("IoPacer parses pressure stall information") {
  using file_indexer::IoPacer;

  CHECK(IoPacer::parseSomeAvg10("some avg10=12.34 avg60=1.00 avg300=0.10 total=12345\n"
                                "full avg10=0.00 avg60=0.00 avg300=0.00 total=0\n") == 12.34);
  CHECK(IoPacer::parseSomeAvg10("some avg10=0.00 avg60=0.00 avg300=0.00 total=0") == 0.0);
  CHECK_FALSE(IoPacer::parseSomeAvg10("garbage").has_value());
  CHECK_FALSE(IoPacer::parseSomeAvg10("").has_value());
}

TEST_CASE("IoPacer without PSI support is a no-op") {
  file_indexer::IoPacer pacer{"/nonexistent/pressure/io"};
  // must not block, sleep or crash
  for (int i = 0; i < 1000; ++i) {
    pacer.checkpoint();
  }
}

TEST_CASE("pickCorrections takes best suggestions under the distance cutoff") {
  std::vector<Suggestion> const suggestions = {
      {.word = "budget", .distance = 100, .score = 125},
      {.word = "budgie", .distance = 100, .score = 126},
      {.word = "budgets", .distance = 100, .score = 128},
      {.word = "bandage", .distance = 300, .score = 80},
  };

  auto picked = correctionWords(pickCorrections(suggestions, "budgte", 3));
  CHECK(picked == std::vector<std::string>{"budget", "budgie"});
}

TEST_CASE("pickCorrections collapses numbered variant families") {
  std::vector<Suggestion> const suggestions = {
      {.word = "sercom5", .distance = 21, .score = 51}, {.word = "sercom3", .distance = 21, .score = 51},
      {.word = "sercom", .distance = 21, .score = 51},  {.word = "src", .distance = 41, .score = 59},
      {.word = "search", .distance = 40, .score = 59},  {.word = "searchable", .distance = 40, .score = 66},
  };

  // sercom replaces sercom5 (prefix = broader substring coverage), sercom3 collapses
  // into the same family, searchable collapses into search
  auto picked = correctionWords(pickCorrections(suggestions, "serach", 3));
  CHECK(picked == std::vector<std::string>{"sercom", "src", "search"});
}

TEST_CASE("pickCorrections keeps genuinely distinct words apart") {
  std::vector<Suggestion> const suggestions = {
      {.word = "famine", .distance = 40, .score = 68},
      {.word = "feminism", .distance = 80, .score = 107},
      {.word = "feminisation", .distance = 80, .score = 108},
  };

  auto picked = correctionWords(pickCorrections(suggestions, "famina", 3));
  CHECK(picked == std::vector<std::string>{"famine", "feminism", "feminisation"});
}

TEST_CASE("pickCorrections drops the original word and its extensions") {
  std::vector<Suggestion> const suggestions = {
      {.word = "engine", .distance = 0, .score = 25, .rank = 1},
      {.word = "engines", .distance = 40, .score = 60, .rank = 1},
      {.word = "engineering", .distance = 60, .score = 80, .rank = 1},
  };

  auto picked = correctionWords(pickCorrections(suggestions, "Engine", 3));
  CHECK(picked == std::vector<std::string>{});
}

TEST_CASE("pickCorrections trusts frequent vocabulary words and corrects rare ones") {
  std::vector<Suggestion> const frequent = {
      {.word = "vicinae", .distance = 0, .score = 25, .rank = 5000},
      {.word = "visionary", .distance = 110, .score = 90, .rank = 2},
  };
  CHECK(correctionWords(pickCorrections(frequent, "vicinae", 3)) == std::vector<std::string>{});

  // with trust disabled (last-resort retry), even known words get corrected
  CHECK(correctionWords(pickCorrections(frequent, "vicinae", 3, false)) ==
        std::vector<std::string>{"visionary"});

  // a one-off exact match may itself be a typo'd filename: still correct it
  std::vector<Suggestion> const rare = {
      {.word = "budgte", .distance = 0, .score = 25, .rank = 1},
      {.word = "budget", .distance = 100, .score = 125, .rank = 200},
  };
  CHECK(correctionWords(pickCorrections(rare, "budgte", 3)) == std::vector<std::string>{"budget"});
}

TEST_CASE("buildCorrectionPlans picks one correction plan at a time") {
  std::vector<QueryWord> const words = {
      {.word = "hello", .corrections = {}},
      {.word = "budgte", .corrections = {{.word = "budget"}, {.word = "budgie"}}},
      {.word = "vrs", .corrections = {{.word = "vers"}}},
  };

  auto plans = buildCorrectionPlans(words, 4);

  REQUIRE(plans.size() == 2);
  CHECK(prepareCorrectionSearchQuery(plans[0]) == R"("hello" "budget" "vers")");
  CHECK(prepareCorrectionSearchQuery(plans[1]) == R"("hello" "budgie" "vers")");
}

TEST_CASE("prepareCorrectionSearchQuery drops bigrams entirely") {
  std::vector<QueryWord> const words = {
      {.word = "budgte", .corrections = {{.word = "budget"}}},
      {.word = "yo", .corrections = {}},
      {.word = "a", .corrections = {}},
  };

  // sub-3-char words are uncorrectable: as hard filters they only flood or nullify,
  // so they are enforced by the reranker instead
  auto plans = buildCorrectionPlans(words, 4);

  REQUIRE(plans.size() == 1);
  CHECK(prepareCorrectionSearchQuery(plans[0]) == R"("budget")");
}

TEST_CASE("correctionWeight decays with spelling distance") {
  // a close correction (frostwire d=20) must outweigh a far one (firstperson d=120)
  // enough that the far one can't win on raw fzf match length alone
  CHECK(correctionWeight(0) == CORRECTION_PENALTY);
  CHECK(correctionWeight(20) > correctionWeight(120) * 1.5);
  CHECK(correctionWeight(MAX_CORRECTION_DISTANCE) == CORRECTION_PENALTY * 0.5);
}

TEST_CASE("compactSubtrees drops paths covered by another path") {
  namespace fs = std::filesystem;
  using file_indexer::compactSubtrees;

  CHECK(compactSubtrees({"/a/b/c", "/a/b", "/a/bc", "/d"}) == std::vector<fs::path>{"/d", "/a/b", "/a/bc"});
  CHECK(compactSubtrees({"/a", "/a"}) == std::vector<fs::path>{"/a"});
  CHECK(compactSubtrees({}) == std::vector<fs::path>{});
}

TEST_CASE("splitQueryWords trims and drops empty words") {
  CHECK(splitQueryWords("  hello   yo ") == std::vector<std::string_view>{"hello", "yo"});
  CHECK(splitQueryWords("") == std::vector<std::string_view>{});
}
