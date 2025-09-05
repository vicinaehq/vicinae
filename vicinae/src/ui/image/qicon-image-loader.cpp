#include "qicon-image-loader.hpp"
#include <QtConcurrent/QtConcurrent>
#include <qfuturewatcher.h>

// we need a way to know we have loaded a given icon at least one time so that we can avoid
// useless parallelism.
static std::set<QString> cachedIcons;

void QIconImageLoader::render(const RenderConfig &config) {
  if (m_watcher.isRunning()) { m_watcher.cancel(); }

  m_config = config;

  if (cachedIcons.contains(m_icon)) {
    iconLoaded(QIcon::fromTheme(m_icon));
    return;
  }

  // XXX - provided as a temporary workaround. Some systems, especially Nix systems, can have a lot of
  // fallback search paths seemingly causing huge slowdowns when we try load an icon that can't be found.
  m_watcher.setFuture(QtConcurrent::run([icon = m_icon]() { return QIcon::fromTheme(icon); }));
}

void QIconImageLoader::iconLoaded(const QIcon &icon) {
  if (icon.isNull()) {
    emit errorOccured(QString("No icon with name: %1").arg(m_icon));
    return;
  }

  auto pix =
      icon.pixmap(m_config.size)
          .scaled(m_config.size * m_config.devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  pix.setDevicePixelRatio(m_config.devicePixelRatio);
  emit dataUpdated(pix);
}

QIconImageLoader::QIconImageLoader(const QString &name, const std::optional<QString> &themeName)
    : m_icon(name), m_theme(themeName) {
  connect(&m_watcher, &QFutureWatcher<QIcon>::finished, this, [this]() {
    if (m_watcher.isFinished()) {
      cachedIcons.insert(m_icon);
      iconLoaded(m_watcher.result());
    }
  });
}
