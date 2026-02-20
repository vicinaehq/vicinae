#pragma once
#include "qml-bridge-view.hpp"
#include "ui/image/url.hpp"

class QmlEmptyViewHost : public QmlFormViewBase {
  Q_OBJECT

public:
  QmlEmptyViewHost(const QString &title, const QString &description, const ImageURL &icon = {});

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;

private:
  QString m_title;
  QString m_description;
  QString m_iconSource;
};
