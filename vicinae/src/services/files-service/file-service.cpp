#include "file-indexer/file-indexer.hpp"
#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <qlogging.h>
#include "file-service.hpp"

namespace fs = std::filesystem;

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

QFuture<std::vector<IndexerFileResult>>
FileService::queryAsync(std::string_view query, const AbstractFileIndexer::QueryParams &params) {
  return m_indexer->queryAsync(query, params);
}

void FileService::rebuildIndex() { m_indexer->rebuildIndex(); }

void FileService::saveAccess(const fs::path &path) {
  auto query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO recent_files(path, access_count) VALUES(:path, 1)
		ON CONFLICT(path) 
		DO UPDATE SET 
			last_accessed_at = unixepoch(), 
			access_count = access_count + 1
  )");
  query.bindValue(":path", path.c_str());

  if (!query.exec()) { qWarning() << "Failed to save access for file" << path.c_str(); }
}

bool FileService::clearRecentlyAccessed() {
  auto query = m_db.createQuery();

  if (!query.exec("DELETE FROM recent_files")) {
    qWarning() << "Failed to clear recently accessed files";
    return false;
  }

  return true;
}

std::vector<FileService::RecentFile> FileService::getRecentlyAccessed() const {
  auto query = m_db.createQuery();

  if (!query.exec("SELECT path, access_count, last_accessed_at FROM recent_files ORDER BY last_accessed_at "
                  "DESC LIMIT 50")) {
    qWarning() << "Failed to get recently accessed files";
    return {};
  }

  std::vector<FileService::RecentFile> files;

  while (query.next()) {
    RecentFile file;

    file.path = query.value(0).toString().toStdString();
    file.accessCount = query.value(1).toInt();
    file.lastAccessedAt = QDateTime::fromSecsSinceEpoch(query.value(2).toULongLong());
    files.emplace_back(file);
  }

  return files;
}

void FileService::preferenceValuesChanged(const QJsonObject &preferences) {
  m_indexer->preferenceValuesChanged(preferences);
}

FileService::FileService(OmniDatabase &db) : m_db(db) { m_indexer = std::make_unique<FileIndexer>(); }
