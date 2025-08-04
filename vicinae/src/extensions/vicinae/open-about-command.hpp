#pragma once
#include "common.hpp"
#include "../../ui/image/url.hpp"
#include "settings-controller/settings-controller.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class OpenAboutCommand : public BuiltinCallbackCommand {
  QString id() const override { return "about"; }
  QString name() const override { return "About"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("info-01").setBackgroundTint(SemanticColor::Red);
  }

  void execute(ApplicationContext *ctx) const override { ctx->settings->openTab("About"); }
};
