#include "ui/qml-dev-loader.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QQmlEngine>

#ifdef VICINAE_QML_SOURCE_DIR
QmlDevLoader *QmlDevLoader::instance() {
  static auto *loader = new QmlDevLoader(QStringLiteral(VICINAE_QML_SOURCE_DIR));
  return loader;
}
#endif

void QmlDevLoader::attach(QQmlEngine *engine) {
#ifdef VICINAE_QML_SOURCE_DIR
  auto *loader = instance();
  loader->m_engines.emplace_back(engine);
  engine->addUrlInterceptor(loader);
#else
  Q_UNUSED(engine);
#endif
}

QmlDevLoader::QmlDevLoader(QString sourceDir)
    : QObject(QCoreApplication::instance()), m_sourceDir(std::move(sourceDir)) {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(100);
  connect(&m_debounce, &QTimer::timeout, this, &QmlDevLoader::reload);

  auto schedule = [this](const QString &) {
    rescan();
    m_debounce.start();
  };
  connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, schedule);
  connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, schedule);

  rescan();
  qInfo() << "QML dev mode: serving Vicinae module from" << m_sourceDir;
}

QUrl QmlDevLoader::intercept(const QUrl &url, DataType type) {
  if (type != QmlFile && type != JavaScriptFile) return url;
  if (url.scheme() != QLatin1String("qrc") || !url.path().startsWith(QLatin1String("/qt/qml/Vicinae/"))) return url;

  auto it = m_files.constFind(url.fileName());
  if (it == m_files.constEnd()) return url;
  return QUrl::fromLocalFile(*it);
}

void QmlDevLoader::rescan() {
  m_files.clear();

  QStringList paths{m_sourceDir};
  QDirIterator it(m_sourceDir, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

  while (it.hasNext()) {
    const QFileInfo info = it.nextFileInfo();
    if (info.isDir()) {
      paths << info.filePath();
      continue;
    }
    const QString suffix = info.suffix();
    if (suffix != QLatin1String("qml") && suffix != QLatin1String("js")) continue;
    m_files.insert(info.fileName(), info.filePath());
    paths << info.filePath();
  }

  const QStringList watched = m_watcher.files() + m_watcher.directories();
  QStringList missing;
  for (const QString &path : paths) {
    if (!watched.contains(path)) missing << path;
  }
  if (!missing.isEmpty()) m_watcher.addPaths(missing);
}

void QmlDevLoader::reload() {
  for (auto &engine : m_engines) {
    if (engine) engine->clearComponentCache();
  }
  qInfo() << "QML dev mode: component cache cleared, next view load picks up changes";
}
