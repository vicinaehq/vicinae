#include <catch2/catch_test_macros.hpp>
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/file-indexer.hpp"
#include "services/files-service/file-indexer/scan.hpp"
#include "test-helpers.hpp"
#include <filesystem>
#include <fstream>
#include <chrono>

TEST_CASE("File type affects ranking", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-filetype";
  std::filesystem::create_directories(tempDir);

  auto pdfPath = tempDir / "doc.pdf";
  auto objPath = tempDir / "doc.o";
  auto txtPath = tempDir / "doc.txt";

  std::ofstream(pdfPath).close();
  std::ofstream(objPath).close();
  std::ofstream(txtPath).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({pdfPath, objPath, txtPath});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("doc"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 3);

  // doc.o should rank lower than doc.pdf and doc.txt
  size_t objFilePos = 0;
  for (size_t i = 0; i < results.size(); ++i) {
    if (results[i].path.filename() == "doc.o") {
      objFilePos = i;
      break;
    }
  }

  REQUIRE(objFilePos > 0); // Not first
}

TEST_CASE("Recent files rank higher", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-recency";
  std::filesystem::create_directories(tempDir);

  auto oldPath = tempDir / "old.txt";
  auto recentPath = tempDir / "recent.txt";

  std::ofstream(oldPath).close();
  std::ofstream(recentPath).close();

  using namespace std::chrono;
  auto now = std::filesystem::file_time_type::clock::now();
  auto twoDaysAgo = now - hours(48);
  auto twoYearsAgo = now - hours(24 * 365 * 2);

  FileIndexerDatabase db(":memory:");
  db.runMigrations();

  db.indexEvents(
      {{FileEventType::Modify, oldPath, twoYearsAgo}, {FileEventType::Modify, recentPath, twoDaysAgo}});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("txt"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 2);
  REQUIRE(results[0].path == recentPath);
}

TEST_CASE("Shallow paths beat deep paths", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-depth";
  std::filesystem::create_directories(tempDir);
  std::filesystem::create_directories(tempDir / "a" / "b" / "c" / "d" / "e");

  auto shallowPath = tempDir / "file.txt";
  auto deepPath = tempDir / "a" / "b" / "c" / "d" / "e" / "file.txt";

  std::ofstream(shallowPath).close();
  std::ofstream(deepPath).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({shallowPath, deepPath});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("file"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 2);
  REQUIRE(results[0].path == shallowPath);
}

TEST_CASE("Hidden files rank lower", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-hidden";
  std::filesystem::create_directories(tempDir);

  auto normalPath = tempDir / "file.txt";
  auto hiddenPath = tempDir / ".file.txt";

  std::ofstream(normalPath).close();
  std::ofstream(hiddenPath).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({normalPath, hiddenPath});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("file"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 2);
  REQUIRE(results[0].path == normalPath);
}

TEST_CASE("Combined ranking factors work together", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-combined";
  std::filesystem::create_directories(tempDir);
  std::filesystem::create_directories(tempDir / "a" / "b" / "c" / "d" / "e");

  // Test file type + path depth ranking:
  // - goodShallow: .pdf (good type) + shallow path
  // - badDeep: .o (bad type) + deep path
  // The combination should amplify the ranking difference
  auto goodShallow = tempDir / "document.pdf";
  auto badDeep = tempDir / "a" / "b" / "c" / "d" / "e" / "document.o";

  std::ofstream(goodShallow).close();
  std::ofstream(badDeep).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({goodShallow, badDeep});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("document"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 2);
  // goodShallow should rank first (good file type + shallow path)
  REQUIRE(results[0].path == goodShallow);
  // badDeep should rank last (bad file type + deep path)
  REQUIRE(results[1].path == badDeep);
}
