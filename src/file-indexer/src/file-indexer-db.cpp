#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/relevancy-scorer.hpp"
#include "file-indexer/migrations.hpp"
#include "file-indexer/util.hpp"
#include "file-indexer/log.hpp"
#include <chrono>
#include <filesystem>
#include <map>
#include <string>

static constexpr const char *SQLITE_PRAGMAS[] = {
    "PRAGMA journal_mode = WAL",
    "PRAGMA synchronous = normal",
    "PRAGMA temp_store = memory",
};

namespace fs = std::filesystem;

fs::path FileIndexerDatabase::getDatabasePath() { return file_indexer::databasePath(); }

std::optional<int64_t>
FileIndexerDatabase::retrieveIndexedLastModified(const std::filesystem::path &path) const {
  auto stmt = m_db.prepare("SELECT last_modified_at FROM indexed_file WHERE path = :path");
  stmt.bind(":path", path.c_str());

  if (!stmt.step()) { return std::nullopt; }

  return static_cast<int64_t>(stmt.columnUInt64(0));
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

void FileIndexerDatabase::init() {
  if (!m_db.exec(std::string(file_indexer::INIT_SQL))) {
    flog::error() << "Failed to run file-indexer migrations" << m_db.lastError();
    return;
  }
}

bool FileIndexerDatabase::setScanError(int scanId, const std::string &error) {
  auto stmt = m_db.prepare("UPDATE scan_history SET status = :status, error = :error WHERE id = :id");
  stmt.bind(":id", scanId);
  stmt.bind(":status", static_cast<int>(ScanStatus::Failed));
  stmt.bind(":error", error);

  if (!stmt.exec()) {
    flog::warn() << "Failed to update scan status" << stmt.lastError();
    return false;
  }

  return true;
}

bool FileIndexerDatabase::updateScanStatus(int scanId, ScanStatus status) {
  auto stmt = m_db.prepare("UPDATE scan_history SET status = :status WHERE id = :id");
  stmt.bind(":id", scanId);
  stmt.bind(":status", static_cast<int>(status));

  if (!stmt.exec()) {
    flog::warn() << "Failed to update scan status" << stmt.lastError();
    return false;
  }

  return true;
}

std::expected<FileIndexerDatabase::ScanRecord, std::string>
FileIndexerDatabase::createScan(const std::filesystem::path &path, ScanType type) {
  auto stmt =
      m_db.prepare("INSERT INTO scan_history (entrypoint, type, status) VALUES (:entrypoint, :type, :status) "
                   "RETURNING id, status, created_at, entrypoint");
  stmt.bind(":entrypoint", path.c_str());
  stmt.bind(":status", static_cast<int>(ScanStatus::Pending));
  stmt.bind(":type", static_cast<int>(type));

  if (!stmt.step()) {
    flog::warn() << "Failed to create scan history";
    return std::unexpected(std::string("Failed to create scan history"));
  }

  ScanRecord record;
  record.id = stmt.columnInt(0);
  record.status = static_cast<ScanStatus>(stmt.columnInt(1));
  record.createdAt = static_cast<int64_t>(stmt.columnUInt64(2));
  record.path = path;

  return record;
}

FileIndexerDatabase::ScanRecord FileIndexerDatabase::mapScan(const db::Statement &stmt) const {
  ScanRecord record;

  record.id = stmt.columnInt(0);
  record.status = static_cast<ScanStatus>(stmt.columnInt(1));
  record.createdAt = static_cast<int64_t>(stmt.columnUInt64(2));
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
    SELECT f.path, f.relevancy_score, unicode_idx.rank
    FROM indexed_file f
    JOIN unicode_idx ON unicode_idx.rowid = f.id
    WHERE unicode_idx MATCH :search
    ORDER BY unicode_idx.rank, f.relevancy_score DESC
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

    candidate.relevancyScore = stmt.columnDouble(1);
    candidate.indexRank = stmt.columnDouble(2);
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
      flog::error() << "Failed to delete indexed file" << path.c_str();
      tx.rollback();
      return;
    }
  }

  if (!tx.commit()) { flog::error() << "Failed to commit"; }
}

void FileIndexerDatabase::deleteAllIndexedFiles() {
  if (!m_db.exec("DELETE FROM indexed_file")) {
    flog::error() << "Failed to delete all indexed files" << m_db.lastError();
  }
}

void FileIndexerDatabase::compact() {
  if (!m_db.exec("VACUUM")) {
    flog::warn() << "VACUUM failed" << m_db.lastError();
    return;
  }

  if (!m_db.exec("PRAGMA wal_checkpoint(TRUNCATE)")) {
    flog::warn() << "wal_checkpoint(TRUNCATE) failed" << m_db.lastError();
  }
}

void FileIndexerDatabase::indexEvents(const std::vector<FileEvent> &events) {

  flog::info() << "Indexing " << events.size() << " events\n";

  auto tx = m_db.transaction();

  auto modifyStmt = m_db.prepare(R"(
    INSERT INTO
      indexed_file (path, parent_path, last_modified_at, relevancy_score)
    VALUES
      (:path, :parent_path, :last_modified_at, :relevancy_score)
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
      flog::error() << "Failed to index" << verbMap[event.type] << "file" << event.path.string()
                    << activeStmt->lastError();
      tx.rollback();
      return;
    }
  }

  if (!tx.commit()) { flog::error() << "Failed to commit"; }
}

void FileIndexerDatabase::indexFiles(const std::vector<std::filesystem::path> &paths) {
  flog::info() << "Indexing " << paths.size() << " files\n";
  auto tx = m_db.transaction();

  auto stmt = m_db.prepare(R"(
    INSERT INTO
      indexed_file (path, parent_path, last_modified_at, relevancy_score)
    VALUES
      (:path, :parent_path, :last_modified_at, :relevancy_score)
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
    stmt.bind(":relevancy_score", score);

    if (!stmt.exec()) {
      flog::error() << "Failed to insert file in index" << path.string() << stmt.lastError();
      tx.rollback();
      return;
    }
  }

  if (!tx.commit()) { flog::error() << "Failed to commit batchIndex" << m_db.lastError(); }
}

int FileIndexerDatabase::userVersion() {
  auto stmt = m_db.prepare("PRAGMA user_version;");
  if (!stmt.step()) return 0;
  return stmt.columnInt(0);
}

void FileIndexerDatabase::setUserVersion(int version) {
  if (!m_db.exec(std::format("PRAGMA user_version = {};", version))) {
    flog::warn() << "Failed to set user version to " << version << ": " << m_db.lastError() << "\n";
  }
}

FileIndexerDatabase::FileIndexerDatabase() {
  std::error_code ec;
  fs::create_directories(file_indexer::dataDir(), ec);

  auto result = db::Database::open(getDatabasePath());

  if (!result) {
    flog::error() << "Failed to open database at" << getDatabasePath().c_str();
    return;
  }

  m_db = std::move(*result);

  for (const auto &pragma : SQLITE_PRAGMAS) {
    if (!m_db.exec(pragma)) { flog::error() << "Failed to run file-indexer pragma" << pragma; }
  }
}
