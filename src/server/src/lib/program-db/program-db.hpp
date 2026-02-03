#pragma once
#include "common/scored.hpp"
#include <qfuturewatcher.h>
#include <qobject.h>
#include <filesystem>

class ProgramDb : public QObject {
public:
  using PathList = std::vector<std::filesystem::path>;

  static std::optional<std::filesystem::path> programPath(std::string_view name);

  void scanSync();
  void backgroundScan();
  std::vector<Scored<std::filesystem::path>> search(std::string_view programs, int limit = 50) const;
  const PathList &programs() const;

  ProgramDb();

private:
  Q_OBJECT

  static std::vector<std::filesystem::path> scan();
  using Watcher = QFutureWatcher<PathList>;

  Watcher *m_watcher = new Watcher(this);
  std::vector<std::filesystem::path> m_progs;

signals:
  void backgroundScanFinished() const;
};
