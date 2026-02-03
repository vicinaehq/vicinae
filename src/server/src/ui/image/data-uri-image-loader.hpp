#pragma once
#include "common.hpp"
#include "local-image-loader.hpp"
#include "ui/image/image.hpp"
#include <QtCore>

class DataUriImageLoader : public AbstractImageLoader {
  QTemporaryFile m_tmp;
  QObjectUniquePtr<LocalImageLoader> m_loader;

  void render(const RenderConfig &config) override;

public:
  DataUriImageLoader(const QString &url);
};
