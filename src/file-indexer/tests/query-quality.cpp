#include <catch2/catch_test_macros.hpp>
#include <sqlcipher/sqlite3.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/file-indexer-query-engine.hpp"

extern "C" int vicinaeFuzzyTrigramInit(sqlite3 *, char **, const void *);
extern "C" int vicinaeSpellfixInit(sqlite3 *, char **, const void *);

namespace fs = std::filesystem;

namespace {

// registered before any test can open a connection, so test order can't matter
struct ExtensionRegistrar {
  ExtensionRegistrar() {
    sqlite3_auto_extension(reinterpret_cast<void (*)()>(vicinaeFuzzyTrigramInit));
    sqlite3_auto_extension(reinterpret_cast<void (*)()>(vicinaeSpellfixInit));
  }
};

ExtensionRegistrar const g_extensionRegistrar;

// every path is created as a real empty file: result ranking stats paths and
// silently drops entries that don't exist
constexpr std::string_view CORPUS[] = {
    "home/docs/budget_2024.xlsx",
    "home/docs/invoice.xlsx",
    "home/docs/Répondre à la nuit.epub",
    "home/docs/vers_la_modernisation.pdf",
    "home/docs/hello-yo.txt",
    "home/docs/notes/meeting_notes.md",
    "home/pictures/icons/frostwire.svg",
    "home/pictures/icons/firstperson.svg",
    "home/pictures/icons/firstparty.svg",
    "home/pictures/icons/SymbolEditor.svg",
    "home/pictures/wallpapers/4k-oled/jellyfish-amoled.png",
    "home/dev/sdk/include/sercom0.h",
    "home/dev/sdk/include/sercom1.h",
    "home/dev/sdk/include/sercom2.h",
    "home/dev/sdk/include/sercom3.h",
    "home/dev/sdk/include/sercom4.h",
    "home/dev/sdk/include/sercom5.h",
    "home/dev/sdk/include/sercom6.h",
    "home/dev/sdk/include/sercom7.h",
    "home/config/vivaldi/search_engines.json",
    "home/dev/kubeConfigBackup.yaml",
    "home/dev/searchEngine.js",
    "home/dev/captures/vrs-dump0.bin",
    "home/dev/captures/vrs-dump1.bin",
    "home/dev/captures/vrs-dump2.bin",
    "home/music/mayonnaise.flac",
    "home/music/yolo-compilation.mp4",
    "home/archive/downloads_backup.tar",
    "home/qmk_firmware/keyboards/planck/keymaps/default/keymap.c",
    "home/dev/kernel/vm_kmap.c",
    "home/docs/la belle data.pdf",
    "home/pictures/Screen Shot 2024-01-15 at 10.30.12.png",
    "home/fonts/SF-Pro-Display-Bold.otf",
    "home/dev/data_loader_utils.py",
    "home/apps/vicinae.AppImage",
    "home/dev/jai/modules/windows.jai",
    "home/pictures/VACATION.JPG",
    "home/pictures/forest_color.jpg",
};

fs::path makeQualityRoot() {
  auto const timestamp = std::chrono::system_clock::now().time_since_epoch().count();
  auto const base = fs::temp_directory_path() / ("vicinae-fi-quality-" + std::to_string(timestamp));

  for (int attempt = 0; attempt != 100; ++attempt) {
    auto candidate = base;
    if (attempt != 0) { candidate += "-" + std::to_string(attempt); }

    std::error_code ec;
    if (fs::create_directory(candidate, ec)) { return candidate; }
  }

  throw std::runtime_error{"failed to create quality test root"};
}

struct QualityEnv {
  fs::path root;

  QualityEnv() {
    root = makeQualityRoot();

    setenv("XDG_DATA_HOME", (root / "data").c_str(), 1);
    setenv("XDG_CACHE_HOME", (root / "cache").c_str(), 1);

    std::vector<fs::path> paths;
    paths.reserve(std::size(CORPUS));

    for (auto const relative : CORPUS) {
      fs::path const path = root / "corpus" / relative;
      fs::create_directories(path.parent_path());
      std::ofstream ofs{path};
      if (relative == std::string_view{"home/docs/budget_2024.xlsx"}) { ofs << "budget-data"; }
      paths.emplace_back(path);
    }
    paths.emplace_back(root / "corpus" / "home/docs");

    FileIndexerDatabase db;
    db.init();
    db.indexFiles(paths);
    db.rebuildSpellfixVocabulary();
  }

  ~QualityEnv() {
    std::error_code ec;
    fs::remove_all(root, ec);
  }
};

const QualityEnv &qualityEnv() {
  static QualityEnv const env;
  return env;
}

// rank of the first result whose path ends with `suffix`, or -1 when absent
int rankOf(std::string_view query, std::string_view suffix) {
  qualityEnv();
  FileIndexerQueryEngine engine;

  for (auto const [idx, result] : engine.query(query, 10) | std::views::enumerate) {
    if (std::string_view{result.path.native()}.ends_with(suffix)) { return static_cast<int>(idx) + 1; }
  }

  return -1;
}

int rankOf(std::string_view query, std::string_view suffix, IndexedFileCategory category) {
  qualityEnv();
  FileIndexerQueryEngine engine;
  FileIndexerQueryEngine::QueryOptions const options{.category = category};

  for (auto const [idx, result] : engine.query(query, 10, options) | std::views::enumerate) {
    if (std::string_view{result.path.native()}.ends_with(suffix)) { return static_cast<int>(idx) + 1; }
  }

  return -1;
}

bool inTop(std::string_view query, std::string_view suffix, int k) {
  int const rank = rankOf(query, suffix);
  return rank > 0 && rank <= k;
}

bool inTop(std::string_view query, std::string_view suffix, int k, IndexedFileCategory category) {
  int const rank = rankOf(query, suffix, category);
  return rank > 0 && rank <= k;
}

bool hasStrictCandidate(std::string_view query, std::string_view suffix, IndexedFileCategory category) {
  qualityEnv();
  FileIndexerDatabase db;
  FileIndexerDatabase::SearchOptions const options{.category = category};
  auto const candidates = db.searchCandidates(query, 10, options);

  return std::ranges::any_of(candidates, [&](const auto &candidate) {
    return std::string_view{candidate.path.native()}.ends_with(suffix);
  });
}

std::optional<IndexedFileCategory> categoryOf(std::string_view query, std::string_view suffix) {
  qualityEnv();
  FileIndexerQueryEngine engine;

  for (const auto &result : engine.query(query, 10)) {
    if (std::string_view{result.path.native()}.ends_with(suffix)) { return result.category; }
  }

  return std::nullopt;
}

} // namespace

TEST_CASE("strict path: exact words rank their file first") {
  CHECK(rankOf("budget", "budget_2024.xlsx") == 1);
  CHECK(rankOf("invoice xlsx", "invoice.xlsx") == 1);
}

TEST_CASE("strict path: bigrams match through prefix queries") {
  CHECK(rankOf("hello yo", "hello-yo.txt") == 1);
}

TEST_CASE("strict path: trigram search ignores diacritics") {
  CHECK(hasStrictCandidate("repondre", "Répondre à la nuit.epub", IndexedFileCategory::Document));
}

TEST_CASE("fallback: transposition typo") { CHECK(inTop("budgte", "budget_2024.xlsx", 3)); }

TEST_CASE("fallback: missing character typo") { CHECK(inTop("mayonaise", "mayonnaise.flac", 3)); }

TEST_CASE("fallback: close correction outranks longer far correction") {
  // firstperson/firstparty match more characters but sit at a far spelling distance
  CHECK(rankOf("frstwire", "frostwire.svg") == 1);
}

TEST_CASE("fallback: word fragment corrects against vocabulary prefixes") {
  CHECK(inTop("moderna", "vers_la_modernisation.pdf", 3));
}

TEST_CASE("fallback: numbered variant family does not crowd out the real correction") {
  CHECK(inTop("serach engine", "search_engines.json", 3));
}

TEST_CASE("fallback: typo'd extension is corrected") { CHECK(inTop("invoice xslx", "invoice.xlsx", 3)); }

TEST_CASE("fallback: vowel drop against camelCase-derived vocabulary") {
  CHECK(inTop("confg backup", "kubeConfigBackup.yaml", 3));
}

TEST_CASE("fallback: weak words are dropped from filtering but still ranked") {
  // 'vrs' is a real corpus token (vrs-dump*, rank 3 -> trusted), so the first
  // corrected pass returns nothing and the distrust retry must correct it to
  // 'vers'; 'lo' is an uncorrectable bigram enforced only by the reranker
  CHECK(inTop("vrs lo moderna", "vers_la_modernisation.pdf", 3));
}

TEST_CASE("fallback: gibberish returns nothing rather than noise") {
  auto const &env = qualityEnv();
  FileIndexerQueryEngine engine;
  auto results = engine.query("zzqqxxw", 10);

  INFO("quality root: " << env.root);
  for (const auto &result : results) {
    INFO("result: " << result.path);
  }

  CHECK(results.empty());
}

TEST_CASE("skeleton: fully devowelized word matches") {
  // distance 3 from 'downloads': far out of spellfix reach
  CHECK(inTop("dwnlds", "downloads_backup.tar", 3));
}

TEST_CASE("skeleton: real-world abbreviation mix") {
  // 'firm' covers firmware by substring, 'dflt' needs the skeleton index
  CHECK(inTop("qmk firm dflt", "keymaps/default/keymap.c", 3));
}

TEST_CASE("skeleton: merges with literal matches instead of being gated by them") {
  // 'kmap' literally matches vm_kmap.c, which must not lock keymap.c out of the
  // results: both compete in one rerank, the contiguous match winning on score
  CHECK(rankOf("kmap", "vm_kmap.c") == 1);
  CHECK(inTop("kmap", "keymaps/default/keymap.c", 4));
}

TEST_CASE("bridging: concatenated queries match across word boundaries") {
  // trigram windows no longer reset at spaces, so spaceless queries reach
  // space-separated filenames
  CHECK(inTop("labelledata", "la belle data.pdf", 3));
  CHECK(inTop("screenshot", "Screen Shot 2024-01-15 at 10.30.12.png", 3));
}

TEST_CASE("bridging: every separator collapses like a space") {
  CHECK(inTop("sfpro", "SF-Pro-Display-Bold.otf", 3));
  CHECK(inTop("sf pro", "SF-Pro-Display-Bold.otf", 3));
  CHECK(inTop("dataloader", "data_loader_utils.py", 3));
  CHECK(inTop("invoicexlsx", "invoice.xlsx", 3));
}

TEST_CASE("directories are typed in the index and queryable by recency") {
  // any query forces the shared corpus env into existence
  CHECK(rankOf("budget", "budget_2024.xlsx") == 1);

  FileIndexerDatabase db;
  auto const dirs = db.listRecentDirectories(100);

  CHECK_FALSE(dirs.empty());

  for (const auto &dir : dirs) {
    CHECK(fs::is_directory(dir));
  }
}

TEST_CASE("structural parent directories are not queryable as indexed content") {
  auto const &env = qualityEnv();
  FileIndexerDatabase db;

  auto const parent = env.root / "corpus" / "home";

  CHECK_FALSE(db.tracksFile(parent));
  CHECK_FALSE(inTop("home", "corpus/home", 10, IndexedFileCategory::Directory));
}

TEST_CASE("indexed entries persist file size and refresh time") {
  auto const &env = qualityEnv();
  fs::path const file = env.root / "corpus" / "home/docs/budget_2024.xlsx";
  fs::path const dir = env.root / "corpus" / "home/docs";
  FileIndexerDatabase db;

  CHECK(db.retrieveIndexedSizeBytes(file) == 11);
  CHECK_FALSE(db.retrieveIndexedSizeBytes(dir).has_value());
  CHECK(db.retrieveIndexedAt(file).value_or(0) > 0);
  CHECK(db.retrieveIndexedAt(dir).value_or(0) > 0);
}

TEST_CASE("query results include indexed file categories") {
  CHECK(categoryOf("invoice", "invoice.xlsx") == IndexedFileCategory::Document);
  CHECK(categoryOf("vacation", "VACATION.JPG") == IndexedFileCategory::Image);
  CHECK(categoryOf("mayonnaise", "mayonnaise.flac") == IndexedFileCategory::Audio);
  CHECK(categoryOf("yolo", "yolo-compilation.mp4") == IndexedFileCategory::Video);
  CHECK(categoryOf("downloads", "downloads_backup.tar") == IndexedFileCategory::Archive);
  CHECK(categoryOf("keymap", "keymap.c") == IndexedFileCategory::Other);
  CHECK(categoryOf("vicinae", "vicinae.AppImage") == IndexedFileCategory::Application);
  CHECK(categoryOf("vrs dump0", "vrs-dump0.bin") == IndexedFileCategory::Other);
  CHECK(categoryOf("docs", "home/docs") == IndexedFileCategory::Directory);
}

TEST_CASE("category filter returns matching files") {
  CHECK(inTop("invoice", "invoice.xlsx", 3, IndexedFileCategory::Document));
  CHECK(inTop("vacation", "VACATION.JPG", 3, IndexedFileCategory::Image));
  CHECK(inTop("mayonnaise", "mayonnaise.flac", 3, IndexedFileCategory::Audio));
  CHECK(inTop("yolo", "yolo-compilation.mp4", 3, IndexedFileCategory::Video));
  CHECK(inTop("downloads", "downloads_backup.tar", 3, IndexedFileCategory::Archive));
  CHECK(inTop("keymap", "keymap.c", 3, IndexedFileCategory::Other));
  CHECK(inTop("vicinae", "vicinae.AppImage", 3, IndexedFileCategory::Application));
  CHECK(inTop("vrs dump0", "vrs-dump0.bin", 3, IndexedFileCategory::Other));
  CHECK(inTop("docs", "home/docs", 3, IndexedFileCategory::Directory));
}

TEST_CASE("category filter is case-insensitive for extensions") {
  CHECK(inTop("vacation", "VACATION.JPG", 3, IndexedFileCategory::Image));
  CHECK_FALSE(inTop("vacation", "VACATION.JPG", 3, IndexedFileCategory::Document));
}

TEST_CASE("skeleton: dropped consonants match through skip-grams") {
  // these abbreviations drop a consonant the skeleton keeps ('cfg' vs 'cnfg',
  // 'dwld' vs 'dwnlds', 'kmap' vs 'kymp'): the colocated skip-one trigrams cover them
  CHECK(inTop("cfg backup", "kubeConfigBackup.yaml", 3));
  CHECK(inTop("cfg bkup", "kubeConfigBackup.yaml", 3));
  CHECK(inTop("dwld", "downloads_backup.tar", 3));
  CHECK(inTop("kmap", "keymaps/default/keymap.c", 3));
  CHECK(inTop("frs clr", "home/pictures/forest_color.jpg", 3));
}

TEST_CASE("skeleton: short skeleton tokens are kept in phrase matches") {
  CHECK(inTop("jai wndws", "jai/modules/windows.jai", 3));
}

TEST_CASE("exact path component substring outranks fuzzy filename subsequence") {
  CHECK(rankOf("oled", "4k-oled/jellyfish-amoled.png") < rankOf("oled", "icons/SymbolEditor.svg"));
}
