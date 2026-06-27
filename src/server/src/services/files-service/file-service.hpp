#pragma once
#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <QDateTime>
#include <QJsonObject>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class FileService {

public:
  struct RecentFile {
    QDateTime lastAccessedAt;
    std::filesystem::path path;
    int accessCount = 0;
  };

  AbstractFileIndexer *indexer() const;

  void rebuildIndex();

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {});

  std::vector<RecentFile> getRecentlyAccessed() const;
  void saveAccess(const std::filesystem::path &path);
  void saveQuerySelection(std::string_view query, const std::filesystem::path &path);
  bool clearRecentlyAccessed();

  void preferenceValuesChanged(const QJsonObject &preferences);

  FileService(OmniDatabase &db);

private:
  struct SerializedRecentFile {
    std::string path;
    int accessCount = 0;
    std::uint64_t lastAccessedAt = 0;
  };

  struct QuerySelection {
    std::string query;
    std::string path;
    std::uint64_t selectedAt = 0;
  };

  static constexpr size_t MAX_RECENT_FILES = 50;
  static constexpr size_t MAX_QUERY_SELECTIONS = 2000;

  void loadRecentFiles();
  bool saveRecentFiles();
  void loadQuerySelections();
  bool saveQuerySelections();
  void migrateRecentFilesFromDatabase();
  static RecentFile fromSerialized(const SerializedRecentFile &file);
  static bool recentFileMoreRecent(const SerializedRecentFile &a, const SerializedRecentFile &b);
  static std::optional<std::uint64_t> fileModifiedAt(const std::filesystem::path &path);
  std::optional<std::uint64_t> recentUsageAt(const std::filesystem::path &path) const;
  std::optional<std::uint64_t> queryUsageAt(std::string_view query, const std::filesystem::path &path) const;
  void sortFilesIfRequested(std::vector<IndexerFileResult> &results, std::string_view query) const;

  OmniDatabase &m_db;
  std::filesystem::path m_recentFilesPath;
  std::string m_recentFilesBuffer;
  std::vector<SerializedRecentFile> m_recentFiles;
  std::filesystem::path m_querySelectionsPath;
  std::string m_querySelectionsBuffer;
  std::vector<QuerySelection> m_querySelections;
  std::unique_ptr<AbstractFileIndexer> m_indexer;
  bool m_defaultOrderingEnabled = false;
};
