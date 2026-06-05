#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <qlogging.h>
#include "file-service.hpp"
#if defined(Q_OS_LINUX)
#include "file-indexer/file-indexer.hpp"
#elif defined(Q_OS_MACOS)
#include "macos/spotlight-file-indexer.hpp"
#else
#include "dummy-file-indexer.hpp"
#endif

namespace fs = std::filesystem;

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

QFuture<std::vector<IndexerFileResult>>
FileService::queryAsync(std::string_view query, const AbstractFileIndexer::QueryParams &params) {
  return m_indexer->queryAsync(query, params);
}

void FileService::rebuildIndex() { m_indexer->rebuildIndex(); }

void FileService::saveAccess(const fs::path &path) {
  auto stmt = m_db.db().prepare(R"(
    INSERT INTO recent_files(path, access_count) VALUES(:path, 1)
    ON CONFLICT(path)
    DO UPDATE SET
      last_accessed_at = unixepoch(),
      access_count = access_count + 1
  )");
  stmt.bind(":path", path.c_str());

  if (!stmt.exec()) { qWarning() << "Failed to save access for file" << path.c_str(); }
}

bool FileService::clearRecentlyAccessed() {
  if (!m_db.db().exec("DELETE FROM recent_files")) {
    qWarning() << "Failed to clear recently accessed files";
    return false;
  }

  return true;
}

std::vector<FileService::RecentFile> FileService::getRecentlyAccessed() const {
  auto stmt = m_db.db().prepare("SELECT path, access_count, last_accessed_at FROM recent_files ORDER BY "
                                "last_accessed_at DESC LIMIT 50");

  std::vector<FileService::RecentFile> files;

  while (stmt.step()) {
    RecentFile file;

    file.path = stmt.columnText(0);
    file.accessCount = stmt.columnInt(1);
    file.lastAccessedAt = QDateTime::fromSecsSinceEpoch(stmt.columnInt64(2));
    files.emplace_back(file);
  }

  return files;
}

void FileService::preferenceValuesChanged(const QJsonObject &preferences) {
  m_indexer->preferenceValuesChanged(preferences);
}

FileService::FileService(OmniDatabase &db) : m_db(db) {
#if defined(Q_OS_LINUX)
  m_indexer = std::make_unique<FileIndexer>();
#elif defined(Q_OS_MACOS)
  m_indexer = std::make_unique<SpotlightFileIndexer>();
#else
  m_indexer = std::make_unique<DummyFileIndexer>();
#endif
}
