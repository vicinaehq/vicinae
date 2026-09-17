#pragma once

#include "services/screenshots/abstract-screenshot-provider.hpp"

class MacosScreenshotProvider : public AbstractScreenshotProvider {
  Q_OBJECT

public:
  using AbstractScreenshotProvider::AbstractScreenshotProvider;
  void refresh() override;
};
