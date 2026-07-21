#pragma once
#include "command-controller.hpp"
#include "common.hpp"
#include "../../ui/image/url.hpp"
#include "settings-controller/settings-controller.hpp"
#include "single-view-command-context.hpp"
#include <QCoreApplication>

class OpenAboutCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenAboutCommand)

  QString id() const override { return "about"; }
  QString name() const override { return tr("About"); }
  QString description() const override { return tr("Open the \"About\" tab of the vicinae settings."); }
  std::vector<QString> keywords() const override { return {"About"}; }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("info-01").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();

    ctx->navigation->closeWindow();
    ctx->settings->openTab("about");
  }
};
