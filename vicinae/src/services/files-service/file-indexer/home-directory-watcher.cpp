#include "home-directory-watcher.hpp"
#include "file-indexer.hpp"
#include "scan.hpp"
#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include "utils/utils.hpp"
#include <chrono>
#include <QDebug>

namespace fs = std::filesystem;

void HomeDirectoryWatcher::directoryChanged(const QString &pathStr) {
  fs::path path(pathStr.toStdString());

  m_dispatcher.enqueue({.type = ScanType::Incremental, .path = path, .maxDepth = 1});

  if (path == homeDir()) { rebuildWatch(); }
}

void HomeDirectoryWatcher::rebuildWatch() {
  auto home = homeDir();
  FileSystemWalker walker;

  walker.setRecursive(false);
  walker.setIgnoreHiddenPaths(true);

  for (const auto &dir : m_watcher->directories()) {
    m_watcher->removePath(dir);
  }

  m_watcher->addPath(home.c_str());

  walker.walk(home, [&](auto &&entry) {
    if (!entry.is_directory()) return;
    qDebug() << "watching path" << entry.path().c_str();
    m_watcher->addPath(entry.path().string().c_str());
  });
}

void HomeDirectoryWatcher::dispatchHourlyUpdate() {
  if (!m_allowsBackgroundUpdates) return;

  for (const auto &dir : m_watcher->directories()) {
    m_dispatcher.enqueue(
        {.type = ScanType::Incremental, .path = dir.toStdString(), .maxDepth = BACKGROUND_UPDATE_DEPTH});
  }
}

std::vector<std::filesystem::path> HomeDirectoryWatcher::getImportantDirectories() {
  return {documentsFolder(), downloadsFolder()};
}

void HomeDirectoryWatcher::setAllowsBackgroundUpdates(bool value) { m_allowsBackgroundUpdates = value; }

bool HomeDirectoryWatcher::allowsBackgroundUpdates() const { return m_allowsBackgroundUpdates; }

void HomeDirectoryWatcher::dispatchImportantUpdate() {
  if (!m_allowsBackgroundUpdates) return;

  for (const auto &dir : getImportantDirectories()) {
    if (m_watcher->directories().contains(dir.c_str())) {
      // 5 max depth
      m_dispatcher.enqueue({.type = ScanType::Incremental, .path = dir, .maxDepth = BACKGROUND_UPDATE_DEPTH});
    }
  }
}

HomeDirectoryWatcher::HomeDirectoryWatcher(ScanDispatcher &dispatcher) : m_dispatcher(dispatcher) {
  using namespace std::chrono_literals;

  m_importantUpdateTimer->setInterval(10min);
  m_hourlyUpdateTimer->setInterval(1h);
  m_importantUpdateTimer->start();
  m_hourlyUpdateTimer->start();
  rebuildWatch();

  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &HomeDirectoryWatcher::directoryChanged);
  connect(m_hourlyUpdateTimer, &QTimer::timeout, this, &HomeDirectoryWatcher::dispatchHourlyUpdate);
}
