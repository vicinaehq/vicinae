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
    std::string name;
    double relevancyScore = 0;
    double indexRank = 0;
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

  void deleteAllIndexedFiles();
  void compact();
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
