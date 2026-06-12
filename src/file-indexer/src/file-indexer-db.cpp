#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/migrations.hpp"
#include "file-indexer/util.hpp"
#include "file-indexer/vocabulary.hpp"
#include "file-indexer/log.hpp"
#include <cctype>
#include <chrono>
#include <filesystem>
#include <format>
#include <string>
#include <unordered_map>
#include <utility>

static constexpr const char *SQLITE_PRAGMAS[] = {
    "PRAGMA journal_mode = WAL",
    "PRAGMA synchronous = normal",
    "PRAGMA temp_store = memory",
};

namespace fs = std::filesystem;

namespace {

// [p + '/', p + ('/' + 1)) covers exactly the byte range of p's descendants
std::pair<std::string, std::string> subtreeRange(const fs::path &path) {
  std::string lower = path.native() + '/';
  std::string upper = path.native() + static_cast<char>('/' + 1);
  return {std::move(lower), std::move(upper)};
}

} // namespace

fs::path FileIndexerDatabase::getDatabasePath() { return file_indexer::databasePath(); }

std::optional<int64_t> FileIndexerDatabase::retrieveFileId(const std::filesystem::path &path) const {
  auto stmt = m_db.prepare("SELECT id FROM indexed_file WHERE path = :path");
  stmt.bind(":path", path.c_str());

  if (!stmt.step()) { return std::nullopt; }

  return stmt.columnInt64(0);
}

std::optional<int64_t> FileIndexerDatabase::ensureDirectory(const std::filesystem::path &dir) {
  if (auto id = retrieveFileId(dir)) { return id; }

  std::optional<int64_t> parentId;
  if (auto parent = dir.parent_path(); !parent.empty() && parent != dir) {
    parentId = ensureDirectory(parent);
  }

  auto stmt = m_db.prepare(R"(
    INSERT INTO
      indexed_file (path, parent_id, last_modified_at)
    VALUES
      (:path, :parent_id, :last_modified_at)
    ON CONFLICT (path) DO UPDATE SET parent_id = excluded.parent_id
    RETURNING id
  )");

  std::error_code ec;

  stmt.bind(":path", dir.c_str());
  stmt.bind(":parent_id", parentId);

  if (auto lastModified = fs::last_write_time(dir, ec); !ec) {
    using namespace std::chrono;
    auto sctp = clock_cast<system_clock>(lastModified);
    stmt.bind(":last_modified_at",
              static_cast<int64_t>(duration_cast<seconds>(sctp.time_since_epoch()).count()));
  } else {
    stmt.bindNull(":last_modified_at");
  }

  if (!stmt.step()) {
    flog::error() << "Failed to index parent directory" << dir.c_str() << stmt.lastError();
    return std::nullopt;
  }

  return stmt.columnInt64(0);
}

std::optional<int64_t>
FileIndexerDatabase::retrieveIndexedLastModified(const std::filesystem::path &path) const {
  auto stmt = m_db.prepare("SELECT last_modified_at FROM indexed_file WHERE path = :path");
  stmt.bind(":path", path.c_str());

  if (!stmt.step()) { return std::nullopt; }

  return static_cast<int64_t>(stmt.columnUInt64(0));
}

std::unordered_set<std::filesystem::path>
FileIndexerDatabase::listIndexedDirectoryFiles(const std::filesystem::path &path) const {
  auto dirId = retrieveFileId(path);
  if (!dirId) { return {}; }

  auto stmt = m_db.prepare("SELECT path FROM indexed_file WHERE parent_id = :parent_id");
  stmt.bind(":parent_id", *dirId);

  std::unordered_set<fs::path> paths;

  while (stmt.step()) {
    paths.emplace(stmt.columnText(0));
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

  // FTS5 rank is negative-is-better: ascending order puts the best matches first,
  // which decides what survives the LIMIT when the match set exceeds it
  auto stmt = m_db.prepare(R"(
    SELECT f.path
    FROM indexed_file f
    JOIN unicode_idx ON unicode_idx.rowid = f.id
    WHERE unicode_idx MATCH :search
    ORDER BY unicode_idx.rank
    LIMIT :limit
  )");
  stmt.bind(":search", searchQuery);
  stmt.bind(":limit", limit);

  std::vector<SearchCandidate> results;

  results.reserve(limit);

  while (stmt.step()) {
    results.emplace_back(SearchCandidate{.path = stmt.columnText(0)});
  }

  return results;
}

std::vector<FileIndexerDatabase::SearchCandidate>
FileIndexerDatabase::searchSkeletonCandidates(std::string_view searchQuery, int limit) {
  if (searchQuery.empty() || limit <= 0) return {};

  auto stmt = m_db.prepare(R"(
    SELECT f.path
    FROM indexed_file f
    JOIN skeleton_idx ON skeleton_idx.rowid = f.id
    WHERE skeleton_idx MATCH :search
    ORDER BY skeleton_idx.rank
    LIMIT :limit
  )");
  stmt.bind(":search", searchQuery);
  stmt.bind(":limit", limit);

  std::vector<SearchCandidate> results;

  results.reserve(limit);

  while (stmt.step()) {
    results.emplace_back(SearchCandidate{.path = stmt.columnText(0)});
  }

  return results;
}

void FileIndexerDatabase::deleteIndexedFiles(const std::vector<fs::path> &paths) {
  auto tx = m_db.transaction();

  flog::warn() << "Deleting " << paths.size() << " files\n";

  auto stmt = m_db.prepare("DELETE FROM indexed_file WHERE path = :path");
  auto subtreeStmt = m_db.prepare("DELETE FROM indexed_file WHERE path >= :lower AND path < :upper");

  for (const auto &path : paths) {
    auto [lower, upper] = subtreeRange(path);

    stmt.bind(":path", path.c_str());
    subtreeStmt.bind(":lower", lower);
    subtreeStmt.bind(":upper", upper);

    if (!stmt.exec() || !subtreeStmt.exec()) {
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

void FileIndexerDatabase::rebuildSpellfixVocabulary() {
  using namespace std::chrono;

  auto const start = steady_clock::now();
  std::unordered_map<std::string, int64_t> counts;

  {
    auto stmt = m_db.prepare("SELECT path FROM indexed_file");

    while (stmt.step()) {
      auto const path = stmt.columnText(0);

      for (auto &token : file_indexer::vocab::tokenizeFilename(file_indexer::vocab::basenameView(path))) {
        ++counts[std::move(token)];
      }
    }
  }

  auto tx = m_db.transaction();

  // drop and recreate: the spellfix1 virtual table has no upsert and mass DELETE
  // through a vtab is much slower
  if (!m_db.exec("DROP TABLE IF EXISTS spellfix_vocab") ||
      !m_db.exec("CREATE VIRTUAL TABLE spellfix_vocab USING spellfix1")) {
    flog::error() << "Failed to recreate spellfix_vocab" << m_db.lastError();
    return;
  }

  auto insert = m_db.prepare("INSERT INTO spellfix_vocab(word, rank) VALUES (:word, :rank)");

  for (const auto &[word, count] : counts) {
    insert.bind(":word", word);
    insert.bind(":rank", count);

    if (!insert.exec()) {
      flog::error() << "Failed to insert vocabulary word" << m_db.lastError();
      return;
    }
  }

  if (!tx.commit()) {
    flog::error() << "Failed to commit vocabulary rebuild" << m_db.lastError();
    return;
  }

  auto const elapsed = duration_cast<milliseconds>(steady_clock::now() - start);
  flog::info() << "Rebuilt spellfix vocabulary: " << counts.size() << " words in " << elapsed.count()
               << "ms\n";
}

bool FileIndexerDatabase::hasSpellfixVocabulary() {
  auto stmt = m_db.prepare("SELECT 1 FROM spellfix_vocab_vocab LIMIT 1");
  return static_cast<bool>(stmt) && stmt.step();
}

std::vector<FileIndexerDatabase::SpellfixSuggestion>
FileIndexerDatabase::spellfixSuggestions(std::string_view word, int top, bool prefix) {
  if (word.empty() || top <= 0) return {};

  auto stmt = m_db.prepare(R"(
    SELECT word, distance, score, rank
    FROM spellfix_vocab
    WHERE word MATCH :pattern AND top = :top
  )");

  // a trailing '*' makes spellfix match against vocabulary word prefixes, which fits
  // incomplete launcher queries; both modes are worth querying as they rank differently
  std::string pattern;
  pattern.reserve(word.size() + 1);
  for (char const c : word) {
    pattern.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }
  if (prefix) { pattern.push_back('*'); }

  stmt.bind(":pattern", pattern);
  stmt.bind(":top", top);

  std::vector<SpellfixSuggestion> results;

  results.reserve(top);

  while (stmt.step()) {
    results.emplace_back(SpellfixSuggestion{.word = stmt.columnText(0),
                                            .distance = stmt.columnInt(1),
                                            .score = stmt.columnInt(2),
                                            .rank = stmt.columnInt64(3)});
  }

  return results;
}

void FileIndexerDatabase::indexEvents(const std::vector<FileEvent> &events) {

  flog::info() << "Indexing " << events.size() << " events\n";

  auto tx = m_db.transaction();

  auto modifyStmt = m_db.prepare(R"(
    INSERT INTO
      indexed_file (path, parent_id, last_modified_at)
    VALUES
      (:path, :parent_id, :last_modified_at)
    ON CONFLICT (path) DO UPDATE SET last_modified_at = excluded.last_modified_at, parent_id = excluded.parent_id
  )");

  auto deleteStmt = m_db.prepare("DELETE FROM indexed_file WHERE path = :path");
  auto deleteSubtreeStmt = m_db.prepare("DELETE FROM indexed_file WHERE path >= :lower AND path < :upper");

  std::unordered_map<std::string, std::optional<int64_t>> parentIds;

  auto resolveParent = [&](const fs::path &path) -> std::optional<int64_t> {
    auto parent = path.parent_path();
    if (parent.empty() || parent == path) { return std::nullopt; }
    auto [it, inserted] = parentIds.try_emplace(parent.native());
    if (inserted) { it->second = ensureDirectory(parent); }
    return it->second;
  };

  for (const auto &event : events) {
    bool ok = false;

    switch (event.type) {
    case FileEventType::Modify: {
      using namespace std::chrono;
      auto const secondsSinceEpoch =
          static_cast<int64_t>(duration_cast<seconds>(event.eventTime.time_since_epoch()).count());
      modifyStmt.bind(":last_modified_at", secondsSinceEpoch);
      modifyStmt.bind(":path", event.path.c_str());
      modifyStmt.bind(":parent_id", resolveParent(event.path));
      ok = modifyStmt.exec();
      break;
    }

    case FileEventType::Delete: {
      auto [lower, upper] = subtreeRange(event.path);

      deleteStmt.bind(":path", event.path.c_str());
      deleteSubtreeStmt.bind(":lower", lower);
      deleteSubtreeStmt.bind(":upper", upper);

      ok = deleteStmt.exec() && deleteSubtreeStmt.exec();
      break;
    }
    }

    if (!ok) {
      flog::error() << "Failed to index event for" << event.path.string() << m_db.lastError();
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
      indexed_file (path, parent_id, last_modified_at)
    VALUES
      (:path, :parent_id, :last_modified_at)
    ON CONFLICT (path) DO UPDATE SET last_modified_at = excluded.last_modified_at, parent_id = excluded.parent_id
  )");

  std::error_code ec;
  double score = 1.0;
  std::unordered_map<std::string, std::optional<int64_t>> parentIds;

  auto resolveParent = [&](const fs::path &path) -> std::optional<int64_t> {
    auto parent = path.parent_path();
    if (parent.empty() || parent == path) { return std::nullopt; }
    auto [it, inserted] = parentIds.try_emplace(parent.native());
    if (inserted) { it->second = ensureDirectory(parent); }
    return it->second;
  };

  for (const auto &path : paths) {
    if (auto lastModified = fs::last_write_time(path, ec); !ec) {
      using namespace std::chrono;
      auto sctp = clock_cast<system_clock>(lastModified);
      auto const epoch = static_cast<int64_t>(duration_cast<seconds>(sctp.time_since_epoch()).count());

      stmt.bind(":last_modified_at", epoch);
    } else {
      stmt.bindNull(":last_modified_at");
    }

    stmt.bind(":path", path.c_str());
    stmt.bind(":parent_id", resolveParent(path));

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
