#pragma once
#include "bridge-view.hpp"
#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include <filesystem>

class CreateExtensionSuccessViewHost : public FormViewBase {
  Q_OBJECT
  Q_PROPERTY(QString markdown READ markdown CONSTANT)

public:
  CreateExtensionSuccessViewHost(const ExtensionBoilerplateConfig &cfg,
                                    const std::filesystem::path &location);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString markdown() const { return m_markdown; }

private:
  QString m_markdown;
  std::filesystem::path m_path;
};
