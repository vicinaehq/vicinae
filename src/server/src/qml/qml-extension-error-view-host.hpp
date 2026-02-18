#pragma once
#include "qml-bridge-view.hpp"

class QmlExtensionErrorViewHost : public QmlFormViewBase {
  Q_OBJECT

public:
  Q_PROPERTY(QString markdown READ markdown CONSTANT)

  explicit QmlExtensionErrorViewHost(const QString &errorText);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString markdown() const;

private:
  QString m_errorText;
  QString m_markdown;
};
