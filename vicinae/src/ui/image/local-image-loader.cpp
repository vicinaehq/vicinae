#include "ui/image/image.hpp"
#include "local-image-loader.hpp"

void LocalImageLoader::render(const RenderConfig &cfg) {
  m_loader = std::make_unique<IODeviceImageLoader>(std::unique_ptr<QIODevice>(new QFile(m_path)));
  connect(m_loader.get(), &IODeviceImageLoader::dataUpdated, this, &LocalImageLoader::dataUpdated);
  connect(m_loader.get(), &IODeviceImageLoader::errorOccured, this, &LocalImageLoader::errorOccured);
  m_loader->render(cfg);
}

LocalImageLoader::LocalImageLoader(const std::filesystem::path &path) { m_path = path; }
