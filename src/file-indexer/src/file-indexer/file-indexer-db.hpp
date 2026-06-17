#pragma once
#include "file-indexer/db.hpp"
#include "file-indexer/scan.hpp"
#include <cstdint>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

enum class IndexedFileCategory {
  Other,
  Directory,
  Image,
  Video,
  Audio,
  Document,
  Archive,
  Code,
  Application,
};

class FileIndexerDatabase {
  db::Database m_db;

  std::optional<int64_t> retrieveFileId(const std::filesystem::path &path) const;
  std::optional<int64_t> ensureDirectory(const std::filesystem::path &dir);

public:
  struct ScanRecord {
    int id;
    ScanStatus status;
    int64_t createdAt = 0;  // unix seconds
    int64_t finishedAt = 0; // unix seconds, 0 while the scan is running
    int64_t indexedFileCount = 0;
    std::filesystem::path path;
    ScanType type;
  };

  struct SearchCandidate {
    std::filesystem::path path;
    IndexedFileCategory category = IndexedFileCategory::Other;
  };

  struct SearchOptions {
    std::optional<IndexedFileCategory> category;
  };

  ScanRecord mapScan(const db::Statement &stmt) const;
  static std::filesystem::path getDatabasePath();

  std::optional<ScanRecord> getLastScan(const std::filesystem::path &path, ScanType scanType) const;
  std::optional<FileIndexerDatabase::ScanRecord>
  getLastSuccessfulScan(const std::filesystem::path &path) const;

  std::vector<ScanRecord> listScans();
  std::vector<ScanRecord> listScans(ScanType scanType, ScanStatus scanStatus);

  bool updateScanStatus(int scanId, ScanStatus status);
  bool finalizeScan(int scanId, ScanStatus status, int64_t indexedFileCount);
  std::expected<ScanRecord, std::string> createScan(const std::filesystem::path &path, ScanType type);

  bool setScanError(int scanId, const std::string &error);

  std::optional<int64_t> retrieveIndexedLastModified(const std::filesystem::path &path) const;
  std::unordered_set<std::filesystem::path>
  listIndexedDirectoryFiles(const std::filesystem::path &path) const;
  bool tracksFile(const std::filesystem::path &path) const;

  std::vector<std::filesystem::path> listRecentDirectories(int limit) const;

  struct SpellfixSuggestion {
    std::string word;
    int distance = 0;
    int score = 0;
    int64_t rank = 0;
  };

  void deleteAllIndexedFiles();
  void compact();
  void rebuildSpellfixVocabulary();
  bool hasSpellfixVocabulary();
  std::vector<SpellfixSuggestion> spellfixSuggestions(std::string_view word, int top, bool prefix);
  void deleteIndexedFiles(const std::vector<std::filesystem::path> &paths);
  void indexFiles(const std::vector<std::filesystem::path> &paths);
  std::vector<SearchCandidate> searchCandidates(std::string_view searchQuery, int limit,
                                                const SearchOptions &options = {});
  // same query string as searchCandidates: the tokenizer skeletonizes queries itself
  std::vector<SearchCandidate> searchSkeletonCandidates(std::string_view searchQuery, int limit,
                                                        const SearchOptions &options = {});

  int userVersion();
  void setUserVersion(int version);

  void indexEvents(const std::vector<FileEvent> &events);

  void init();

  db::Database &database();

  FileIndexerDatabase();
  ~FileIndexerDatabase() = default;
};
