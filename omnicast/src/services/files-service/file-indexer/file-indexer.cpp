#include <filesystem>
#include <mutex>
#include "file-indexer.hpp"
#include <QtConcurrent/QtConcurrent>
#include "omnicast.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include "services/files-service/file-indexer/relevancy-scorer.hpp"
#include "timer.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "utils/utils.hpp"
#include <QDebug>
#include <chrono>
#include <qcryptographichash.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <QSqlError>
#include <ranges>
#include <thread>
#include <unistd.h>
#include <unordered_set>

// clang-format off
static const std::vector<std::string> sqlitePragmas = {
	"PRAGMA journal_mode = WAL",
	"PRAGMA synchronous = normal",
	"PRAGMA temp_store = memory",
	// "PRAGMA mmap_size = 30000000000"
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

std::optional<QDateTime> IncrementalScanner::retrieveIndexedLastModified(const fs::path &path) const {
  QSqlQuery query(m_db);

  query.prepare("SELECT last_modified_at FROM indexed_file WHERE path = :path");
  query.addBindValue(path.c_str());

  if (!query.exec()) {
    qCritical() << "Failed to retriveIndexedLastModified" << query.lastError();
    return std::nullopt;
  }

  if (!query.next()) {
    qDebug() << "no indexed file at" << path;
    return std::nullopt;
  }

  return QDateTime::fromSecsSinceEpoch(query.value(0).toULongLong());
}

std::vector<fs::path> IncrementalScanner::listIndexedDirectoryFiles(const fs::path &path) const {
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

void IncrementalScanner::processDirectory(const std::filesystem::path &root) {
  auto indexedFiles = listIndexedDirectoryFiles(root);
  auto existingFiles = indexedFiles | std::ranges::to<std::unordered_set>();
  std::unordered_set<fs::path> currentFiles;
  std::vector<fs::path> deletedFiles;
  std::error_code ec;

  currentFiles.insert(root);

  for (const auto &entry : fs::directory_iterator(root, ec)) {
    if (ec) continue;

    // XXX - We may want to differenciate between new files and already existing later
    // especially if we start indexing file content as well.
    currentFiles.insert(entry.path());
  }

  for (const auto &path : indexedFiles) {
    if (currentFiles.find(path) == currentFiles.end()) { deletedFiles.emplace_back(path); }
  }

  if (!m_db.transaction()) {
    qCritical() << "Failed to start transaction" << m_db.lastError();
    return;
  }

  {
    QSqlQuery query(m_db);

    query.prepare("DELETE FROM indexed_file WHERE path = :path");

    for (const auto &path : deletedFiles) {
      query.addBindValue(path.c_str());

      if (!query.exec()) {
        qCritical() << "Failed to delete indexed file" << path.c_str();
        m_db.rollback();
        return;
      }

      qCritical() << "Delete file" << path.c_str() << "from index";
    }
  }

  {
    QSqlQuery query(m_db);

    query.prepare(R"(
    INSERT INTO 
	  	indexed_file (path, parent_path, name, last_modified_at, relevancy_score) 
	VALUES 
		(:path, :parent_path, :name, :last_modified_at, :relevancy_score) 
	ON CONFLICT (path) DO UPDATE SET 
	last_modified_at = :last_modified_at
  )");

    std::error_code ec;
    RelevancyScorer scorer;
    double score = 1.0;

    for (const auto &path : currentFiles) {
      if (auto lastModified = fs::last_write_time(path, ec); !ec) {
        using namespace std::chrono;
        auto sctp = clock_cast<system_clock>(lastModified);
        long long epoch = duration_cast<seconds>(sctp.time_since_epoch()).count();

        if (root == downloadsFolder()) { qDebug() << "write file" << path << epoch; }

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

  if (!m_db.commit()) {
    qCritical() << "Failed to commit process directory";
    return;
  }
}

std::vector<fs::path> IncrementalScanner::getScannableDirectories(const fs::path &path,
                                                                  std::optional<size_t> maxDepth) const {
  std::vector<fs::path> scannableDirs;
  std::error_code ec;
  FileSystemWalker walker;

  walker.setMaxDepth(maxDepth);
  walker.walk(path, [&](const fs::directory_entry &entry) {
    if (entry.is_directory()) {
      auto lastScanned = retrieveIndexedLastModified(entry);

      if (auto lastModified = fs::last_write_time(entry); lastScanned.has_value() && !ec) {
        using namespace std::chrono;
        auto sctp = clock_cast<system_clock>(lastModified);
        auto lastModifiedDate =
            QDateTime::fromSecsSinceEpoch(duration_cast<seconds>(sctp.time_since_epoch()).count());

        if (lastScanned < lastModifiedDate) { scannableDirs.emplace_back(entry.path()); }
      }
    }
  });

  return scannableDirs;
}

void IncrementalScanner::scan(const fs::path &path, std::optional<size_t> maxDepth) {
  auto dirs = getScannableDirectories(path, maxDepth);

  for (const auto &dir : dirs) {
    qDebug() << "scannable" << dir;
    processDirectory(dir);
  }
}

IncrementalScanner::IncrementalScanner(QSqlDatabase &db) : m_db(db) {}

void WriterWorker::stop() {
  m_alive = false;
  m_batchCv.notify_one();
}

void WriterWorker::run() {
  std::filesystem::path dbPath = Omnicast::dataDir() / "file-indexer.db";
  db = QSqlDatabase::addDatabase("QSQLITE", "file-indexer-writer");
  db.setDatabaseName(dbPath.c_str());

  if (!db.open()) { qCritical() << "Failed to open file indexer database at" << dbPath; }

  QSqlQuery query(db);

  for (const auto &pragma : sqlitePragmas) {
    if (!query.exec(pragma.c_str())) { qCritical() << "Failed to run file-indexer pragma" << pragma; }
  }

  while (m_alive) {
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
	  	indexed_file (path, parent_path, name, last_modified_at, relevancy_score) 
	VALUES 
		(:path, :parent_path, :name, :last_modified_at, :relevancy_score) 
	ON CONFLICT (path) DO NOTHING
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
      db.rollback();
      return;
    }
  }

  if (!db.commit()) { qCritical() << "Failed to commit batchIndex" << db.lastError(); }

  qCritical() << "INDEXED" << paths.size() << "files";
}

WriterWorker::WriterWorker(std::mutex &batchMutex, std::deque<std::vector<std::filesystem::path>> &batchQueue,
                           std::condition_variable &batchCv)
    : batchMutex(batchMutex), batchQueue(batchQueue), m_batchCv(batchCv) {}

void FileIndexer::createTables() {
  QSqlQuery query(m_db);
  MigrationManager manager(m_db, "file-indexer");

  manager.runMigrations();
}

FileIndexer::ScanRecord FileIndexer::mapScan(const QSqlQuery &query) const {
  ScanRecord record;

  record.id = query.value(0).toInt();
  record.status = static_cast<ScanStatus>(query.value(1).toInt());
  record.createdAt = QDateTime::fromSecsSinceEpoch(query.value(2).toULongLong());

  return record;
}

std::optional<FileIndexer::ScanRecord> FileIndexer::getLastScan() const {
  QSqlQuery query(m_db);

  if (!query.exec("SELECT id, status, created_at FROM scan_history ORDER BY created_at DESC LIMIT 1")) {
    qCritical() << "Failed to list scan records" << query.lastError();
    return {};
  }

  if (!query.next()) return std::nullopt;

  return mapScan(query);
}

std::vector<FileIndexer::ScanRecord> FileIndexer::listScans() {
  QSqlQuery query(m_db);

  if (!query.exec("SELECT id, status, created_at FROM scan_history")) {
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

void IndexerScanner::stop() {
  m_alive = false;
  m_scanCv.notify_one();
  m_writerThread.join();
}

void IndexerScanner::enqueueBatch(const std::vector<std::filesystem::path> &paths) {
  bool shouldWait = true;

  while (shouldWait) {
    {
      std::lock_guard lock(m_batchMutex);

      /**
       * handle backpressure by waiting if too many batches are queued
       */
      shouldWait = m_writeBatches.size() >= MAX_PENDING_BATCH_COUNT;
      if (!shouldWait) { m_writeBatches.emplace_back(paths); }
    }

    if (shouldWait) {
      qWarning() << "Handling backpressure: too many batched";
      std::this_thread::sleep_for(std::chrono::milliseconds(BACKPRESSURE_WAIT_MS));
    }
  }

  m_batchCv.notify_one();
}

void IndexerScanner::scan(const std::filesystem::path &root) {
  std::vector<fs::path> batchedIndex;
  FileSystemWalker walker;

  walker.walk(root, [&](const fs::directory_entry &entry) {
    batchedIndex.emplace_back(entry);

    if (batchedIndex.size() > INDEX_BATCH_SIZE) {
      enqueueBatch(batchedIndex);
      batchedIndex.clear();
    }
  });

  enqueueBatch(batchedIndex);
}

void IndexerScanner::enqueue(const std::filesystem::path &path) {
  std::lock_guard lock(m_scanMutex);
  m_scanPaths.push(path);
  m_scanCv.notify_one();
}

void IndexerScanner::run() {
  m_writerWorker = std::make_unique<WriterWorker>(m_batchMutex, m_writeBatches, m_batchCv);
  m_writerThread = std::thread([&]() { m_writerWorker->run(); });

  while (m_alive) {
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

void FileIndexer::start() {
  auto lastScan = getLastScan();

  if (!lastScan) {
    createScan();

    for (const auto &entrypoint : m_entrypoints) {
      m_scanner->enqueue(entrypoint.root);
    }
    return;
  }

  // XXX - for testing
  IncrementalScanner scanner(m_db);

  for (const auto &entrypoint : m_entrypoints) {
    scanner.scan(entrypoint.root, 3);
  }
}

void FileIndexer::setEntrypoints(const std::vector<Entrypoint> &entrypoints) { m_entrypoints = entrypoints; }

QString FileIndexer::preparePrefixSearchQuery(std::string_view query) const {
  QString finalQuery;

  for (const auto &word : std::views::split(query, std::string_view(" "))) {
    std::string_view view(word);

    if (!finalQuery.isEmpty()) { finalQuery += ' '; }

    finalQuery += QString("\"%1\"").arg(qStringFromStdView(view));
  }

  finalQuery += '*';

  return finalQuery;
}

IndexerAsyncQuery *FileIndexer::queryAsync(std::string_view view, const QueryParams &params) const {
  auto asyncQuery = new IndexerAsyncQuery();
  auto searchQuery = qStringFromStdView(view);
  QString finalQuery = preparePrefixSearchQuery(view);

  QThreadPool::globalInstance()->start([asyncQuery, params, finalQuery]() {
    QString connectionName = QString("file-indexer-reader-%1").arg(QUuid::createUuid().toString());
    {
      QSqlDatabase db;
      std::filesystem::path dbPath = Omnicast::dataDir() / "file-indexer.db";

      db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
      db.setDatabaseName(dbPath.c_str());

      if (!db.open()) { qCritical() << "Failed to open file indexer database at" << dbPath; }

      // QSQLITE doesn't support bindValue for MATCH queries, so we need to inject like this
      auto queryString = QString(R"(
  	SELECT path FROM indexed_file f 
	JOIN unicode_idx ON unicode_idx.rowid = f.id 
	WHERE 
	    unicode_idx MATCH '%1'
	ORDER BY f.relevancy_score DESC, unicode_idx.rank 
	LIMIT :limit
	OFFSET :offset
  )")
                             .arg(finalQuery);

      QSqlQuery query(db);

      query.prepare(queryString);
      query.bindValue(":limit", params.pagination.limit);
      query.bindValue(":offset", params.pagination.offset);

      Timer timer;
      if (!query.exec()) { qCritical() << "Search query failed" << query.lastError(); }

      timer.time("queryAsync");

      std::vector<IndexerFileResult> results;
      while (query.next()) {
        IndexerFileResult result;

        result.path = query.value(0).toString().toStdString();
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

FileIndexer::~FileIndexer() {
  m_scanner->stop();
  m_scannerThread.join();
}
