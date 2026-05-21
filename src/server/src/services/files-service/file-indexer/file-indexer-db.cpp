#include "file-indexer-db.hpp"
#include "scan.hpp"
#include "vicinae.hpp"
#include "services/files-service/file-indexer/relevancy-scorer.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "utils/utils.hpp"
#include <chrono>
#include <format>
#include <qlogging.h>

static constexpr const char *SQLITE_PRAGMAS[] = {
    "PRAGMA journal_mode = WAL",
    "PRAGMA synchronous = normal",
    "PRAGMA temp_store = memory",
};

namespace fs = std::filesystem;

fs::path FileIndexerDatabase::getDatabasePath() { return Omnicast::dataDir() / "file-indexer.db"; }

std::optional<QDateTime>
FileIndexerDatabase::retrieveIndexedLastModified(const std::filesystem::path &path) const {
  auto stmt = m_db.prepare("SELECT last_modified_at FROM indexed_file WHERE path = :path");
  stmt.bind(":path", path.c_str());

  if (!stmt.step()) { return std::nullopt; }

  return QDateTime::fromSecsSinceEpoch(stmt.columnUInt64(0));
}

std::vector<std::filesystem::path>
FileIndexerDatabase::listIndexedDirectoryFiles(const std::filesystem::path &path) const {
  auto stmt = m_db.prepare("SELECT path, last_modified_at FROM indexed_file WHERE parent_path = :path");
  stmt.bind(":path", path.c_str());

  std::vector<fs::path> paths;

  while (stmt.step()) {
    paths.emplace_back(stmt.columnText(0));
  }

  return paths;
}

void FileIndexerDatabase::runMigrations() {
  MigrationManager manager(m_db, "file-indexer");
  manager.runMigrations();
}

bool FileIndexerDatabase::setScanError(int scanId, const QString &error) {
  auto stmt = m_db.prepare("UPDATE scan_history SET status = :status, error = :error WHERE id = :id");
  stmt.bind(":id", scanId);
  stmt.bind(":status", static_cast<int>(ScanStatus::Failed));
  stmt.bind(":error", error);

  if (!stmt.exec()) {
    qWarning() << "Failed to update scan status" << stmt.lastError().c_str();
    return false;
  }

  return true;
}

bool FileIndexerDatabase::updateScanStatus(int scanId, ScanStatus status) {
  auto stmt = m_db.prepare("UPDATE scan_history SET status = :status WHERE id = :id");
  stmt.bind(":id", scanId);
  stmt.bind(":status", static_cast<int>(status));

  if (!stmt.exec()) {
    qWarning() << "Failed to update scan status" << stmt.lastError().c_str();
    return false;
  }

  return true;
}

std::expected<FileIndexerDatabase::ScanRecord, QString>
FileIndexerDatabase::createScan(const std::filesystem::path &path, ScanType type) {
  auto stmt =
      m_db.prepare("INSERT INTO scan_history (entrypoint, type, status) VALUES (:entrypoint, :type, :status) "
                   "RETURNING id, status, created_at, entrypoint");
  stmt.bind(":entrypoint", path.c_str());
  stmt.bind(":status", static_cast<int>(ScanStatus::Pending));
  stmt.bind(":type", static_cast<int>(type));

  if (!stmt.step()) {
    qWarning() << "Failed to create scan history";
    return std::unexpected(QString("Failed to create scan history"));
  }

  ScanRecord record;
  record.id = stmt.columnInt(0);
  record.status = static_cast<ScanStatus>(stmt.columnInt(1));
  record.createdAt = QDateTime::fromSecsSinceEpoch(stmt.columnUInt64(2));
  record.path = path;

  return record;
}

FileIndexerDatabase::ScanRecord FileIndexerDatabase::mapScan(const db::Statement &stmt) const {
  ScanRecord record;

  record.id = stmt.columnInt(0);
  record.status = static_cast<ScanStatus>(stmt.columnInt(1));
  record.createdAt = QDateTime::fromSecsSinceEpoch(stmt.columnUInt64(2));
  record.path = stmt.columnText(3);
  record.type = static_cast<ScanType>(stmt.columnInt(4));

  return record;
}

std::optional<FileIndexerDatabase::ScanRecord>
FileIndexerDatabase::getLastSuccessfulScan(const std::filesystem::path &path) const {
  auto stmt = m_db.prepare("SELECT id, status, created_at, entrypoint, type "
                           "FROM scan_history "
                           "WHERE type in (:type1, :type2) "
                           "AND status = :status "
                           "AND entrypoint = :entrypoint "
                           "ORDER BY created_at "
                           "DESC LIMIT 1");

  stmt.bind(":type1", static_cast<int>(ScanType::Full));
  stmt.bind(":type2", static_cast<int>(ScanType::Incremental));
  stmt.bind(":status", static_cast<int>(ScanStatus::Succeeded));
  stmt.bind(":entrypoint", path.c_str());

  if (!stmt.step()) return std::nullopt;

  return mapScan(stmt);
}

std::optional<FileIndexerDatabase::ScanRecord>
FileIndexerDatabase::getLastScan(const std::filesystem::path &path, ScanType scanType) const {
  auto stmt = m_db.prepare("SELECT id, status, created_at, entrypoint, type "
                           "FROM scan_history "
                           "WHERE type = :type "
                           "AND entrypoint = :entrypoint "
                           "ORDER BY created_at "
                           "DESC LIMIT 1");

  stmt.bind(":type", static_cast<int>(scanType));
  stmt.bind(":entrypoint", path.c_str());

  if (!stmt.step()) return std::nullopt;

  return mapScan(stmt);
}

std::vector<FileIndexerDatabase::ScanRecord> FileIndexerDatabase::listScans() {
  auto stmt = m_db.prepare("SELECT id, status, created_at, entrypoint, type FROM scan_history");

  std::vector<ScanRecord> records;
  records.reserve(0xF);

  while (stmt.step()) {
    records.emplace_back(mapScan(stmt));
  }

  return records;
}

std::vector<FileIndexerDatabase::ScanRecord> FileIndexerDatabase::listScans(ScanType scanType,
                                                                            ScanStatus scanStatus) {
  auto stmt = m_db.prepare("SELECT id, status, created_at, entrypoint, type "
                           "FROM scan_history WHERE status = :status and type = :type");
  stmt.bind(":status", static_cast<int>(ScanStatus::Started));
  stmt.bind(":type", static_cast<int>(scanType));

  std::vector<ScanRecord> records;
  records.reserve(0xF);

  while (stmt.step()) {
    records.emplace_back(mapScan(stmt));
  }

  return records;
}

db::Database &FileIndexerDatabase::database() { return m_db; }

std::vector<FileIndexerDatabase::SearchCandidate>
FileIndexerDatabase::searchCandidates(std::string_view searchQuery, int limit) {
  if (searchQuery.empty() || limit <= 0) return {};

  auto stmt = m_db.prepare(R"(
    SELECT f.path, f.name, f.relevancy_score, unicode_idx.rank
    FROM indexed_file f
    JOIN unicode_idx ON unicode_idx.rowid = f.id
    WHERE unicode_idx MATCH :search
    ORDER BY unicode_idx.rank, f.relevancy_score DESC, f.path
    LIMIT :limit
  )");
  stmt.bind(":search", searchQuery);
  stmt.bind(":limit", limit);

  std::vector<SearchCandidate> results;
  std::error_code ec;

  results.reserve(limit);

  while (stmt.step()) {
    SearchCandidate candidate;

    candidate.path = stmt.columnText(0);
    if (!fs::exists(candidate.path, ec)) { continue; }

    candidate.name = stmt.columnText(1);
    candidate.relevancyScore = stmt.columnDouble(2);
    candidate.indexRank = stmt.columnDouble(3);
    results.emplace_back(std::move(candidate));
  }

  return results;
}

void FileIndexerDatabase::deleteIndexedFiles(const std::vector<fs::path> &paths) {
  auto tx = m_db.transaction();

  auto stmt = m_db.prepare("DELETE FROM indexed_file WHERE path = :path");

  for (const auto &path : paths) {
    stmt.bind(":path", path.c_str());

    if (!stmt.exec()) {
      qCritical() << "Failed to delete indexed file" << path.c_str();
      tx.rollback();
      return;
    }
  }

  if (!tx.commit()) { qCritical() << "Failed to commit"; }
}

void FileIndexerDatabase::deleteAllIndexedFiles() {
  if (!m_db.exec("DELETE FROM indexed_file")) {
    qCritical() << "Failed to delete all indexed files" << m_db.lastError().c_str();
  }
}

void FileIndexerDatabase::compact() {
  if (!m_db.exec("VACUUM")) {
    qWarning() << "VACUUM failed" << m_db.lastError().c_str();
    return;
  }

  if (!m_db.exec("PRAGMA wal_checkpoint(TRUNCATE)")) {
    qWarning() << "wal_checkpoint(TRUNCATE) failed" << m_db.lastError().c_str();
  }
}

void FileIndexerDatabase::indexEvents(const std::vector<FileEvent> &events) {
  auto tx = m_db.transaction();

  auto modifyStmt = m_db.prepare(R"(
    INSERT INTO
      indexed_file (path, parent_path, name, last_modified_at, relevancy_score)
    VALUES
      (:path, :parent_path, :name, :last_modified_at, :relevancy_score)
    ON CONFLICT (path) DO UPDATE SET last_modified_at = :last_modified_at
  )");

  auto deleteStmt = m_db.prepare("DELETE FROM indexed_file WHERE path = :path");

  db::Statement *activeStmt = nullptr;

  for (const auto &event : events) {
    switch (event.type) {
    case FileEventType::Modify: {
      using namespace std::chrono;
      auto const secondsSinceEpoch =
          static_cast<int64_t>(duration_cast<seconds>(event.eventTime.time_since_epoch()).count());
      modifyStmt.bind(":last_modified_at", secondsSinceEpoch);
      modifyStmt.bind(":path", event.path.c_str());
      modifyStmt.bind(":parent_path", event.path.parent_path().c_str());
      modifyStmt.bind(":name", event.path.filename().c_str());

      RelevancyScorer scorer;
      modifyStmt.bind(":relevancy_score", scorer.computeScore(event.path, event.eventTime));

      activeStmt = &modifyStmt;
      break;
    }

    case FileEventType::Delete: {
      deleteStmt.bind(":path", event.path.c_str());
      activeStmt = &deleteStmt;
      break;
    }
    }

    if (!activeStmt->exec()) {
      static std::map<FileEventType, std::string> verbMap = {{FileEventType::Delete, "deleted"},
                                                             {FileEventType::Modify, "modified"}};
      qCritical() << "Failed to index" << verbMap[event.type] << "file" << event.path
                  << activeStmt->lastError().c_str();
      tx.rollback();
      return;
    }
  }

  if (!tx.commit()) { qCritical() << "Failed to commit"; }
}

void FileIndexerDatabase::indexFiles(const std::vector<std::filesystem::path> &paths) {
  auto tx = m_db.transaction();

  auto stmt = m_db.prepare(R"(
    INSERT INTO
      indexed_file (path, parent_path, name, last_modified_at, relevancy_score)
    VALUES
      (:path, :parent_path, :name, :last_modified_at, :relevancy_score)
    ON CONFLICT (path) DO UPDATE SET last_modified_at = :last_modified_at
  )");

  std::error_code ec;
  RelevancyScorer scorer;
  double score = 1.0;

  for (const auto &path : paths) {
    if (auto lastModified = fs::last_write_time(path, ec); !ec) {
      using namespace std::chrono;
      auto sctp = clock_cast<system_clock>(lastModified);
      auto const epoch = static_cast<int64_t>(duration_cast<seconds>(sctp.time_since_epoch()).count());

      stmt.bind(":last_modified_at", epoch);
      score = scorer.computeScore(path, lastModified);
    } else {
      stmt.bindNull(":last_modified_at");
      score = scorer.computeScore(path, std::nullopt);
    }

    stmt.bind(":path", path.c_str());
    stmt.bind(":parent_path", path.parent_path().c_str());
    stmt.bind(":name", path.filename().c_str());
    stmt.bind(":relevancy_score", score);

    if (!stmt.exec()) {
      qCritical() << "Failed to insert file in index" << path << stmt.lastError().c_str();
      tx.rollback();
      return;
    }
  }

  if (!tx.commit()) { qCritical() << "Failed to commit batchIndex" << m_db.lastError().c_str(); }
}

FileIndexerDatabase::FileIndexerDatabase() {
  auto result = db::Database::open(getDatabasePath());

  if (!result) {
    qCritical() << "Failed to open database at" << getDatabasePath().c_str();
    return;
  }

  m_db = std::move(*result);

  for (const auto &pragma : SQLITE_PRAGMAS) {
    if (!m_db.exec(pragma)) { qCritical() << "Failed to run file-indexer pragma" << pragma; }
  }
}
