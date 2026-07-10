#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "vicinae.hpp"
#include <algorithm>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qlogging.h>
#include <ranges>
#include "file-service.hpp"
#if defined(Q_OS_LINUX)
#include "file-indexer/file-indexer.hpp"
#elif defined(Q_OS_MACOS)
#include "macos/spotlight-file-indexer.hpp"
#else
#include "dummy-file-indexer.hpp"
#endif

namespace fs = std::filesystem;

constexpr auto RECENT_FILES_NAME = "recent-files.json";

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

QFuture<std::vector<IndexerFileResult>> FileService::queryAsync(std::string_view query,
                                                                const IndexerQueryParams &params) {
  return m_indexer->queryAsync(query, params);
}

void FileService::rebuildIndex() { m_indexer->rebuildIndex(); }

void FileService::saveAccess(const fs::path &path) {
  auto pathString = path.string();
  auto now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  auto it = std::ranges::find_if(m_recentFiles, [&](const auto &file) { return file.path == pathString; });

  if (it == m_recentFiles.end()) {
    m_recentFiles.emplace_back(
        SerializedRecentFile{.path = std::move(pathString), .accessCount = 1, .lastAccessedAt = now});
  } else {
    ++it->accessCount;
    it->lastAccessedAt = now;
  }

  std::ranges::sort(m_recentFiles, recentFileMoreRecent);
  if (m_recentFiles.size() > MAX_RECENT_FILES) { m_recentFiles.resize(MAX_RECENT_FILES); }

  if (!saveRecentFiles()) { qWarning() << "Failed to save recent file access for" << path.c_str(); }
}

bool FileService::clearRecentlyAccessed() {
  m_recentFiles.clear();
  return saveRecentFiles();
}

std::vector<FileService::RecentFile> FileService::getRecentlyAccessed() const {
  return m_recentFiles | std::views::transform(fromSerialized) | std::ranges::to<std::vector>();
}

void FileService::loadRecentFiles() {
  if (!fs::is_regular_file(m_recentFilesPath)) {
    fs::create_directories(m_recentFilesPath.parent_path());
    if (!saveRecentFiles()) {
      qWarning() << "Unable to create recent files state at" << m_recentFilesPath.string();
    }
  }

  if (const auto error = glz::read_file_json(m_recentFiles, m_recentFilesPath.string(), m_recentFilesBuffer)) {
    qWarning() << "Failed to read recent files state at" << m_recentFilesPath.string()
               << glz::format_error(error).c_str();
    m_recentFiles.clear();
  }

  std::ranges::sort(m_recentFiles, recentFileMoreRecent);
  if (m_recentFiles.size() > MAX_RECENT_FILES) {
    m_recentFiles.resize(MAX_RECENT_FILES);
    saveRecentFiles();
  }
}

bool FileService::saveRecentFiles() {
  fs::create_directories(m_recentFilesPath.parent_path());

  if (const auto error =
          glz::write_file_json(m_recentFiles, m_recentFilesPath.string(), m_recentFilesBuffer)) {
    qWarning() << "Failed to save recent files state at" << m_recentFilesPath.string()
               << glz::format_error(error).c_str();
    return false;
  }

  return true;
}

void FileService::migrateRecentFilesFromDatabase() {
  if (!m_recentFiles.empty()) return;

  auto checkStmt =
      m_db.db().prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='recent_files'");

  if (!checkStmt.step()) return;

  auto stmt = m_db.db().prepare(R"(
    SELECT path, access_count, last_accessed_at
    FROM recent_files
    ORDER BY last_accessed_at DESC
    LIMIT :limit
  )");
  stmt.bind(":limit", static_cast<int>(MAX_RECENT_FILES));

  std::vector<SerializedRecentFile> migrated;
  migrated.reserve(MAX_RECENT_FILES);

  while (stmt.step()) {
    migrated.emplace_back(SerializedRecentFile{.path = stmt.columnText(0),
                                               .accessCount = stmt.columnInt(1),
                                               .lastAccessedAt = stmt.columnUInt64(2)});
  }

  if (migrated.empty()) return;

  m_recentFiles = std::move(migrated);
  if (!saveRecentFiles()) {
    m_recentFiles.clear();
    return;
  }

  qInfo() << "Migrated" << m_recentFiles.size() << "recent files from database to state JSON";
}

FileService::RecentFile FileService::fromSerialized(const SerializedRecentFile &file) {
  return RecentFile{.lastAccessedAt = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(file.lastAccessedAt)),
                    .path = file.path,
                    .accessCount = file.accessCount};
}

bool FileService::recentFileMoreRecent(const SerializedRecentFile &a, const SerializedRecentFile &b) {
  return a.lastAccessedAt > b.lastAccessedAt;
}

void FileService::preferenceValuesChanged(const QJsonObject &preferences) {
  m_indexer->preferenceValuesChanged(preferences);
}

bool FileService::isAvailable() const { return m_indexer->isAvailable(); }

FileService::FileService(OmniDatabase &db)
    : m_db(db), m_recentFilesPath(Omnicast::stateDir() / RECENT_FILES_NAME) {
  bool const shouldMigrateRecentFiles = !fs::is_regular_file(m_recentFilesPath);

  loadRecentFiles();
  if (shouldMigrateRecentFiles) { migrateRecentFilesFromDatabase(); }

#if defined(Q_OS_LINUX)
  m_indexer = std::make_unique<FileIndexer>();
#elif defined(Q_OS_MACOS)
  m_indexer = std::make_unique<SpotlightFileIndexer>();
#else
  m_indexer = std::make_unique<DummyFileIndexer>();
#endif
}
