#include "omni-database.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "vicinae.hpp"
#include <algorithm>
#include <chrono>
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
constexpr auto QUERY_SELECTIONS_NAME = "file-query-selections.json";

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

QFuture<std::vector<IndexerFileResult>> FileService::queryAsync(std::string_view query,
                                                                const IndexerQueryParams &params) {
  auto defaultOrdering = m_defaultOrderingEnabled;
  std::string queryText{query};
  return m_indexer->queryAsync(query, params).then(
      [this, defaultOrdering, queryText = std::move(queryText)](std::vector<IndexerFileResult> results) {
        if (defaultOrdering) { sortFilesIfRequested(results, queryText); }
    return results;
  });
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

void FileService::saveQuerySelection(std::string_view query, const fs::path &path) {
  if (query.empty()) return;

  auto pathString = path.string();
  auto now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());
  auto it = std::ranges::find_if(m_querySelections, [&](const auto &selection) {
    return selection.query == query && selection.path == pathString;
  });

  if (it == m_querySelections.end()) {
    m_querySelections.emplace_back(QuerySelection{.query = std::string{query},
                                                  .path = std::move(pathString),
                                                  .selectedAt = now});
  } else {
    it->selectedAt = now;
  }

  std::ranges::sort(m_querySelections, [](const auto &a, const auto &b) { return a.selectedAt > b.selectedAt; });
  if (m_querySelections.size() > MAX_QUERY_SELECTIONS) { m_querySelections.resize(MAX_QUERY_SELECTIONS); }
  if (!saveQuerySelections()) { qWarning() << "Failed to save file query selections for" << path.c_str(); }
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
      qWarning() << "Unable to create recent files state at" << m_recentFilesPath.c_str();
    }
  }

  if (const auto error = glz::read_file_json(m_recentFiles, m_recentFilesPath.c_str(), m_recentFilesBuffer)) {
    qWarning() << "Failed to read recent files state at" << m_recentFilesPath.c_str()
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
          glz::write_file_json(m_recentFiles, m_recentFilesPath.c_str(), m_recentFilesBuffer)) {
    qWarning() << "Failed to save recent files state at" << m_recentFilesPath.c_str()
               << glz::format_error(error).c_str();
    return false;
  }

  return true;
}

void FileService::loadQuerySelections() {
  if (!fs::is_regular_file(m_querySelectionsPath)) {
    fs::create_directories(m_querySelectionsPath.parent_path());
    if (!saveQuerySelections()) {
      qWarning() << "Unable to create file query selections state at" << m_querySelectionsPath.c_str();
    }
  }

  if (const auto error = glz::read_file_json(m_querySelections, m_querySelectionsPath.c_str(),
                                             m_querySelectionsBuffer)) {
    qWarning() << "Failed to read file query selections state at" << m_querySelectionsPath.c_str()
               << glz::format_error(error).c_str();
    m_querySelections.clear();
  }

  std::ranges::sort(m_querySelections, [](const auto &a, const auto &b) { return a.selectedAt > b.selectedAt; });
  if (m_querySelections.size() > MAX_QUERY_SELECTIONS) {
    m_querySelections.resize(MAX_QUERY_SELECTIONS);
    saveQuerySelections();
  }
}

bool FileService::saveQuerySelections() {
  fs::create_directories(m_querySelectionsPath.parent_path());

  if (const auto error = glz::write_file_json(m_querySelections, m_querySelectionsPath.c_str(),
                                              m_querySelectionsBuffer)) {
    qWarning() << "Failed to save file query selections state at" << m_querySelectionsPath.c_str()
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

std::optional<std::uint64_t> FileService::fileModifiedAt(const std::filesystem::path &path) {
  std::error_code ec;
  auto const fileTime = std::filesystem::last_write_time(path, ec);
  if (ec) return std::nullopt;

  using namespace std::chrono;
  auto const nowSystem = system_clock::now();
  auto const nowFile = std::filesystem::file_time_type::clock::now();
  auto const adjusted = time_point_cast<system_clock::duration>(fileTime - nowFile + nowSystem);
  return static_cast<std::uint64_t>(duration_cast<seconds>(adjusted.time_since_epoch()).count());
}

std::optional<std::uint64_t> FileService::recentUsageAt(const std::filesystem::path &path) const {
  auto const pathString = path.string();
  auto it = std::ranges::find_if(m_recentFiles, [&](const auto &file) { return file.path == pathString; });
  if (it == m_recentFiles.end()) return std::nullopt;
  return it->lastAccessedAt;
}

std::optional<std::uint64_t> FileService::queryUsageAt(std::string_view query,
                                                       const std::filesystem::path &path) const {
  if (query.empty()) return std::nullopt;

  auto const pathString = path.string();
  auto it = std::ranges::find_if(m_querySelections, [&](const auto &selection) {
    return selection.query == query && selection.path == pathString;
  });
  if (it == m_querySelections.end()) return std::nullopt;
  return it->selectedAt;
}

void FileService::sortFilesIfRequested(std::vector<IndexerFileResult> &results, std::string_view query) const {
  std::ranges::stable_sort(results, [this, query](const auto &a, const auto &b) {
    auto const aQueryUsed = queryUsageAt(query, a.path);
    auto const bQueryUsed = queryUsageAt(query, b.path);
    if (aQueryUsed != bQueryUsed) { return aQueryUsed.has_value(); }
    if (aQueryUsed && bQueryUsed && aQueryUsed != bQueryUsed) { return aQueryUsed > bQueryUsed; }

    auto const aUsed = recentUsageAt(a.path);
    auto const bUsed = recentUsageAt(b.path);
    if (aUsed != bUsed) { return aUsed.has_value(); }
    if (aUsed && bUsed && aUsed != bUsed) { return aUsed > bUsed; }

    auto const aModified = fileModifiedAt(a.path);
    auto const bModified = fileModifiedAt(b.path);
    if (aModified != bModified) { return aModified.has_value(); }
    if (aModified && bModified && aModified != bModified) { return aModified > bModified; }

    return a.path < b.path;
  });
}

void FileService::preferenceValuesChanged(const QJsonObject &preferences) {
  m_defaultOrderingEnabled = preferences.value("defaultOrdering").toBool(false);
  m_indexer->preferenceValuesChanged(preferences);
}

FileService::FileService(OmniDatabase &db)
    : m_db(db), m_recentFilesPath(Omnicast::stateDir() / RECENT_FILES_NAME),
      m_querySelectionsPath(Omnicast::stateDir() / QUERY_SELECTIONS_NAME) {
  bool const shouldMigrateRecentFiles = !fs::is_regular_file(m_recentFilesPath);

  loadRecentFiles();
  loadQuerySelections();
  if (shouldMigrateRecentFiles) { migrateRecentFilesFromDatabase(); }

#if defined(Q_OS_LINUX)
  m_indexer = std::make_unique<FileIndexer>();
#elif defined(Q_OS_MACOS)
  m_indexer = std::make_unique<SpotlightFileIndexer>();
#else
  m_indexer = std::make_unique<DummyFileIndexer>();
#endif
}
