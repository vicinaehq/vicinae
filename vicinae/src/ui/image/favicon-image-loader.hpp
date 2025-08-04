#pragma once
#include "favicon/favicon-request.hpp"
#include "ui/image/image.hpp"

class FaviconImageLoader : public AbstractImageLoader {
  QSharedPointer<AbstractFaviconRequest> m_requester;
  QString m_domain;

  void render(const RenderConfig &config) override;
  void abort() const override;

public:
  FaviconImageLoader(const QString &hostname);
};
