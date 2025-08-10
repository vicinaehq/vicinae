#pragma once
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class OpenDocumentationCommand : public BuiltinCallbackCommand {
  QString id() const override { return "documentation"; }
  QString name() const override { return "Open documentation"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("book").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(ApplicationContext *ctx) const override;
};
