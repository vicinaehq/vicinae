#pragma once
#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <filesystem>
#include <string_view>

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
                                                     const AbstractFileIndexer::QueryParams &params = {});

  std::vector<RecentFile> getRecentlyAccessed() const;
  void saveAccess(const std::filesystem::path &path);
  bool clearRecentlyAccessed();

  void preferenceValuesChanged(const QJsonObject &preferences);

  FileService(OmniDatabase &db);

private:
  OmniDatabase &m_db;
  std::unique_ptr<AbstractFileIndexer> m_indexer;
};
