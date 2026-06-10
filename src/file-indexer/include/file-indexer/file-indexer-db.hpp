#pragma once
#include "file-indexer/db.hpp"
#include "file-indexer/scan.hpp"
#include <cstdint>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class FileIndexerDatabase {
  db::Database m_db;

  std::optional<int64_t> retrieveFileId(const std::filesystem::path &path) const;
  std::optional<int64_t> ensureDirectory(const std::filesystem::path &dir);

public:
  struct ScanRecord {
    int id;
    ScanStatus status;
    int64_t createdAt; // unix seconds
    std::filesystem::path path;
    ScanType type;
  };

  struct SearchCandidate {
    std::filesystem::path path;
  };

  ScanRecord mapScan(const db::Statement &stmt) const;
  static std::filesystem::path getDatabasePath();

  std::optional<ScanRecord> getLastScan(const std::filesystem::path &path, ScanType scanType) const;
  std::optional<FileIndexerDatabase::ScanRecord>
  getLastSuccessfulScan(const std::filesystem::path &path) const;

  std::vector<ScanRecord> listScans();
  std::vector<ScanRecord> listScans(ScanType scanType, ScanStatus scanStatus);

  bool updateScanStatus(int scanId, ScanStatus status);
  std::expected<ScanRecord, std::string> createScan(const std::filesystem::path &path, ScanType type);

  bool setScanError(int scanId, const std::string &error);

  std::optional<int64_t> retrieveIndexedLastModified(const std::filesystem::path &path) const;
  std::vector<std::filesystem::path> listIndexedDirectoryFiles(const std::filesystem::path &path) const;

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
  std::vector<SearchCandidate> searchCandidates(std::string_view searchQuery, int limit);

  int userVersion();
  void setUserVersion(int version);

  void indexEvents(const std::vector<FileEvent> &events);

  void init();

  db::Database &database();

  FileIndexerDatabase();
  ~FileIndexerDatabase() = default;
};
