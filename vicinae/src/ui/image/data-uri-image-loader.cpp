#include "data-uri-image-loader.hpp"
#include "data-uri/data-uri.hpp"

DataUriImageLoader::DataUriImageLoader(const QString &url) {
  DataUri uri(url);

  if (!m_tmp.open()) {
    qCritical() << "Failed to open temp file for data URI image";
    return;
  }

  m_tmp.write(uri.decodeContent());
  m_tmp.close();
  m_loader.reset(new LocalImageLoader(m_tmp.filesystemFileName()));

  connect(m_loader.get(), &LocalImageLoader::dataUpdated, this, &LocalImageLoader::dataUpdated);
  connect(m_loader.get(), &LocalImageLoader::errorOccured, this, &LocalImageLoader::errorOccured);
}

void DataUriImageLoader::render(const RenderConfig &config) { m_loader->render(config); }
