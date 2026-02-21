#pragma once
#include "bridge-view.hpp"

class ExtensionErrorViewHost : public FormViewBase {
  Q_OBJECT

public:
  Q_PROPERTY(QString markdown READ markdown CONSTANT)

  explicit ExtensionErrorViewHost(const QString &errorText);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString markdown() const;

private:
  QString m_errorText;
  QString m_markdown;
};
