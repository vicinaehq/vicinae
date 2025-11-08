#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/scan.hpp"
#include "utils/expected.hpp"
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

  std::optional<ScanRecord> getLastScan(const std::filesystem::path &path, ScanType scanType) const;
  std::optional<FileIndexerDatabase::ScanRecord>
  getLastSuccessfulScan(const std::filesystem::path &path) const;

  std::vector<ScanRecord> listScans();
  std::vector<ScanRecord> listScans(ScanType scanType, ScanStatus scanStatus);

  bool updateScanStatus(int scanId, ScanStatus status);
  tl::expected<ScanRecord, QString> createScan(const std::filesystem::path &path, ScanType type);

  bool setScanError(int scanId, const QString &error);

  std::optional<QDateTime> retrieveIndexedLastModified(const std::filesystem::path &path) const;
  std::vector<std::filesystem::path> listIndexedDirectoryFiles(const std::filesystem::path &path) const;

  void deleteAllIndexedFiles();
  void deleteIndexedFiles(const std::vector<std::filesystem::path> &paths);
  void indexFiles(const std::vector<std::filesystem::path> &paths);
  std::vector<std::filesystem::path> search(std::string_view searchQuery,
                                            const AbstractFileIndexer::QueryParams &params);

  void indexEvents(const std::vector<FileEvent> &events);

  void runMigrations();

  QSqlDatabase *database();

  FileIndexerDatabase();
  ~FileIndexerDatabase();
};
