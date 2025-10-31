#include "file-indexer-db.hpp"
#include "scan.hpp"
#include "vicinae.hpp"
#include "services/files-service/file-indexer/relevancy-scorer.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "utils/utils.hpp"
#include "regex-utils.hpp"
#include <chrono>
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qtimer.h>
#include <quuid.h>
#include <qdebug.h>
#include <QSqlError>
#include <QRegularExpression>
#include <QSqlDriver>
#include <QVariant>
#include <sqlite3.h>

// clang-format off
static const std::vector<std::string> SQLITE_PRAGMAS = {
	"PRAGMA journal_mode = WAL",
	"PRAGMA synchronous = normal",
	"PRAGMA temp_store = memory",
	// "PRAGMA mmap_size = 30000000000"
};
// clang-format on

namespace fs = std::filesystem;

QString FileIndexerDatabase::createRandomConnectionId() {
  return QString("file-indexer-%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

fs::path FileIndexerDatabase::getDatabasePath() { return Omnicast::dataDir() / "file-indexer.db"; }

std::optional<QDateTime>
FileIndexerDatabase::retrieveIndexedLastModified(const std::filesystem::path &path) const {
  QSqlQuery query(m_db);

  query.prepare("SELECT last_modified_at FROM indexed_file WHERE path = :path");
  query.addBindValue(path.c_str());

  if (!query.exec()) {
    qWarning() << "Failed to retriveIndexedLastModified" << query.lastError();
    return std::nullopt;
  }

  if (!query.next()) { return std::nullopt; }

  return QDateTime::fromSecsSinceEpoch(query.value(0).toULongLong());
}

std::vector<std::filesystem::path>
FileIndexerDatabase::listIndexedDirectoryFiles(const std::filesystem::path &path) const {
  QSqlQuery query(m_db);

  query.prepare("SELECT path, last_modified_at FROM indexed_file WHERE parent_path = :path");
  query.addBindValue(path.c_str());

  if (!query.exec()) {
    qCritical() << "listIndexedDirectoryFiles failed:" << query.lastError();
    return {};
  }

  std::vector<fs::path> paths;

  while (query.next()) {
    paths.emplace_back(query.value(0).toString().toStdString());
  }

  return paths;
}

void FileIndexerDatabase::runMigrations() {
  QSqlQuery query(m_db);
  MigrationManager manager(m_db, "file-indexer");

  manager.runMigrations();
}

bool FileIndexerDatabase::setScanError(int scanId, const QString &error) {
  QSqlQuery query(m_db);

  query.prepare("UPDATE scan_history SET status = :status, error = :error WHERE id = :id");
  query.bindValue(":id", scanId);
  query.bindValue(":status", static_cast<quint8>(ScanStatus::Failed));
  query.bindValue(":error", error);

  if (!query.exec()) {
    qWarning() << "Failed to update scan status" << query.lastError();
    return false;
  }

  return true;
}

bool FileIndexerDatabase::updateScanStatus(int scanId, ScanStatus status) {
  QSqlQuery query(m_db);

  query.prepare("UPDATE scan_history SET status = :status WHERE id = :id");
  query.bindValue(":id", scanId);
  query.bindValue(":status", static_cast<quint8>(status));

  if (!query.exec()) {
    qWarning() << "Failed to update scan status" << query.lastError();
    return false;
  }

  return true;
}

tl::expected<FileIndexerDatabase::ScanRecord, QString>
FileIndexerDatabase::createScan(const std::filesystem::path &path, ScanType type) {
  QSqlQuery query(m_db);

  query.prepare("INSERT INTO scan_history (entrypoint, type, status) VALUES (:entrypoint, :type, :status) "
                "RETURNING id, status, "
                "created_at, entrypoint");
  query.bindValue(":entrypoint", path.c_str());
  query.bindValue(":status", static_cast<quint8>(ScanStatus::Pending));
  query.bindValue(":type", static_cast<quint8>(type));

  if (!query.exec()) {
    qWarning() << "Failed to create scan history" << query.lastError();
    return tl::unexpected("Failed to create scan history");
  }

  if (!query.next()) {
    qWarning() << "No next entry after createScan" << query.lastError();
    return tl::unexpected("No next entry after createScan");
  }

  ScanRecord record;

  record.id = query.value(0).toInt();
  record.status = static_cast<ScanStatus>(query.value(1).toInt());
  record.createdAt = QDateTime::fromSecsSinceEpoch(query.value(2).toULongLong());
  record.path = path;

  return record;
}

FileIndexerDatabase::ScanRecord FileIndexerDatabase::mapScan(const QSqlQuery &query) const {
  ScanRecord record;

  record.id = query.value(0).toInt();
  record.status = static_cast<ScanStatus>(query.value(1).toInt());
  record.createdAt = QDateTime::fromSecsSinceEpoch(query.value(2).toULongLong());
  record.path = query.value(3).toString().toStdString();
  record.type = static_cast<ScanType>(query.value(4).toUInt());

  return record;
}

std::optional<FileIndexerDatabase::ScanRecord> FileIndexerDatabase::getLastScan() const {
  QSqlQuery query(m_db);

  if (!query.exec("SELECT id, status, created_at, entrypoint, type FROM scan_history ORDER BY created_at "
                  "DESC LIMIT 1")) {
    qCritical() << "Failed to list scan records" << query.lastError();
    return {};
  }

  if (!query.next()) return std::nullopt;

  return mapScan(query);
}

std::vector<FileIndexerDatabase::ScanRecord> FileIndexerDatabase::listScans() {
  QSqlQuery query(m_db);

  if (!query.exec("SELECT id, status, created_at, entrypoint, type FROM scan_history")) {
    qCritical() << "Failed to list scan records" << query.lastError();
    return {};
  }

  std::vector<ScanRecord> records;

  records.reserve(0xF);

  while (query.next()) {
    records.emplace_back(mapScan(query));
  }

  return records;
}

std::vector<FileIndexerDatabase::ScanRecord> FileIndexerDatabase::listStartedScans() {
  QSqlQuery query(m_db);

  query.prepare("SELECT id, status, created_at, entrypoint, type FROM scan_history WHERE status = :status");
  query.bindValue(":status", static_cast<quint8>(ScanStatus::Started));

  if (!query.exec()) {
    qCritical() << "Failed to list started scan records" << query.lastError();
    return {};
  }

  std::vector<ScanRecord> records;

  records.reserve(0xF);

  while (query.next()) {
    records.emplace_back(mapScan(query));
  }

  return records;
}

QSqlDatabase *FileIndexerDatabase::database() { return &m_db; }

std::vector<fs::path> FileIndexerDatabase::search(std::string_view searchQuery,
                                                  const AbstractFileIndexer::QueryParams &params) {
  QString queryString;
  std::string_view regexString;
  std::string staticChars;
  bool hasPattern = false;

  if (params.useRegex) {
    regexString = searchQuery;
    // trigram search requires at least 3 characters
    staticChars = extractStaticCharsFromRegex(searchQuery, 3);
    searchQuery = staticChars;
    bool hasSearchString = !searchQuery.empty();
    hasPattern = !regexString.empty();

    queryString =
        QString(R"(
    SELECT f.path, tri_idx.rank FROM indexed_file f 
        JOIN tri_idx ON tri_idx.rowid = f.id
        WHERE %2%3
        ORDER BY f.relevancy_score, tri_idx.rank
        LIMIT :limit
        OFFSET :offset
  )")
            .arg(hasSearchString ? "tri_idx MATCH '" + qStringFromStdView(searchQuery) + "'" : "1=1")
            .arg(hasPattern ? " AND f.name REGEXP :pattern" : "");
  } else {
    queryString = QString(R"(
  	SELECT path, rank FROM indexed_file f 
	JOIN unicode_idx ON unicode_idx.rowid = f.id 
	WHERE 
	    unicode_idx MATCH '%1'
	ORDER BY f.relevancy_score DESC, unicode_idx.rank 
	LIMIT :limit
	OFFSET :offset
  )")
                      .arg(qStringFromStdView(searchQuery));
  }

  QSqlQuery query(m_db);
  query.prepare(queryString);

  if (params.useRegex && hasPattern) { query.bindValue(":pattern", qStringFromStdView(regexString)); }
  query.bindValue(":limit", params.pagination.limit);
  query.bindValue(":offset", params.pagination.offset);

  if (!query.exec()) { qWarning() << "Search query failed" << query.lastError(); }

  std::vector<fs::path> results;
  std::error_code ec;

  results.reserve(params.pagination.limit);

  while (query.next()) {
    fs::path path = query.value(0).toString().toStdString();

    if (fs::exists(path, ec)) { results.emplace_back(path); }
  }

  return results;
}

void FileIndexerDatabase::deleteIndexedFiles(const std::vector<fs::path> &paths) {
  if (!m_db.transaction()) {
    qCritical() << "Failed to start transaction";
    return;
  }

  {
    QSqlQuery query(m_db);

    query.prepare("DELETE FROM indexed_file WHERE path = :path");

    for (const auto &path : paths) {
      query.addBindValue(path.c_str());

      if (!query.exec()) {
        qCritical() << "Failed to delete indexed file" << path.c_str();
        m_db.rollback();
        return;
      }
    }
  }

  if (!m_db.commit()) { qCritical() << "Failed to commit"; }
}

void FileIndexerDatabase::indexEvents(const std::vector<FileEvent> &events) {
  if (!m_db.transaction()) {
    qWarning() << "Failed to start batch transaction" << m_db.lastError();
    return;
  }

  {
    QSqlQuery modifyQuery(m_db);
    modifyQuery.prepare(R"(
    INSERT INTO 
                indexed_file (path, parent_path, name, last_modified_at, relevancy_score) 
        VALUES 
                (:path, :parent_path, :name, :last_modified_at, :relevancy_score) 
        ON CONFLICT (path) DO UPDATE SET last_modified_at = :last_modified_at
    )");

    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM indexed_file WHERE path = :path");

    QSqlQuery *activeQuery;

    for (const auto &event : events) {
      switch (event.type) {
      case FileEventType::Modify: {
        using namespace std::chrono;
        long long secondsSinceEpoch = duration_cast<seconds>(event.eventTime.time_since_epoch()).count();
        modifyQuery.bindValue(":last_modified_at", secondsSinceEpoch);

        modifyQuery.bindValue(":path", event.path.c_str());
        modifyQuery.bindValue(":parent_path", event.path.parent_path().c_str());
        modifyQuery.bindValue(":name", event.path.filename().c_str());

        // Move this out of the loop if RelevancyScore ends up having any state
        RelevancyScorer scorer;
        modifyQuery.bindValue(":relevancy_score", scorer.computeScore(event.path, event.eventTime));

        activeQuery = &modifyQuery;
        break;
      }

      case FileEventType::Delete: {
        deleteQuery.bindValue(":path", event.path.c_str());

        activeQuery = &deleteQuery;
        break;
      }
      }
      if (!activeQuery->exec()) {
        static std::map<FileEventType, std::string> verbMap = {{FileEventType::Delete, "deleted"},
                                                               {FileEventType::Modify, "modified"}};
        qCritical() << "Failed to index" << verbMap[event.type] << "file" << event.path
                    << activeQuery->lastError();
        m_db.rollback();
        return;
      }
    }
  }
  if (!m_db.commit()) { qCritical() << "Failed to commit"; }
}

void FileIndexerDatabase::indexFiles(const std::vector<std::filesystem::path> &paths) {

  if (!m_db.transaction()) {
    qWarning() << "Failed to start batch insert transaction" << m_db.lastError();
    return;
  }

  {
    QSqlQuery query(m_db);

    query.prepare(R"(
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
        long long epoch = duration_cast<seconds>(sctp.time_since_epoch()).count();

        query.bindValue(":last_modified_at", epoch);
        score = scorer.computeScore(path, lastModified);
      } else {
        query.bindValue(":last_modified_at", QVariant());
        score = scorer.computeScore(path, std::nullopt);
      }

      query.bindValue(":path", path.c_str());
      query.bindValue(":parent_path", path.parent_path().c_str());
      query.bindValue(":name", path.filename().c_str());
      query.bindValue(":relevancy_score", score);

      if (!query.exec()) {
        qCritical() << "Failed to insert file in index" << path << query.lastError();
        m_db.rollback();
        return;
      }
    }
  }

  if (!m_db.commit()) { qCritical() << "Failed to commit batchIndex" << m_db.lastError(); }
}

static void sqliteRegexpCallback(sqlite3_context *context, int argc, sqlite3_value **argv) {
  if (argc != 2) {
    sqlite3_result_error(context, "REGEXP requires 2 arguments", -1);
    return;
  }

  const char *pattern = reinterpret_cast<const char *>(sqlite3_value_text(argv[0]));
  const char *text = reinterpret_cast<const char *>(sqlite3_value_text(argv[1]));

  if (!pattern || !text) {
    sqlite3_result_null(context);
    return;
  }

  QRegularExpression regex(QString::fromUtf8(pattern));
  QString textStr = QString::fromUtf8(text);

  if (!regex.isValid()) {
    sqlite3_result_error(context, "Invalid regular expression", -1);
    return;
  }

  sqlite3_result_int(context, regex.match(textStr).hasMatch() ? 1 : 0);
}

FileIndexerDatabase::FileIndexerDatabase() : m_connectionId(createRandomConnectionId()) {
  m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionId);
  m_db.setDatabaseName(getDatabasePath().c_str());

  if (!m_db.open()) {
    qCritical() << "Failed to open datbase at" << getDatabasePath();
    return;
  }

  QVariant v = m_db.driver()->handle();
  if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*") == 0) {
    sqlite3 *db_handle = *static_cast<sqlite3 **>(v.data());
    if (db_handle) {
      sqlite3_create_function(db_handle, "REGEXP", 2, SQLITE_UTF8, nullptr, sqliteRegexpCallback, nullptr,
                              nullptr);
    }
  }

  QSqlQuery query(m_db);

  for (const auto &pragma : SQLITE_PRAGMAS) {
    if (!query.exec(pragma.c_str())) { qCritical() << "Failed to run file-indexer pragma" << pragma.c_str(); }
  }
}

FileIndexerDatabase::~FileIndexerDatabase() {
  QString id = m_connectionId;
  // we need this to prevent a warning, although weirdly enough we don't have
  // any pending QSQLQuery by that point.
  QTimer::singleShot(0, [id]() { QSqlDatabase::removeDatabase(id); });
}
