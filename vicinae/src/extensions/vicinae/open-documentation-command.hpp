#pragma once
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class OpenDocumentationCommand : public BuiltinCallbackCommand {
  QString id() const override { return "documentation"; }
  QString name() const override { return "Open documentation"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("book").setBackgroundTint(SemanticColor::Red);
  }

  void execute(ApplicationContext *ctx) const override;
};
