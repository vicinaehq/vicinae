#pragma once

#include <memory>
#include "services/screenshots/abstract-screenshot-provider.hpp"

class MacosScreenshotProvider : public AbstractScreenshotProvider {
  Q_OBJECT

public:
  explicit MacosScreenshotProvider(QObject *parent = nullptr);
  ~MacosScreenshotProvider() override;
  void refresh() override;

private:
  void collect();
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};
