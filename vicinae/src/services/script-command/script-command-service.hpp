#pragma once
#include "script/script-command-file.hpp"
#include "script/script-scanner.hpp"
#include "xdgpp/env/env.hpp"

class ScriptCommandService : public QObject {
  Q_OBJECT

signals:
  void scriptsChanged() const;

public:
  using Watcher = QFutureWatcher<std::vector<std::shared_ptr<ScriptCommandFile>>>;

  ScriptCommandService() {
    m_watcherDebounce.setInterval(100);
    m_watcherDebounce.setSingleShot(true);
    updateWatchedPaths();
    triggerScan();
    connect(&m_scanWatcher, &Watcher::finished, this, [this]() {
      m_scripts = std::move(m_scanWatcher.future().takeResult());
      emit scriptsChanged();
    });
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { m_watcherDebounce.start(); });
    connect(&m_watcherDebounce, &QTimer::timeout, this, [this]() {
      qInfo() << "script directory changed, triggering rescan";
      triggerScan();
    });
  }

  void setCustomScriptPaths(const std::vector<std::filesystem::path> &paths) {
    m_customScriptPaths = paths;
    triggerScan();
    updateWatchedPaths();
  }

  std::vector<std::filesystem::path> defaultScriptDirectories() const {
    return xdgpp::dataDirs() | std::views::transform([](auto &&p) { return p / "vicinae" / "scripts"; }) |
           std::ranges::to<std::vector>();
  }

  std::vector<std::filesystem::path> scriptDirectories() const {
    return std::views::concat(m_customScriptPaths, defaultScriptDirectories()) |
           std::ranges::to<std::vector>();
  }

  const std::vector<std::shared_ptr<ScriptCommandFile>> &scripts() const { return m_scripts; }

  void triggerScan() {
    m_scanWatcher.setFuture(
        QtConcurrent::run([dirs = scriptDirectories()]() { return ScriptScanner::scan(dirs); }));
  }

private:
  void updateWatchedPaths() {
    for (const QString &dir : m_watcher->directories()) {
      m_watcher->removePath(dir);
    }
    for (const auto &path : std::views::concat(m_customScriptPaths, defaultScriptDirectories())) {
      m_watcher->addPath(path.c_str());
    }
  }

  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  QTimer m_watcherDebounce;
  std::vector<std::shared_ptr<ScriptCommandFile>> m_scripts;
  std::vector<std::filesystem::path> m_customScriptPaths;
  Watcher m_scanWatcher;
};
