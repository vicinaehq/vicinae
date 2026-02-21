#pragma once
#include "bridge-view.hpp"
#include "ui/image/url.hpp"

class EmptyViewHost : public FormViewBase {
  Q_OBJECT

public:
  EmptyViewHost(const QString &title, const QString &description, const ImageURL &icon = {});

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;

private:
  QString m_title;
  QString m_description;
  QString m_iconSource;
};
