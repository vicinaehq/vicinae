// Headless smoke test for the FffFileIndexer integration.
//
// Builds a small indexer pointed at the vicinae repo, waits for the scan to
// finish, runs a query for a path we know must exist, and asserts that at
// least one result came back. Exits non-zero on failure.
//
// Not part of the main executable. Built only when -DBUILD_FFF_SMOKE=ON.

#include <QCoreApplication>
#include <QDebug>
#include <QFutureWatcher>
#include <QJsonObject>
#include <QTimer>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/fff/fff-file-indexer.hpp"
#include "services/files-service/fff/fff-library.hpp"

namespace fs = std::filesystem;

namespace {

#define LOG(fmt, ...)                                                                                        \
  do {                                                                                                       \
    std::printf("[smoke] " fmt "\n", ##__VA_ARGS__);                                                         \
    std::fflush(stdout);                                                                                     \
  } while (0)
#define FAIL(fmt, ...)                                                                                       \
  do {                                                                                                       \
    std::fprintf(stderr, "[smoke][FAIL] " fmt "\n", ##__VA_ARGS__);                                          \
    std::fflush(stderr);                                                                                     \
  } while (0)

int run(QCoreApplication &app, const fs::path &base) {
  using namespace std::chrono_literals;

  LOG("base_path = %s", base.c_str());

  // 1. Exercise the low-level RAII wrapper directly.
  {
    vicinae::fff::FffInstance::Config cfg;
    cfg.basePath = base;
    cfg.frecencyDbPath = "";
    cfg.historyDbPath = "";

    auto created = vicinae::fff::FffInstance::create(cfg);
    if (!created.has_value()) {
      FAIL("FffInstance::create failed: %s", created.error().c_str());
      return 1;
    }

    auto &instance = created.value();
    LOG("waiting for initial scan (up to 60s)...");
    if (!instance->waitForScan(60s)) {
      FAIL("wait_for_scan timed out");
      return 1;
    }

    auto progress = instance->progress();
    LOG("scanned=%llu warmup_complete=%d", static_cast<unsigned long long>(progress.scannedFilesCount),
        progress.isWarmupComplete ? 1 : 0);

    vicinae::fff::FffInstance::SearchOptions opts;
    opts.pageSize = 20;
    auto direct = instance->search("fff-library", opts);
    LOG("FffInstance::search(\"fff-library\") returned %zu results", direct.size());

    bool foundHpp = false;
    bool foundCpp = false;
    for (const auto &r : direct) {
      auto name = r.path.filename().string();
      LOG("  - %s (rank=%.1f)", r.path.c_str(), r.rank);
      if (name == "fff-library.hpp") foundHpp = true;
      if (name == "fff-library.cpp") foundCpp = true;
    }
    if (!foundHpp || !foundCpp) {
      FAIL("expected fff-library.{hpp,cpp} via FffInstance; hpp=%d cpp=%d", foundHpp, foundCpp);
      return 1;
    }
    LOG("PASS: FffInstance direct search");
  }

  // 2. Exercise the QObject-based FffFileIndexer end-to-end.
  auto indexer = std::make_unique<FffFileIndexer>();

  QJsonObject prefs;
  prefs.insert("paths", QString::fromStdString(base.string()));
  indexer->preferenceValuesChanged(prefs);
  indexer->start();

  int exitCode = 0;
  bool completed = false;

  QFutureWatcher<std::vector<IndexerFileResult>> watcher;
  QObject::connect(&watcher, &QFutureWatcher<std::vector<IndexerFileResult>>::finished, &app, [&]() {
    auto results = watcher.result();
    LOG("FffFileIndexer::queryAsync returned %zu results", results.size());

    bool found = false;
    for (const auto &r : results) {
      LOG("  - %s (rank=%.1f)", r.path.c_str(), r.rank);
      if (r.path.filename() == "fff-file-indexer.cpp") { found = true; }
    }

    if (!found) {
      FAIL("FffFileIndexer did not return fff-file-indexer.cpp");
      exitCode = 1;
    } else {
      LOG("PASS: FffFileIndexer queryAsync");
    }

    completed = true;
    app.quit();
  });

  // Overall budget: 90 seconds. First open of fff on a cold tree can be slow.
  QTimer::singleShot(90'000, &app, [&]() {
    if (!completed) {
      FAIL("indexer query did not complete within timeout");
      exitCode = 1;
      app.quit();
    }
  });

  LOG("kicking off FffFileIndexer::queryAsync(\"fff-file-indexer\")");
  auto future = indexer->queryAsync("fff-file-indexer");
  watcher.setFuture(future);

  app.exec();
  return exitCode;
}

// Verifies the "park latest, drop older" behavior of FffFileIndexer.
// Strategy: start() the indexer, then synchronously fire 3 queries before the
// worker thread can flip m_scanReady. With the supersede semantics, q1 and q2
// should resolve with empty results (they were displaced) and q3 should be
// dispatched once the scan finishes.
int runSupersedeTest(QCoreApplication &app, const fs::path &base) {
  LOG("--- supersede test: 3 queries during warmup ---");

  auto indexer = std::make_unique<FffFileIndexer>();
  QJsonObject prefs;
  prefs.insert("paths", QString::fromStdString(base.string()));
  indexer->preferenceValuesChanged(prefs);
  indexer->start();

  // Fire 3 queries back-to-back. Because spawnInstance() sets
  // m_initInProgress=true synchronously before the worker is queued, all 3
  // calls land while the indexer is still warming up.
  auto fut1 = indexer->queryAsync("fff-file-indexer");
  auto fut2 = indexer->queryAsync("fff-library");
  auto fut3 = indexer->queryAsync("fff-file-indexer");

  int exitCode = 0;
  bool done1 = false, done2 = false, done3 = false;
  std::size_t n1 = 0, n2 = 0, n3 = 0;
  bool latestFoundExpected = false;

  auto checkAllDone = [&]() {
    if (!(done1 && done2 && done3)) return;

    LOG("supersede: q1 (superseded) -> %zu results", n1);
    LOG("supersede: q2 (superseded) -> %zu results", n2);
    LOG("supersede: q3 (latest)     -> %zu results", n3);

    bool ok = true;
    if (n1 != 0) {
      FAIL("expected q1 (superseded) to return 0 results, got %zu", n1);
      ok = false;
    }
    if (n2 != 0) {
      FAIL("expected q2 (superseded) to return 0 results, got %zu", n2);
      ok = false;
    }
    if (n3 == 0) {
      FAIL("expected q3 (latest) to return some results, got 0");
      ok = false;
    }
    if (!latestFoundExpected) {
      FAIL("expected q3 results to contain fff-file-indexer.cpp");
      ok = false;
    }
    if (ok) {
      LOG("PASS: supersede behavior verified");
    } else {
      exitCode = 1;
    }
    app.quit();
  };

  using Watcher = QFutureWatcher<std::vector<IndexerFileResult>>;
  Watcher w1, w2, w3;

  QObject::connect(&w1, &Watcher::finished, &app, [&]() {
    n1 = w1.result().size();
    done1 = true;
    checkAllDone();
  });
  QObject::connect(&w2, &Watcher::finished, &app, [&]() {
    n2 = w2.result().size();
    done2 = true;
    checkAllDone();
  });
  QObject::connect(&w3, &Watcher::finished, &app, [&]() {
    auto results = w3.result();
    n3 = results.size();
    for (const auto &r : results) {
      if (r.path.filename() == "fff-file-indexer.cpp") {
        latestFoundExpected = true;
        break;
      }
    }
    done3 = true;
    checkAllDone();
  });

  w1.setFuture(fut1);
  w2.setFuture(fut2);
  w3.setFuture(fut3);

  QTimer::singleShot(90'000, &app, [&]() {
    if (!(done1 && done2 && done3)) {
      FAIL("supersede test did not complete within timeout (done1=%d done2=%d done3=%d)", done1, done2,
           done3);
      exitCode = 1;
      app.quit();
    }
  });

  app.exec();
  return exitCode;
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);

  fs::path base = argc >= 2 ? fs::path(argv[1]) : fs::current_path();
  std::printf("fff smoke: starting (base=%s)\n", base.c_str());
  std::fflush(stdout);
  int rc = run(app, base);
  if (rc == 0) { rc = runSupersedeTest(app, base); }
  std::printf("fff smoke: done rc=%d\n", rc);
  std::fflush(stdout);
  return rc;
}
