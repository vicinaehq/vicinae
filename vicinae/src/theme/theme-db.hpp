#pragma once
#include "common.hpp"
#include "theme-file.hpp"
#include <filesystem>
#include <QDebug>
#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <unordered_map>

class QFileSystemWatcher;

class ThemeDatabase : public QObject {
  Q_OBJECT

signals:
  void themeChanged(const ThemeFile &theme) const;

public:
  using ThemeMap = std::unordered_map<QString, std::shared_ptr<ThemeFile>>;
  using ThemeList = std::vector<std::shared_ptr<ThemeFile>>;

  static std::vector<std::filesystem::path> defaultSearchPaths();

  ThemeDatabase();
  std::vector<std::filesystem::path> searchPaths() const { return m_searchPaths; }
  void setSearchPaths(const std::vector<std::filesystem::path> &paths) { m_searchPaths = paths; }
  void scan();

  const ThemeFile *theme(const QString &id);
  ThemeList themes() const;

private:
  void directoryChanged(const QString &path);
  void reinstallWatches();
  void scanPath(const std::filesystem::path &path);

  std::vector<std::filesystem::path> m_searchPaths;
  QObjectUniquePtr<QFileSystemWatcher> m_watcher;
  ThemeList m_themes;
  QTimer m_watcherDebounce;
};
