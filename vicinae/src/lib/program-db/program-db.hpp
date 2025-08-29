#pragma once
#include <qfuturewatcher.h>
#include <qobject.h>
#include <filesystem>
#include <qtmetamacros.h>

class ProgramDb : public QObject {

public:
  using PathList = std::vector<std::filesystem::path>;

  void scanSync();
  void backgroundScan();
  PathList search(const QString &programs, int limit = 50) const;
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
