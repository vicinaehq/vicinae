#pragma once
#include <QFileSystemWatcher>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QQmlAbstractUrlInterceptor>
#include <QTimer>
#include <vector>

class QQmlEngine;

// Dev-mode only (VICINAE_DEV_MODE): serves the Vicinae QML module from the source tree instead
// of the embedded copies, and clears the component cache when a file changes so the next
// component load picks up the edit. Root windows still need a restart.
class QmlDevLoader : public QObject, public QQmlAbstractUrlInterceptor {
  Q_OBJECT

public:
  static void attach(QQmlEngine *engine);

  QUrl intercept(const QUrl &url, DataType type) override;

private:
  explicit QmlDevLoader(QString sourceDir);
  static QmlDevLoader *instance();

  void rescan();
  void reload();

  QString m_sourceDir;
  QHash<QString, QString> m_files;
  QFileSystemWatcher m_watcher;
  QTimer m_debounce;
  std::vector<QPointer<QQmlEngine>> m_engines;
};
