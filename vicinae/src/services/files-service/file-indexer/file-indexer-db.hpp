#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include <expected>
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
  enum class ScanType { Full, Incremental };
  enum class ScanStatus {
    Pending,
    Started,
    Failed,
    Finished,
  };

  struct ScanRecord {
    int id;
    ScanStatus status;
    QDateTime createdAt;
    std::filesystem::path path;
    ScanType type;
  };

  ScanRecord mapScan(const QSqlQuery &query) const;
  static QString createRandomConnectionId();
  static std::filesystem::path getDatabasePath();

  std::vector<ScanRecord> listScans();
  std::optional<ScanRecord> getLastScan() const;

  std::vector<ScanRecord> listStartedScans();

  bool updateScanStatus(int scanId, ScanStatus status);
  std::expected<ScanRecord, QString> createScan(const std::filesystem::path &path, ScanType type);

  bool setScanError(int scanId, const QString &error);

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
