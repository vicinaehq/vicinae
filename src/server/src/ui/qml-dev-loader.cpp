#include "ui/qml-dev-loader.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QQmlEngine>

#ifdef VICINAE_QML_SOURCE_DIR
QmlDevLoader *QmlDevLoader::instance() {
  static auto *loader = new QmlDevLoader(
      {{QStringLiteral("/qt/qml/Vicinae/"), QStringLiteral(VICINAE_QML_SOURCE_DIR)},
       {QStringLiteral("/qt/qml/Vicinae/Documents/"), QStringLiteral(VICINAE_DOCUMENT_QML_SOURCE_DIR)}});
  return loader;
}
#endif

void QmlDevLoader::attach(QQmlEngine *engine, std::function<void()> reloadRoot) {
#ifdef VICINAE_QML_SOURCE_DIR
  auto *loader = instance();
  loader->m_targets.push_back({engine, std::move(reloadRoot)});
  engine->addUrlInterceptor(loader);
#else
  Q_UNUSED(engine);
  Q_UNUSED(reloadRoot);
#endif
}

QmlDevLoader::QmlDevLoader(QHash<QString, QString> sources)
    : QObject(QCoreApplication::instance()), m_sources(std::move(sources)) {
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
  qInfo() << "QML dev mode: serving modules from" << m_sources;
}

QUrl QmlDevLoader::intercept(const QUrl &url, DataType type) {
  if (type != QmlFile && type != JavaScriptFile) return url;
  if (url.scheme() != QLatin1String("qrc")) return url;

  auto it = m_files.constFind(url.path());
  if (it == m_files.constEnd()) return url;
  return QUrl::fromLocalFile(*it);
}

void QmlDevLoader::rescan() {
  m_files.clear();

  QStringList paths;
  for (auto source = m_sources.cbegin(); source != m_sources.cend(); ++source) {
    paths << source.value();
    QDirIterator it(source.value(), QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
      const QFileInfo info = it.nextFileInfo();
      if (info.isDir()) {
        paths << info.filePath();
        continue;
      }
      const QString suffix = info.suffix();
      if (suffix != QLatin1String("qml") && suffix != QLatin1String("js")) continue;
      m_files.insert(source.key() + info.fileName(), info.filePath());
      paths << info.filePath();
    }
  }

  const QStringList watched = m_watcher.files() + m_watcher.directories();
  QStringList missing;
  for (const QString &path : paths) {
    if (!watched.contains(path)) missing << path;
  }
  if (!missing.isEmpty()) m_watcher.addPaths(missing);
}

void QmlDevLoader::reload() {
  for (auto &target : m_targets) {
    if (target.engine) target.engine->clearComponentCache();
  }
  for (auto &target : m_targets) {
    if (target.engine && target.reloadRoot) target.reloadRoot();
  }
  qInfo() << "QML dev mode: reloaded";
}
