#pragma once
#include "services/files-service/file-indexer/scan-dispatcher.hpp"
#include <QTimer>
#include <QFileSystemWatcher>
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

/**
 * We watch non-hidden top level home directories to instantly detect changes made to them
 * We also update those directories at a fixed interval following simple heuristics.
 */

class HomeDirectoryWatcher : public QObject {
  QTimer *m_hourlyUpdateTimer = new QTimer(this);
  QTimer *m_importantUpdateTimer = new QTimer(this);
  ScanDispatcher &m_dispatcher;
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  bool m_allowsBackgroundUpdates = true;
  static constexpr size_t BACKGROUND_UPDATE_DEPTH = 5;

  std::vector<std::filesystem::path> getImportantDirectories();

  void rebuildWatch();
  void directoryChanged(const QString &path);
  void dispatchHourlyUpdate();
  void dispatchImportantUpdate();
  void setAllowsBackgroundUpdates(bool value);

  bool allowsBackgroundUpdates() const;

public:
  HomeDirectoryWatcher(ScanDispatcher &dispatcher);
};
