#pragma once
#include "command-controller.hpp"
#include "common.hpp"
#include "../../ui/image/url.hpp"
#include "settings-controller/settings-controller.hpp"
#include "single-view-command-context.hpp"

class OpenAboutCommand : public BuiltinCallbackCommand {
  QString id() const override { return "about"; }
  QString name() const override { return "About"; }
  QString description() const override { return "Open the \"About\" tab of the vicinae settings."; }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("info-01").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();

    ctx->navigation->closeWindow();
    ctx->settings->openTab("about");
  }
};
