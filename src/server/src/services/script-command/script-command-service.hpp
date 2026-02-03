#pragma once
#include "common/types.hpp"
#include "script/script-command-file.hpp"
#include "script/script-metadata-store.hpp"
#include <qfilesystemwatcher.h>
#include <qfuturewatcher.h>
#include <qtimer.h>

class ScriptCommandService : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void scriptsChanged() const;

public:
  using Watcher = QFutureWatcher<std::vector<std::shared_ptr<ScriptCommandFile>>>;

  ScriptCommandService();

  void setCustomScriptPaths(const std::vector<std::filesystem::path> &paths);
  const std::vector<std::filesystem::path> &defaultScriptDirectories() const;
  std::vector<std::filesystem::path> scriptDirectories() const;
  const std::vector<std::shared_ptr<ScriptCommandFile>> &scripts() const;
  void triggerScan();

  ScriptMetadataStore *metadata() { return &m_metadataStore; }
  const ScriptMetadataStore *metadata() const { return &m_metadataStore; }

private:
  void updateWatchedPaths();

  ScriptMetadataStore m_metadataStore;
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  QTimer m_watcherDebounce;
  QTimer m_refreshTimer;
  std::vector<std::shared_ptr<ScriptCommandFile>> m_scripts;
  std::vector<std::filesystem::path> m_customScriptPaths;
  Watcher m_scanWatcher;
  size_t scanCount = 0;
};
