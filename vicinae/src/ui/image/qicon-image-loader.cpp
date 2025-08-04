#include "qicon-image-loader.hpp"

void QIconImageLoader::render(const RenderConfig &config) {
  if (m_icon.isNull()) {
    emit errorOccured("No icon for this name" + m_icon.name());
    return;
  }

  auto sizes = m_icon.availableSizes();
  auto it = std::ranges::max_element(
      sizes, [](QSize a, QSize b) { return a.width() * a.height() < b.width() * b.height(); });

  // most likely SVG, we can request the size we want
  if (it == sizes.end()) {
    emit dataUpdated(m_icon.pixmap(config.size));
    return;
  }

  auto pix =
      m_icon.pixmap(config.size)
          .scaled(config.size * config.devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  pix.setDevicePixelRatio(config.devicePixelRatio);

  emit dataUpdated(pix);
}

QIconImageLoader::QIconImageLoader(const QString &name) {
  m_icon = QIcon(name);
  if (m_icon.isNull()) { m_icon = QIcon::fromTheme(name); }
}
