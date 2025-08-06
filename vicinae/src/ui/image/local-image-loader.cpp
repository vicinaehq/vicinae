#include "ui/image/image.hpp"
#include "local-image-loader.hpp"

void LocalImageLoader::render(const RenderConfig &cfg) {
  QFile file(m_path);

  file.open(QIODevice::ReadOnly);

  m_loader = std::make_unique<IODeviceImageLoader>(file.readAll());
  m_loader->forwardSignals(this);
  m_loader->render(cfg);
}

LocalImageLoader::LocalImageLoader(const std::filesystem::path &path) { m_path = path; }
