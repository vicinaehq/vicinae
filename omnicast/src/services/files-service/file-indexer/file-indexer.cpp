#include <filesystem>
#include <mutex>
#include "file-indexer.hpp"
#include <QtConcurrent/QtConcurrent>
#include "omnicast.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/relevancy-scorer.hpp"
#include "timer.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "utils/utils.hpp"
#include <QDebug>
#include <chrono>
#include <qcryptographichash.h>
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <QSqlError>
#include <ranges>

// clang-format off
static const std::vector<std::string> sqlitePragmas = {
	"PRAGMA journal_mode = WAL",
	"PRAGMA synchronous = normal",
	"PRAGMA temp_store = memory",
	"PRAGMA mmap_size = 30000000000"
};

// clang-format on

namespace fs = std::filesystem;

/*
 * List of absolute paths to never follow during indexing. Mostly used to exclude
 * pseudo filesystems such as /run or /proc.
 * Contextual exclusions (using gitignore-like semantics) are handled separately.
 */
static const std::vector<fs::path> EXCLUDED_PATHS = {"/sys", "/run",     "/proc", "/tmp",
                                                     "/mnt", "/var/tmp", "/efi",  "/dev"};

void WriterWorker::run() {

  std::filesystem::path dbPath = Omnicast::dataDir() / "file-indexer.db";
  m_homeRootDirectories = homeRootDirectories();
  db = QSqlDatabase::addDatabase("QSQLITE", "file-indexer-writer");
  db.setDatabaseName(dbPath.c_str());

  if (!db.open()) { qCritical() << "Failed to open file indexer database at" << dbPath; }

  QSqlQuery query(db);

  for (const auto &pragma : sqlitePragmas) {
    if (!query.exec(pragma.c_str())) { qCritical() << "Failed to run file-indexer pragma" << pragma; }
  }

  while (true) {
    std::deque<std::vector<std::filesystem::path>> batch;

    {
      std::unique_lock<std::mutex> lock(batchMutex);

      m_batchCv.wait(lock, [&]() { return !batchQueue.empty(); });
      batch = std::move(batchQueue);
      batchQueue.clear();
    }

    for (const auto &paths : batch) {
      batchWrite(paths);
    }
  }
}

void WriterWorker::batchWrite(const std::vector<fs::path> &paths) {
  // Writing is happening in the writerThread
  QSqlQuery query(db);

  qDebug() << "starting batch write" << paths.size();

  if (!db.transaction()) {
    qCritical() << "Failed to start batch insert transaction" << db.lastError();
    return;
  }

  query.prepare(R"(
    INSERT INTO 
	  	indexed_file (path, name, last_modified_at, relevancy_score) 
	VALUES 
		(:path, :name, :last_modified_at, :relevancy_score) 
	ON CONFLICT (path) DO NOTHING
  )");

  std::error_code ec;
  RelevancyScorer scorer;
  double score = 1.0;

  for (const auto &path : paths) {
    if (auto lastModified = std::filesystem::last_write_time(path, ec); !ec) {
      auto sctp = std::chrono::clock_cast<std::chrono::system_clock>(lastModified);
      long long epoch = std::chrono::duration_cast<std::chrono::seconds>(sctp.time_since_epoch()).count();

      query.bindValue(":last_modified_at", epoch);
      score = scorer.computeScore(path, lastModified);
    } else {
      query.bindValue(":last_modified_at", QVariant());
      score = scorer.computeScore(path, std::nullopt);
    }

    query.bindValue(":path", path.c_str());
    query.bindValue(":name", path.filename().c_str());
    query.bindValue(":relevancy_score", score);

    if (!query.exec()) {
      qCritical() << "Failed to insert file in index" << path << query.lastError();
      db.rollback();
      return;
    }
  }

  if (!db.commit()) { qCritical() << "Failed to commit batchIndex" << db.lastError(); }

  qCritical() << "INDEXED" << paths.size() << "files";
}

void FileIndexer::createTables() {
  QSqlQuery query(m_db);
  MigrationManager manager(m_db, "file-indexer");

  manager.runMigrations();
}

std::vector<FileIndexer::ScanRecord> FileIndexer::listScans() {
  QSqlQuery query(m_db);

  if (!query.exec("SELECT id, status, created_at FROM scan_history")) {
    qCritical() << "Failed to list scan records" << query.lastError();
    return {};
  }

  std::vector<ScanRecord> records;

  while (query.next()) {
    ScanRecord record;

    record.id = query.value(0).toInt();
    record.status = static_cast<ScanStatus>(query.value(1).toInt());
    record.createdAt = QDateTime::fromSecsSinceEpoch(query.value(2).toULongLong());
    records.emplace_back(record);
  }

  return records;
}

FileIndexer::ScanRecord FileIndexer::createScan() {
  QSqlQuery query(m_db);

  query.prepare("INSERT INTO scan_history (status) VALUES (:status) RETURNING id, status, created_at");
  query.addBindValue(ScanStatus::Started);

  if (!query.exec()) {
    qCritical() << "Failed to create scan history" << query.lastError();
    return {};
  }

  if (!query.next()) {
    qCritical() << "No next entry after createScan" << query.lastError();
    return {};
  }

  ScanRecord record;

  record.id = query.value(0).toInt();
  record.status = static_cast<ScanStatus>(query.value(1).toInt());
  record.createdAt = QDateTime::fromSecsSinceEpoch(query.value(2).toULongLong());

  return record;
}

void IndexerScanner::enqueueBatch(const std::vector<std::filesystem::path> &paths) {
  std::lock_guard lock(m_batchMutex);
  m_writeBatches.emplace_back(paths);
  m_batchCv.notify_one();
}

void IndexerScanner::scan(const std::filesystem::path &root) {
  std::stack<fs::path> paths;
  std::error_code ec;
  std::vector<fs::path> batchedIndex;

  paths.push(root);

  while (!paths.empty()) {
    auto path = paths.top();

    paths.pop();

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (ec) continue;
      if (entry.is_symlink()) { continue; }

      // qDebug() << entry.path();

      if (std::ranges::find(EXCLUDED_PATHS, entry.path()) != EXCLUDED_PATHS.end()) {
        qDebug() << "ignored path" << entry.path();
        continue;
      }

      if (entry.is_directory()) { paths.push(entry); }

      batchedIndex.emplace_back(entry);

      auto entryPath = entry.path().string();
    }

    if (batchedIndex.size() > INDEX_BATCH_SIZE) {
      enqueueBatch(batchedIndex);
      batchedIndex.clear();
    }
  }

  qDebug() << "batch ready" << batchedIndex.size();
  enqueueBatch(batchedIndex);
}

void IndexerScanner::enqueue(const std::filesystem::path &path) {
  std::lock_guard lock(m_scanMutex);
  m_scanPaths.push(path);
  m_scanCv.notify_one();
}

void IndexerScanner::run() {
  m_writerThread = std::thread([&]() {
    auto worker = std::make_unique<WriterWorker>(m_batchMutex, m_writeBatches, m_batchCv);
    worker->run();
  });

  while (true) {
    std::filesystem::path path;
    {
      std::unique_lock<std::mutex> lock(m_scanMutex);
      m_scanCv.wait(lock, [&]() { return !m_scanPaths.empty(); });
      path = m_scanPaths.front();
      m_scanPaths.pop();
    }

    scan(path);
  }
}

void FileIndexer::start() {}

std::vector<IndexerFileResult> FileIndexer::query(std::string_view view) const {
  std::vector<IndexerFileResult> results;
  QSqlQuery query(m_db);

  // QSQLITE doesn't support binding values for MATCH, sadly we are forced to inject like this
  auto queryString = QString(R"(
  	SELECT path, unicode_idx.rank FROM indexed_file f 
	JOIN unicode_idx ON unicode_idx.rowid = f.id 
	WHERE 
	    unicode_idx MATCH '"%1"*'
	ORDER BY bm25(unicode_idx)
	LIMIT 50
  )")
                         .arg(qStringFromStdView(view));

  qDebug() << "query" << queryString;

  if (!query.exec(queryString)) {
    qCritical() << "Search query failed" << query.lastError();
    return {};
  }

  while (query.next()) {
    IndexerFileResult result{.path = query.value(0).toString().toStdString()};

    results.emplace_back(result);
  }

  std::ranges::sort(results,
                    [](auto &&a, auto &&b) { return a.path.string().size() > b.path.string().size(); });

  return results;
}

void FileIndexer::setEntrypoints(const std::vector<Entrypoint> &entrypoints) {
  m_entrypoints = entrypoints;

  if (listScans().empty()) {
    // perform initial scan. we will handle incremental scans later one
    qDebug() << "Initial scan";
    for (const auto &entrypoint : entrypoints) {
      m_scanner->enqueue(entrypoint.root);
    }

    return;
  }

  // perform incremental scan based on existing data
}

IndexerAsyncQuery *FileIndexer::queryAsync(std::string_view view) const {
  auto asyncQuery = new IndexerAsyncQuery();
  auto searchQuery = qStringFromStdView(view);

  QThreadPool::globalInstance()->start([asyncQuery, searchQuery]() {
    QString connectionName = QString("file-indexer-reader-%1").arg(QUuid::createUuid().toString());
    {
      QSqlDatabase db;
      std::filesystem::path dbPath = Omnicast::dataDir() / "file-indexer.db";

      db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
      db.setDatabaseName(dbPath.c_str());

      if (!db.open()) { qCritical() << "Failed to open file indexer database at" << dbPath; }

      QSqlQuery query(db);

      for (const auto &pragma : sqlitePragmas) {
        if (!query.exec(pragma.c_str())) { qCritical() << "Failed to run file-indexer pragma" << pragma; }
      }

      auto queryString = QString(R"(
  	SELECT path, unicode_idx.rank FROM indexed_file f 
	JOIN unicode_idx ON unicode_idx.rowid = f.id 
	WHERE 
	    unicode_idx MATCH '"%1"*'
	ORDER BY f.relevancy_score DESC, bm25(unicode_idx)
	LIMIT 50
  )")
                             .arg(searchQuery);

      Timer timer;
      if (!query.exec(queryString)) { qCritical() << "Search query failed" << query.lastError(); }

      timer.time("queryAsync");

      std::vector<IndexerFileResult> results;
      while (query.next()) {
        IndexerFileResult result{.path = query.value(0).toString().toStdString()};

        results.emplace_back(result);
      }

      emit asyncQuery->finished(results);
    }

    QSqlDatabase::removeDatabase(connectionName);
  });

  return asyncQuery;
}

FileIndexer::FileIndexer() : m_db(QSqlDatabase::addDatabase("QSQLITE", "file-indexer")) {
  std::filesystem::path dbPath = Omnicast::dataDir() / "file-indexer.db";

  m_db.setDatabaseName(dbPath.c_str());

  if (!m_db.open()) { qCritical() << "Failed to open file indexer database at" << dbPath; }

  QSqlQuery query(m_db);

  for (const auto &pragma : sqlitePragmas) {
    if (!query.exec(pragma.c_str())) { qCritical() << "Failed to run file-indexer pragma" << pragma; }
  }

  createTables();
  m_scannerThread = std::thread([&]() { m_scanner->run(); });
}
