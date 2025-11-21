#include <catch2/catch_test_macros.hpp>
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/file-indexer.hpp"
#include "test-helpers.hpp"
#include <filesystem>
#include <fstream>

// Helper to create test files
auto createTestFiles(const std::filesystem::path &tempDir, const std::vector<std::string> &names) {
  std::filesystem::create_directories(tempDir);
  std::vector<std::filesystem::path> paths;
  for (const auto &name : names) {
    auto path = tempDir / name;
    if (name.back() == '/') {
      std::filesystem::create_directory(path);
    } else {
      std::ofstream(path).close();
    }
    paths.push_back(path);
  }
  return paths;
}

TEST_CASE("Multi-word queries find best matches", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-multi";
  auto paths = createTestFiles(tempDir, {"project-readme.md", "project.txt", "readme-old.md"});

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles(paths);

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("project readme"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(!results.empty());
  REQUIRE(results[0].path == paths[0]);
}

TEST_CASE("Queries with special characters are handled correctly", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-special";
  auto paths = createTestFiles(tempDir, {"config.json", "config-backup.json"});

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles(paths);

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("config.json"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() >= 1);
  REQUIRE(results[0].path == paths[0]);
}

TEST_CASE("Case insensitive search works correctly", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-case";
  auto paths = createTestFiles(tempDir, {"readme.md", "ReadMe.txt", "README.markdown"});

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles(paths);

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("README"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 3);
}

TEST_CASE("Partial filename matching respects boundaries", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-partial";
  auto paths = createTestFiles(tempDir, {"main.cpp", "maintain.cpp", "domain.cpp"});

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles(paths);

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("main"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 2);

  bool foundMain = false;
  bool foundMaintain = false;
  bool foundDomain = false;

  for (const auto &result : results) {
    if (result.path.filename() == "main.cpp") foundMain = true;
    if (result.path.filename() == "maintain.cpp") foundMaintain = true;
    if (result.path.filename() == "domain.cpp") foundDomain = true;
  }

  REQUIRE(foundMain);
  REQUIRE(foundMaintain);
  REQUIRE(!foundDomain);
}

TEST_CASE("No results for non-existent files", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-noresults";
  auto paths = createTestFiles(tempDir, {"document.pdf", "notes.txt"});

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles(paths);

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("nonexistent"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.empty());
}

TEST_CASE("Empty database returns no results", FILE_SEARCH_GROUP) {
  FileIndexerDatabase db(":memory:");
  db.runMigrations();

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("anything"));

  REQUIRE(results.empty());
}

TEST_CASE("Database with files but query has no matches", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-nomatch";
  std::filesystem::create_directories(tempDir);

  auto path = tempDir / "document.pdf";
  std::ofstream(path).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({path});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("xyz123nonexistent"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.empty());
}

TEST_CASE("Very long query is handled gracefully", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-longquery";
  std::filesystem::create_directories(tempDir);

  auto path = tempDir / "test.txt";
  std::ofstream(path).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({path});

  FileIndexer indexer(std::ref(db));

  // 1000 characters
  std::string longQuery(1000, 'a');
  auto results = waitForSearchResults(indexer.queryAsync(longQuery));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.empty());
}

TEST_CASE("Query with only special characters returns no results", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-special";
  std::filesystem::create_directories(tempDir);

  auto path = tempDir / "test.txt";
  std::ofstream(path).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({path});

  FileIndexer indexer(std::ref(db));
  auto results = waitForSearchResults(indexer.queryAsync("!@#$%^&*()"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.empty());
}

TEST_CASE("Multiple sequential queries on same database work correctly", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test-sequential";
  std::filesystem::create_directories(tempDir);

  auto path1 = tempDir / "document.pdf";
  auto path2 = tempDir / "test.txt";

  std::ofstream(path1).close();
  std::ofstream(path2).close();

  FileIndexerDatabase db(":memory:");
  db.runMigrations();
  db.indexFiles({path1, path2});

  FileIndexer indexer(std::ref(db));

  auto results1 = waitForSearchResults(indexer.queryAsync("document"));
  auto results2 = waitForSearchResults(indexer.queryAsync("test"));
  auto results3 = waitForSearchResults(indexer.queryAsync("pdf"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results1[0].path == path1);
  REQUIRE(results2[0].path == path2);
  REQUIRE(results3[0].path == path1);
}

TEST_CASE("Single word prefix search finds matching files", FILE_SEARCH_GROUP) {
  auto tempDir = std::filesystem::temp_directory_path() / "vicinae-test";
  std::filesystem::create_directories(tempDir);

  auto docPath = tempDir / "document.pdf";
  auto docsPath = tempDir / "docs";
  auto picturesPath = tempDir / "pictures";

  std::ofstream(docPath).close(); // Create empty file
  std::filesystem::create_directory(docsPath);
  std::filesystem::create_directory(picturesPath);

  FileIndexerDatabase db(":memory:");
  db.runMigrations();

  db.indexFiles({docPath, docsPath, picturesPath});

  FileIndexer indexer(std::ref(db));

  auto results = waitForSearchResults(indexer.queryAsync("doc"));

  std::filesystem::remove_all(tempDir);

  REQUIRE(results.size() == 2);

  bool foundDocument = false;
  bool foundDocs = false;

  for (const auto &result : results) {
    if (result.path == docPath) foundDocument = true;
    if (result.path == docsPath) foundDocs = true;
  }

  REQUIRE(foundDocument);
  REQUIRE(foundDocs);
}
