#pragma once

#include <QFuture>
#include <QJsonObject>
#include <QPromise>
#include <QTimer>

#include <atomic>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/fff/fff-library.hpp"

/**
 * `AbstractFileIndexer` backed by the fff C library (path index only;
 * content indexing, grep, and the fff watcher are disabled).
 *
 * Dormant until `start()` is called. The first `start()` triggers a
 * background scan; queries issued during warmup are parked so the user can
 * keep typing freely. Only the most recent query survives — older parked
 * queries are resolved with empty results as soon as they are superseded,
 * and the surviving one is dispatched once the scan completes. Queries
 * issued before `start()` resolve synchronously with an empty result, so
 * callers can opt out of forcing a scan.
 */
class FffFileIndexer : public AbstractFileIndexer {
  Q_OBJECT

public:
  FffFileIndexer();
  ~FffFileIndexer() override;

  void start() override;
  void stop() override;
  void rebuildIndex() override;
  void preferenceValuesChanged(const QJsonObject &preferences) override;
  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                     const QueryParams &params = {}) override;

  ScanState scanState() const override;

private:
  struct PendingQuery {
    std::string query;
    QueryParams params;
    std::shared_ptr<QPromise<std::vector<IndexerFileResult>>> promise;
  };

  vicinae::fff::FffInstance::Config buildConfig(bool forceLocalDbs = false) const;

  void spawnInstance();
  void runQuery(std::shared_ptr<vicinae::fff::FffInstance> instance, PendingQuery query) const;

  std::shared_ptr<vicinae::fff::FffInstance> currentInstance() const;
  void setCurrentInstance(std::shared_ptr<vicinae::fff::FffInstance> instance);

  void drainPendingEmpty();
  void emitProgressSnapshot();

  mutable std::mutex m_instanceMtx;
  std::shared_ptr<vicinae::fff::FffInstance> m_instance;

  std::atomic<bool> m_scanReady{false};
  std::atomic<bool> m_initInProgress{false};
  // Bumped on every `stop()` so any in-flight spawn worker can detect that
  // its FffInstance has been orphaned and drop it.
  std::atomic<std::uint64_t> m_generation{0};
  // Set when a `start()` lands while a stale spawn is still draining; the
  // outgoing worker re-issues a fresh `spawnInstance()` once it exits.
  std::atomic<bool> m_pendingRestart{false};

  mutable std::mutex m_pendingMtx;
  std::optional<PendingQuery> m_pendingQuery;

  mutable std::mutex m_prefsMtx;
  std::filesystem::path m_basePath;
  // When true (default), buildConfig() consults `detectNvimFffPaths()` and
  // points the fff instance at fff.nvim's frecency / history LMDB envs if
  // they exist. Toggled via the `reuseNvimDbs` preference.
  bool m_reuseNvimDbs = true;

  QTimer m_progressPoll;

  std::uint64_t m_lastEmittedCount = 0;
  bool m_lastEmittedScanning = false;
  bool m_lastEmittedReady = false;
};
