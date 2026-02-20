#pragma once
#include "qml-bridge-view.hpp"
#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include <filesystem>

class QmlCreateExtensionSuccessViewHost : public QmlFormViewBase {
  Q_OBJECT
  Q_PROPERTY(QString markdown READ markdown CONSTANT)

public:
  QmlCreateExtensionSuccessViewHost(const ExtensionBoilerplateConfig &cfg,
                                    const std::filesystem::path &location);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString markdown() const { return m_markdown; }

private:
  QString m_markdown;
  std::filesystem::path m_path;
};
