#include "script-command-service.hpp"
#include "script/script-scanner.hpp"
#include "xdgpp/env/env.hpp"
#include <algorithm>
#include <ranges>
#include <QtConcurrentRun>

ScriptCommandService::ScriptCommandService() {
  using namespace std::chrono_literals;

  m_refreshTimer.setInterval(15min);
  m_refreshTimer.start();
  m_watcherDebounce.setInterval(100ms);
  m_watcherDebounce.setSingleShot(true);

  connect(&m_scanWatcher, &Watcher::finished, this, [this]() {
    m_scripts = std::move(m_scanWatcher.future().takeResult());
    emit scriptsChanged();
  });
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { m_watcherDebounce.start(); });
  connect(&m_refreshTimer, &QTimer::timeout, this, [this]() { triggerScan(); });
  connect(&m_watcherDebounce, &QTimer::timeout, this, [this]() {
    qInfo() << "script directory changed, triggering rescan";
    triggerScan();
  });
}

void ScriptCommandService::setCustomScriptPaths(const std::vector<std::filesystem::path> &paths) {
  if (scanCount > 0 && std::ranges::equal(m_customScriptPaths, paths)) return;

  m_customScriptPaths = paths;
  triggerScan();
  updateWatchedPaths();
}

const std::vector<std::filesystem::path> &ScriptCommandService::defaultScriptDirectories() const {
  static const auto dirs = xdgpp::commonDataDirs() |
                           std::views::transform([](auto &&p) { return p / "vicinae" / "scripts"; }) |
                           std::ranges::to<std::vector>();
  return dirs;
}

std::vector<std::filesystem::path> ScriptCommandService::scriptDirectories() const {
  return std::views::concat(m_customScriptPaths, defaultScriptDirectories()) | std::ranges::to<std::vector>();
}

const std::vector<std::shared_ptr<ScriptCommandFile>> &ScriptCommandService::scripts() const {
  return m_scripts;
}

void ScriptCommandService::triggerScan() {
  m_refreshTimer.start();
  m_scanWatcher.setFuture(
      QtConcurrent::run([dirs = scriptDirectories()]() { return ScriptScanner::scan(dirs); }));
  ++scanCount;
}

void ScriptCommandService::updateWatchedPaths() {
  for (const QString &dir : m_watcher->directories()) {
    m_watcher->removePath(dir);
  }
  for (const auto &path : std::views::concat(m_customScriptPaths, defaultScriptDirectories())) {
    m_watcher->addPath(path.c_str());
  }
}
