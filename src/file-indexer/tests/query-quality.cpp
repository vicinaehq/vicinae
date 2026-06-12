#include <catch2/catch_test_macros.hpp>
#include <sqlcipher/sqlite3.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <string_view>
#include <system_error>
#include <vector>
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/file-indexer-query-engine.hpp"

extern "C" int vicinaeFuzzyTrigramInit(sqlite3 *, char **, const void *);
extern "C" int vicinaeSpellfixInit(sqlite3 *, char **, const void *);

namespace fs = std::filesystem;

namespace {

// every path is created as a real empty file: result ranking stats paths and
// silently drops entries that don't exist
constexpr std::string_view CORPUS[] = {
    "home/docs/budget_2024.xlsx",
    "home/docs/invoice.xlsx",
    "home/docs/vers_la_modernisation.pdf",
    "home/docs/hello-yo.txt",
    "home/docs/notes/meeting_notes.md",
    "home/pictures/icons/frostwire.svg",
    "home/pictures/icons/firstperson.svg",
    "home/pictures/icons/firstparty.svg",
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
};

struct QualityEnv {
  fs::path root;

  QualityEnv() {
    sqlite3_auto_extension(reinterpret_cast<void (*)()>(vicinaeFuzzyTrigramInit));
    sqlite3_auto_extension(reinterpret_cast<void (*)()>(vicinaeSpellfixInit));

    std::string tmpl = (fs::temp_directory_path() / "vicinae-fi-quality.XXXXXX").string();
    root = mkdtemp(tmpl.data());

    setenv("XDG_DATA_HOME", (root / "data").c_str(), 1);

    std::vector<fs::path> paths;
    paths.reserve(std::size(CORPUS));

    for (auto const relative : CORPUS) {
      fs::path const path = root / "corpus" / relative;
      fs::create_directories(path.parent_path());
      std::ofstream{path};
      paths.emplace_back(path);
    }

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

// rank of the first result whose path ends with `suffix`, or -1 when absent
int rankOf(std::string_view query, std::string_view suffix) {
  static QualityEnv const env;
  FileIndexerQueryEngine engine;

  for (auto const [idx, result] : engine.query(query, 10) | std::views::enumerate) {
    if (std::string_view{result.path.native()}.ends_with(suffix)) { return static_cast<int>(idx) + 1; }
  }

  return -1;
}

bool inTop(std::string_view query, std::string_view suffix, int k) {
  int const rank = rankOf(query, suffix);
  return rank > 0 && rank <= k;
}

} // namespace

TEST_CASE("strict path: exact words rank their file first") {
  CHECK(rankOf("budget", "budget_2024.xlsx") == 1);
  CHECK(rankOf("invoice xlsx", "invoice.xlsx") == 1);
}

TEST_CASE("strict path: bigrams match through prefix queries") {
  CHECK(rankOf("hello yo", "hello-yo.txt") == 1);
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
  FileIndexerQueryEngine engine;
  CHECK(engine.query("zzqqxxw", 10).empty());
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
  // the path separator bridges too: components fuse for retrieval
  CHECK(inTop("kernelvm", "vm_kmap.c", 3));
}

TEST_CASE("skeleton: dropped consonants match through skip-grams") {
  // these abbreviations drop a consonant the skeleton keeps ('cfg' vs 'cnfg',
  // 'dwld' vs 'dwnlds', 'kmap' vs 'kymp'): the colocated skip-one trigrams cover them
  CHECK(inTop("cfg backup", "kubeConfigBackup.yaml", 3));
  CHECK(inTop("cfg bkup", "kubeConfigBackup.yaml", 3));
  CHECK(inTop("dwld", "downloads_backup.tar", 3));
  CHECK(inTop("kmap", "keymaps/default/keymap.c", 3));
}
