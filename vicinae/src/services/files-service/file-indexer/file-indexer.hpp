#pragma once
#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qthread.h>
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/home-directory-watcher.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/scan-dispatcher.hpp"
#include <malloc.h>
#include <qdatetime.h>
#include <qsqldatabase.h>
#include <qtmetamacros.h>

/**
 * Generic file indexer that should be usable in most linux environments.
 * SQLite FTS5 is used as a backend, making it technically possible to index the entire
 * filesystem if necessary.
 * Queries usually remain very fast (<100ms), although not fast enough to perform them in the UI thread
 * without introducing slowdowns.
 */
class FileIndexer : public AbstractFileIndexer {
  Q_OBJECT

public:
  std::shared_ptr<DbWriter> m_writer;
  std::vector<std::filesystem::path> m_entrypoints;
  std::vector<std::filesystem::path> m_watcherPaths;
  std::vector<std::string> m_excludedFilenames;
  FileIndexerDatabase m_db;

  ScanDispatcher m_dispatcher;

  std::unique_ptr<HomeDirectoryWatcher> m_homeWatcher;

  // move that somewhere else later
  QString preparePrefixSearchQuery(std::string_view query) const;

public:
  void startFullscan();
  void rebuildIndex() override;
  void preferenceValuesChanged(const QJsonObject &preferences) override;
  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                     const QueryParams &params = {}) const override;
  void start() override;

  FileIndexer();
};
