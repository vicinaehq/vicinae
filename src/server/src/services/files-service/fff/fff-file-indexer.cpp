#include "fff-file-indexer.hpp"

#include <QDebug>
#include <QMetaObject>
#include <QThreadPool>

#include <chrono>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "utils/utils.hpp"
#include "vicinae.hpp"

namespace fs = std::filesystem;

using FffInstance = vicinae::fff::FffInstance;

namespace {
fs::path defaultBasePath() {
  if (const char *home = std::getenv("HOME")) return fs::path(home);
  return fs::current_path();
}

fs::path fffStateDir() { return Omnicast::dataDir() / "fff"; }

// Resolve a directory honoring the matching XDG variable. `homeRelative` is
// the fallback (e.g. ".cache" or ".local/share") used when the env var is
// unset or empty.
fs::path xdgDir(const char *envVar, const char *homeRelative) {
  if (const char *v = std::getenv(envVar); v && *v) return fs::path(v);
  if (const char *home = std::getenv("HOME"); home && *home) return fs::path(home) / homeRelative;
  return fs::current_path() / homeRelative;
}

struct NvimFffPaths {
  std::optional<fs::path> frecency;
  std::optional<fs::path> history;
};

// Probe fff.nvim's default LMDB envs. A path counts as "present" only if it
// is a directory containing a `data.mdb` file (the LMDB sentinel), so we
// don't accidentally point fff at an empty directory.
NvimFffPaths detectNvimFffPaths() {
  NvimFffPaths out;

  fs::path frecency = xdgDir("XDG_CACHE_HOME", ".cache") / "nvim" / "fff_nvim";
  fs::path history = xdgDir("XDG_DATA_HOME", ".local/share") / "nvim" / "fff_queries";

  std::error_code ec;
  if (fs::is_directory(frecency, ec) && fs::is_regular_file(frecency / "data.mdb", ec)) {
    out.frecency = std::move(frecency);
  }
  ec.clear();
  if (fs::is_directory(history, ec) && fs::is_regular_file(history / "data.mdb", ec)) {
    out.history = std::move(history);
  }
  return out;
}

std::vector<fs::path> parsePaths(const QJsonObject &preferences, const QString &key) {
  auto raw = preferences.value(key).toString();
  auto parts = raw.split(';', Qt::SkipEmptyParts);
  return ranges_to<std::vector>(
      parts | std::views::transform([](const QString &v) { return fs::path(v.trimmed().toStdString()); }));
}

} // namespace

FffFileIndexer::FffFileIndexer() : m_basePath(defaultBasePath()) {
  m_progressPoll.setInterval(200);
  m_progressPoll.setSingleShot(false);
  QObject::connect(&m_progressPoll, &QTimer::timeout, this, &FffFileIndexer::emitProgressSnapshot);
}

FffFileIndexer::~FffFileIndexer() {
  m_progressPoll.stop();
  m_generation.fetch_add(1, std::memory_order_acq_rel);
  drainPendingEmpty();
}

FffInstance::Config FffFileIndexer::buildConfig(bool forceLocalDbs) const {
  fs::path base;
  bool reuseNvim;
  {
    std::lock_guard<std::mutex> lock(m_prefsMtx);
    base = m_basePath.empty() ? defaultBasePath() : m_basePath;
    reuseNvim = m_reuseNvimDbs && !forceLocalDbs;
  }

  auto stateDir = fffStateDir();
  std::error_code ec;
  fs::create_directories(stateDir, ec);

  fs::path frecencyDb = stateDir / "frecency";
  fs::path historyDb = stateDir / "history";

  if (reuseNvim) {
    auto nvim = detectNvimFffPaths();
    if (nvim.frecency) {
      qInfo() << "fff: reusing fff.nvim frecency db at" << nvim.frecency->c_str();
      frecencyDb = std::move(*nvim.frecency);
    }
    if (nvim.history) {
      qInfo() << "fff: reusing fff.nvim history db at" << nvim.history->c_str();
      historyDb = std::move(*nvim.history);
    }
  }

  FffInstance::Config config;
  config.basePath = std::move(base);
  config.frecencyDbPath = std::move(frecencyDb);
  config.historyDbPath = std::move(historyDb);
  config.enableMmapCache = false;
  config.enableContentIndexing = false;
  config.watch = false;
  config.aiMode = false;
  return config;
}

std::shared_ptr<FffInstance> FffFileIndexer::currentInstance() const {
  std::lock_guard<std::mutex> lock(m_instanceMtx);
  return m_instance;
}

void FffFileIndexer::setCurrentInstance(std::shared_ptr<FffInstance> instance) {
  std::lock_guard<std::mutex> lock(m_instanceMtx);
  m_instance = std::move(instance);
}

void FffFileIndexer::start() {
  if (currentInstance() && m_scanReady.load()) return;

  if (m_initInProgress.load()) {
    // Stale spawn still draining; ask it to re-spawn on exit.
    m_pendingRestart.store(true);
    return;
  }

  spawnInstance();
}

void FffFileIndexer::stop() {
  m_progressPoll.stop();
  m_generation.fetch_add(1, std::memory_order_acq_rel);
  setCurrentInstance(nullptr);
  m_scanReady.store(false);
  drainPendingEmpty();
  emit scanStateChanged(0, false, false);
}

void FffFileIndexer::rebuildIndex() {
  m_scanReady = false;
  setCurrentInstance(nullptr);
  m_generation.fetch_add(1, std::memory_order_acq_rel);
  if (m_initInProgress.load()) {
    m_pendingRestart.store(true);
    return;
  }
  spawnInstance();
}

void FffFileIndexer::drainPendingEmpty() {
  std::optional<PendingQuery> pending;
  {
    std::lock_guard<std::mutex> lock(m_pendingMtx);
    pending = std::move(m_pendingQuery);
    m_pendingQuery.reset();
  }
  if (!pending || !pending->promise) return;
  pending->promise->start();
  pending->promise->addResult({});
  pending->promise->finish();
}

AbstractFileIndexer::ScanState FffFileIndexer::scanState() const {
  ScanState state{};
  // `m_scanReady` is the truth source: fff's own `is_warmup_complete` is tied
  // to features we disable (content indexing / watcher) and cannot be relied
  // upon here.
  bool const ready = m_scanReady.load();
  bool const inProgress = m_initInProgress.load();
  auto instance = currentInstance();

  if (instance) { state.scannedFilesCount = instance->progress().scannedFilesCount; }

  state.isReady = ready;
  state.isScanning = !ready && (inProgress || instance != nullptr);
  return state;
}

void FffFileIndexer::emitProgressSnapshot() {
  auto state = scanState();
  if (state.scannedFilesCount == m_lastEmittedCount && state.isScanning == m_lastEmittedScanning &&
      state.isReady == m_lastEmittedReady) {
    return;
  }
  m_lastEmittedCount = state.scannedFilesCount;
  m_lastEmittedScanning = state.isScanning;
  m_lastEmittedReady = state.isReady;

  emit scanStateChanged(static_cast<quint64>(state.scannedFilesCount), state.isScanning, state.isReady);

  if (state.isReady && !m_initInProgress.load()) { m_progressPoll.stop(); }
}

void FffFileIndexer::preferenceValuesChanged(const QJsonObject &preferences) {
  auto paths = parsePaths(preferences, "paths");
  fs::path newBase;

  if (paths.empty()) {
    newBase = defaultBasePath();
  } else {
    newBase = paths.front();
    if (paths.size() > 1) {
      qWarning() << "fff: multiple 'paths' entries configured;" << paths.size() - 1
                 << "extra path(s) ignored. fff indexes a single base path. Using:" << newBase.c_str();
    }
  }

  // The "reuse fff.nvim dbs" toggle defaults ON when the preference is
  // missing entirely (e.g. first run before the user has touched it).
  bool newReuse = preferences.contains("reuseNvimDbs") ? preferences.value("reuseNvimDbs").toBool() : true;

  bool changed = false;
  {
    std::lock_guard<std::mutex> lock(m_prefsMtx);
    if (m_basePath != newBase) {
      m_basePath = newBase;
      changed = true;
    }
    if (m_reuseNvimDbs != newReuse) {
      m_reuseNvimDbs = newReuse;
      changed = true;
    }
  }

  if (changed && currentInstance()) {
    m_scanReady = false;
    setCurrentInstance(nullptr);
    m_generation.fetch_add(1, std::memory_order_acq_rel);
    if (m_initInProgress.load()) {
      m_pendingRestart.store(true);
    } else {
      spawnInstance();
    }
  }
}

void FffFileIndexer::spawnInstance() {
  bool expected = false;
  if (!m_initInProgress.compare_exchange_strong(expected, true)) {
    m_pendingRestart.store(true);
    return;
  }

  auto config = buildConfig();
  // Detect whether we ended up pointing at fff.nvim's dbs (so the worker
  // knows to retry with vicinae-local dbs if create() fails).
  bool const usedNvimDbs = [&]() {
    auto stateDir = fffStateDir();
    return config.frecencyDbPath != (stateDir / "frecency") || config.historyDbPath != (stateDir / "history");
  }();
  auto const myGen = m_generation.load(std::memory_order_acquire);

  if (!m_progressPoll.isActive()) m_progressPoll.start();
  emit scanStateChanged(0, true, false);

  QThreadPool::globalInstance()->start([this, myGen, usedNvimDbs, config = std::move(config)]() mutable {
    auto created = FffInstance::create(config);

    // If we tried fff.nvim's dbs and create failed (likely a schema/version
    // mismatch between vicinae's fff v0.7.0 and the user's nvim plugin),
    // retry once with vicinae's own dbs so the user still gets a valid index
    if (!created.has_value() && usedNvimDbs) {
      qWarning() << "fff: failed to open fff.nvim dbs (" << created.error().c_str()
                 << "). Falling back to vicinae-local dbs.";
      auto fallback = buildConfig(/*forceLocalDbs=*/true);
      auto retry = FffInstance::create(fallback);
      if (retry.has_value()) {
        created = std::move(retry);
        config = std::move(fallback);
      }
    }

    auto const finishWorker = [this]() {
      m_initInProgress.store(false);
      if (m_pendingRestart.exchange(false)) {
        QMetaObject::invokeMethod(this, [this]() { spawnInstance(); }, Qt::QueuedConnection);
      } else {
        QMetaObject::invokeMethod(this, [this]() { emitProgressSnapshot(); }, Qt::QueuedConnection);
      }
    };

    if (!created.has_value()) {
      qCritical() << "fff: failed to create instance for base_path" << config.basePath.c_str() << ":"
                  << created.error().c_str();
      drainPendingEmpty();
      finishWorker();
      return;
    }

    if (myGen != m_generation.load(std::memory_order_acquire)) {
      drainPendingEmpty();
      finishWorker();
      return;
    }

    std::shared_ptr<FffInstance> instance = std::move(created.value());

    // Expose the handle now so the main-thread poller can read live progress while the warmup runs.
    setCurrentInstance(instance);

    if (!instance->waitForScan(std::chrono::milliseconds(0))) {
      qWarning() << "fff: wait_for_scan did not report completion for" << config.basePath.c_str();
    }

    // stop() may have fired during the warmup.
    if (myGen != m_generation.load(std::memory_order_acquire)) {
      setCurrentInstance(nullptr);
      drainPendingEmpty();
      finishWorker();
      return;
    }

    qInfo() << "fff: index ready for" << config.basePath.c_str();

    // important to drain the parked query under the same lock
    // so another queryAsync wouldn't have a race window to partially override
    std::optional<PendingQuery> pending;
    {
      std::lock_guard<std::mutex> lock(m_pendingMtx);
      m_scanReady = true;
      pending = std::move(m_pendingQuery);
      m_pendingQuery.reset();
    }

    QMetaObject::invokeMethod(this, [this]() { emitProgressSnapshot(); }, Qt::QueuedConnection);

    if (pending) { runQuery(instance, std::move(*pending)); }

    m_initInProgress.store(false);
    if (m_pendingRestart.exchange(false)) {
      QMetaObject::invokeMethod(this, [this]() { spawnInstance(); }, Qt::QueuedConnection);
    }
  });
}

void FffFileIndexer::runQuery(std::shared_ptr<FffInstance> instance, PendingQuery query) const {
  if (!instance || !query.promise) return;

  QThreadPool::globalInstance()->start([instance = std::move(instance), query = std::move(query)]() mutable {
    FffInstance::SearchOptions opts;
    opts.pageIndex = query.params.pagination.offset / std::max(1, query.params.pagination.limit);
    opts.pageSize = query.params.pagination.limit > 0 ? query.params.pagination.limit : 100;

    auto results = instance->search(query.query, opts);
    query.promise->start();
    query.promise->addResult(std::move(results));
    query.promise->finish();
  });
}

QFuture<std::vector<IndexerFileResult>> FffFileIndexer::queryAsync(std::string_view view,
                                                                   const QueryParams &params) {
  auto promise = std::make_shared<QPromise<std::vector<IndexerFileResult>>>();
  auto future = promise->future();

  PendingQuery pending{.query = std::string(view), .params = params, .promise = promise};

  // Three states to handle:
  // - scan ready  -> dispatch immediatly on the current instance
  // - scan in progress -> park the query, superseding any older parked query
  // - not started -> do nothing, we require explicit ::start() to be called to trigger indexer
  std::shared_ptr<FffInstance> instance;
  std::optional<PendingQuery> superseded;
  bool parked = false;

  {
    std::lock_guard<std::mutex> lock(m_pendingMtx);
    if (m_scanReady.load()) {
      instance = currentInstance();
    } else if (m_initInProgress.load()) {
      superseded = std::move(m_pendingQuery);
      m_pendingQuery = std::move(pending);
      parked = true;
    }
  }

  if (superseded && superseded->promise) {
    superseded->promise->start();
    superseded->promise->addResult({});
    superseded->promise->finish();
  }

  if (parked) return future;

  // if we already started run the actual search
  if (instance) {
    runQuery(std::move(instance), std::move(pending));
    return future;
  }

  promise->start();
  promise->addResult({});
  promise->finish();
  return future;
}
