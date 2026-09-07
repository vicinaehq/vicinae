#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/abstract-recent-files-provider.hpp"
#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

class FileService {

public:
  bool isAvailable() const;

  AbstractFileIndexer *indexer() const;
  AbstractRecentFilesProvider *recentFiles() const;

  void rebuildIndex();

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {});

  QFuture<std::vector<std::filesystem::path>> recentFilesAsync(const RecentFilesParams &params = {});
  void recordAccess(const std::filesystem::path &path);

  void preferenceValuesChanged(const QJsonObject &preferences);

  FileService();

private:
  std::unique_ptr<AbstractFileIndexer> m_indexer;
  std::unique_ptr<AbstractRecentFilesProvider> m_recentFiles;
};
