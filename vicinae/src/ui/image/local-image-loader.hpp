#pragma once
#include "ui/image/image.hpp"
#include "io-image-loader.hpp"

class LocalImageLoader : public AbstractImageLoader {
  std::unique_ptr<IODeviceImageLoader> m_loader;
  std::filesystem::path m_path;

public:
  void render(const RenderConfig &cfg) override;

  LocalImageLoader(const std::filesystem::path &path);
};
