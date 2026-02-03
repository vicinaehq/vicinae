#pragma once
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "create-snippet-command.hpp"
#include "manage-snippets-command.hpp"

class SnippetExtension : public BuiltinCommandRepository {
  QString id() const override { return "snippets"; }
  QString displayName() const override { return "Snippets"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  SnippetExtension() {
    registerCommand<CreateSnippetCommand>();
    registerCommand<ManageSnippetsCommand>();
  }
};
