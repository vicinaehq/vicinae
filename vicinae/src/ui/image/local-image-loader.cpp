#include "ui/image/image.hpp"
#include "ui/image/io-image-loader.hpp"
#include "ui/image/svg-image-loader.hpp"
#include <qlogging.h>
#include "local-image-loader.hpp"

void LocalImageLoader::render(const RenderConfig &cfg) {
  if (m_path.extension() == ".svg") {
    m_loader = std::make_unique<SvgImageLoader>(QString(m_path.c_str()));
  } else {
    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning() << "Failed to open image file:" << m_path;
      return;
    }
    m_loader = std::make_unique<IODeviceImageLoader>(file.readAll());
  }

  m_loader->forwardSignals(this);
  m_loader->render(cfg);
}

LocalImageLoader::LocalImageLoader(const std::filesystem::path &path) { m_path = path; }
