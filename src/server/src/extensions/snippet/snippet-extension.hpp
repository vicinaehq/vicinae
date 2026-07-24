#pragma once
#include <QCoreApplication>
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "create-snippet-command.hpp"
#include "manage-snippets-command.hpp"

class SnippetExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(SnippetExtension)
  QString id() const override { return "snippets"; }
  QString displayName() const override { return tr("Snippets"); }
  QString description() const override { return tr("Text expansion and snippet management"); }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  std::vector<Preference> preferences() const override;
  void initialized(const QJsonObject &preferences) const override;
  void preferenceValuesChanged(const QJsonObject &value) const override;

public:
  SnippetExtension() {
    registerCommand<CreateSnippetCommand>();
    registerCommand<ManageSnippetsCommand>();
  }
};
