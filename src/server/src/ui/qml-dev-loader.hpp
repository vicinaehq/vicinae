#pragma once
#include <QFileSystemWatcher>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QQmlAbstractUrlInterceptor>
#include <QTimer>
#include <functional>
#include <vector>

class QQmlEngine;

// VICINAE_DEV_MODE only: serves the Vicinae module from the source tree and reloads on change
class QmlDevLoader : public QObject, public QQmlAbstractUrlInterceptor {
  Q_OBJECT

public:
  static void attach(QQmlEngine *engine, std::function<void()> reloadRoot = {});

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
  struct Target {
    QPointer<QQmlEngine> engine;
    std::function<void()> reloadRoot;
  };
  std::vector<Target> m_targets;
};
