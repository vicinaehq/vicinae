#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include <qdatetime.h>
#include <qobject.h>
#include <qrandom.h>
#include <qsqldatabase.h>
#include <filesystem>

/**
 * File indexer sqlite database operations.
 * Note that each instance owns its own database connection, as a single
 * connection is not thread safe.
 */

class FileIndexerDatabase : public QObject {
  QSqlDatabase m_db;
  QString m_connectionId;

public:
  enum ScanStatus {
    Started,
    Failed,
    Finished,
  };

  struct ScanRecord {
    int id;
    ScanStatus status;
    QDateTime createdAt;
  };

  ScanRecord mapScan(const QSqlQuery &query) const;
  static QString createRandomConnectionId();
  static std::filesystem::path getDatabasePath();

  std::vector<ScanRecord> listScans();
  std::optional<ScanRecord> getLastScan() const;
  ScanRecord createScan();

  std::optional<QDateTime> retrieveIndexedLastModified(const std::filesystem::path &path) const;
  std::vector<std::filesystem::path> listIndexedDirectoryFiles(const std::filesystem::path &path) const;

  void deleteIndexedFiles(const std::vector<std::filesystem::path> &paths);
  void indexFiles(const std::vector<std::filesystem::path> &paths);
  std::vector<std::filesystem::path> search(std::string_view searchQuery,
                                            const AbstractFileIndexer::QueryParams &params);

  void runMigrations();

  QSqlDatabase *database();

  FileIndexerDatabase();
  ~FileIndexerDatabase();
};
