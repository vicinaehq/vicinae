#pragma once
#include "common.hpp"
#include "../../ui/image/url.hpp"
#include "settings-controller/settings-controller.hpp"
#include "single-view-command-context.hpp"

class OpenAboutCommand : public BuiltinCallbackCommand {
  QString id() const override { return "about"; }
  QString name() const override { return "About"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("info-01").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(const LaunchProps &props, ApplicationContext *ctx) const override {
    ctx->navigation->closeWindow();
    ctx->settings->openTab("about");
  }
};
