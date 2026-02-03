#pragma once
#include "favicon/favicon-service.hpp"
#include "ui/image/image.hpp"
#include <qfuturewatcher.h>

class FaviconImageLoader : public AbstractImageLoader {
  using Watcher = QFutureWatcher<FaviconService::FaviconResponse>;
  RenderConfig m_config;

  Watcher m_watcher;
  QString m_domain;

  void render(const RenderConfig &config) override;
  void abort() const override;

public:
  FaviconImageLoader(const QString &hostname);
};
