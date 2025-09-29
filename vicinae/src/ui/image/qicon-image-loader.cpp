#include "qicon-image-loader.hpp"

void QIconImageLoader::render(const RenderConfig &config) {
  QString savedTheme = QIcon::themeName();

  if (m_theme) { QIcon::setThemeName(*m_theme); }

  auto icon = QIcon::fromTheme(m_icon);

  // If icon fails to resolve, try loading it from the filesystem (QIcon does not resolve icons not part of a
  // theme).
  if (icon.isNull()) { icon = loadIconFromFileSystem(m_icon); }

  if (icon.isNull()) {
    emit errorOccured(QString("No icon with name: %1").arg(m_icon));
    QIcon::setThemeName(savedTheme);
    return;
  }

  auto sizes = icon.availableSizes();
  auto it = std::ranges::max_element(
      sizes, [](QSize a, QSize b) { return a.width() * a.height() < b.width() * b.height(); });

  // most likely SVG, we can request the size we want
  if (it == sizes.end()) {
    emit dataUpdated(icon.pixmap(config.size));
    QIcon::setThemeName(savedTheme);
    return;
  }

  auto pix =
      icon.pixmap(config.size)
          .scaled(config.size * config.devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  pix.setDevicePixelRatio(config.devicePixelRatio);
  QIcon::setThemeName(savedTheme);

  emit dataUpdated(pix);
}

// Loads an icon not in a theme directory from the filesystem.
QIcon QIconImageLoader::loadIconFromFileSystem(const QString &iconName) {
  const QStringList searchPaths = {"/usr/share/pixmaps", "/usr/share/icons"};
  const QStringList extensions = {".png", ".svg", ".xpm"};

  // Try each search path with each extension
  for (const QString &path : searchPaths) {
    for (const QString &ext : extensions) {
      QString iconPath = QString("%1/%2%3").arg(path, iconName, ext);
      if (QFile::exists(iconPath)) { return QIcon(iconPath); }
    }
  }

  // In case icon is specified as a full path.
  return QIcon(iconName);
}

QIconImageLoader::QIconImageLoader(const QString &name, const std::optional<QString> &themeName)
    : m_icon(name), m_theme(themeName) {}
