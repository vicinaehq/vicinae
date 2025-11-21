#include <catch2/catch_test_macros.hpp>
#include "services/files-service/file-indexer/file-indexer.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <QJsonObject>
#include <filesystem>
#include <thread>
#include <chrono>

TEST_CASE("FileIndexer properly initializes excluded filenames from preferences", FILE_SEARCH_GROUP) {
  FileIndexerDatabase db(":memory:");
  db.runMigrations();

  FileIndexer indexer(std::ref(db));

  // Simulate setting preferences with watcher paths
  QJsonObject preferences;
  preferences["paths"] =
      QString::fromStdString((std::filesystem::temp_directory_path() / "test-index").string());
  preferences["excludedPaths"] = "";
  preferences["watcherPaths"] =
      QString::fromStdString((std::filesystem::temp_directory_path() / "test-watcher").string());

  indexer.preferenceValuesChanged(preferences);

  // The indexer should have set m_excludedFilenames to include database file names
  // This is populated in preferenceValuesChanged() based on FileIndexerDatabase::getDatabasePath()

  // Expected excluded filenames:
  // - vicinae-file-index.db (or whatever getDatabasePath().filename() returns)
  // - vicinae-file-index.db-wal

  REQUIRE(indexer.m_excludedFilenames.size() == 2);

  std::string dbFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  REQUIRE(indexer.m_excludedFilenames[0] == dbFilename);
  REQUIRE(indexer.m_excludedFilenames[1] == dbFilename + "-wal");
}

TEST_CASE("Watcher scans should include excluded filenames to prevent watching database files",
          FILE_SEARCH_GROUP) {
  FileIndexerDatabase db(":memory:");
  db.runMigrations();

  FileIndexer indexer(std::ref(db));

  // Set up preferences that will configure excluded filenames and watcher paths
  QJsonObject preferences;
  auto testWatcherPath = std::filesystem::temp_directory_path() / "test-watcher-exclusion";
  std::filesystem::create_directories(testWatcherPath);

  preferences["paths"] = "";
  preferences["excludedPaths"] = "";
  preferences["watcherPaths"] = QString::fromStdString(testWatcherPath.string());

  indexer.preferenceValuesChanged(preferences);

  std::string dbFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  REQUIRE(indexer.m_excludedFilenames.size() == 2);
  REQUIRE(indexer.m_excludedFilenames[0] == dbFilename);
  REQUIRE(indexer.m_excludedFilenames[1] == dbFilename + "-wal");

  indexer.startFullScan();

  // Wait for the watcher scan to be enqueued (condition-based waiting instead of arbitrary timeout)
  // The scan is enqueued asynchronously in a detached thread after database cleanup completes
  auto startTime = std::chrono::steady_clock::now();
  constexpr auto timeout = std::chrono::seconds(5);

  bool foundWatcherScan = false;
  bool watcherScanHasExcludedFilenames = false;

  while (!foundWatcherScan) {
    auto scans = indexer.m_dispatcher.scans();

    for (const auto &[id, scan] : scans) {
      if (scan.type == ScanType::Watcher && scan.path == testWatcherPath) {
        foundWatcherScan = true;

        if (scan.excludedFilenames.size() == 2 && scan.excludedFilenames[0] == dbFilename &&
            scan.excludedFilenames[1] == dbFilename + "-wal") {
          watcherScanHasExcludedFilenames = true;
        }
        break;
      }
    }

    if (foundWatcherScan) break;

    auto elapsed = std::chrono::steady_clock::now() - startTime;
    if (elapsed > timeout) {
      FAIL("Timeout waiting for watcher scan to be enqueued after "
           << std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count() << "ms");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  indexer.m_dispatcher.interruptAll();
  std::filesystem::remove_all(testWatcherPath);

  REQUIRE(foundWatcherScan);
  REQUIRE(watcherScanHasExcludedFilenames);
}
