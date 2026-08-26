#pragma once
#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <QDateTime>
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

  bool isAvailable() const;

  AbstractFileIndexer *indexer() const;

  void rebuildIndex();

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {});

  std::vector<RecentFile> getRecentlyAccessed() const;
  void saveAccess(const std::filesystem::path &path);
  bool clearRecentlyAccessed();

  void preferenceValuesChanged(const QJsonObject &preferences);

  FileService(OmniDatabase &db);

private:
  struct SerializedRecentFile {
    std::string path;
    int accessCount = 0;
    std::uint64_t lastAccessedAt = 0;
  };

  static constexpr size_t MAX_RECENT_FILES = 50;

  void loadRecentFiles();
  bool saveRecentFiles();
  void migrateRecentFilesFromDatabase();
  static RecentFile fromSerialized(const SerializedRecentFile &file);
  static bool recentFileMoreRecent(const SerializedRecentFile &a, const SerializedRecentFile &b);

  OmniDatabase &m_db;
  std::filesystem::path m_recentFilesPath;
  std::string m_recentFilesBuffer;
  std::vector<SerializedRecentFile> m_recentFiles;
  std::unique_ptr<AbstractFileIndexer> m_indexer;
};
