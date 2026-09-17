#include "file-service.hpp"
#if defined(Q_OS_LINUX)
#include "file-indexer/file-indexer.hpp"
#include "linux/xbel-recent-files-provider.hpp"
#elif defined(Q_OS_MACOS)
#include "macos/spotlight-file-indexer.hpp"
#include "macos/spotlight-recent-files-provider.hpp"
#elif defined(Q_OS_WIN)
#include "windows/win-file-indexer-selector.hpp"
#include "windows/win-recent-files-provider.hpp"
#else
#include "dummy-file-indexer.hpp"
#include "dummy-recent-files-provider.hpp"
#endif

namespace fs = std::filesystem;

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

AbstractRecentFilesProvider *FileService::recentFiles() const { return m_recentFiles.get(); }

QFuture<std::vector<IndexerFileResult>> FileService::queryAsync(std::string_view query,
                                                                const IndexerQueryParams &params) {
  return m_indexer->queryAsync(query, params);
}

void FileService::rebuildIndex() { m_indexer->rebuildIndex(); }

QFuture<std::vector<fs::path>> FileService::recentFilesAsync(const RecentFilesParams &params) {
  return m_recentFiles->listAsync(params);
}

void FileService::recordAccess(const fs::path &path) { m_recentFiles->recordAccess(path); }

void FileService::preferenceValuesChanged(const QJsonObject &preferences) {
  m_indexer->preferenceValuesChanged(preferences);
}

bool FileService::isAvailable() const { return m_indexer->isAvailable(); }

FileService::FileService() {
#if defined(Q_OS_LINUX)
  m_indexer = std::make_unique<FileIndexer>();
  m_recentFiles = std::make_unique<XbelRecentFilesProvider>();
#elif defined(Q_OS_MACOS)
  m_indexer = std::make_unique<SpotlightFileIndexer>();
  m_recentFiles = std::make_unique<SpotlightRecentFilesProvider>();
#elif defined(Q_OS_WIN)
  m_indexer = std::make_unique<WinFileIndexerSelector>();
  m_recentFiles = std::make_unique<WinRecentFilesProvider>();
#else
  m_indexer = std::make_unique<DummyFileIndexer>();
  m_recentFiles = std::make_unique<DummyRecentFilesProvider>();
#endif
}
