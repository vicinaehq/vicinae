#pragma once
#include "extensions/vicinae/store/intro-view.hpp"
#include "single-view-command-context.hpp"
#include "store/store-listing-view.hpp"
#include "vicinae.hpp"

class VicinaeStoreCommand : public BuiltinCallbackCommand {
  QString id() const override { return "store"; }
  QString name() const override { return "Extension Store"; }
  QString description() const override { return "Install extensions from the Vicinae store"; }
  QString extensionId() const override { return "vicinae"; }
  QString commandId() const override { return "store"; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("cart");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
  std::vector<Preference> preferences() const override {
    auto alwaysShowIntro = Preference::makeCheckbox("alwaysShowIntro", "Always show intro");
    alwaysShowIntro.setDefaultValue(false);
    return {alwaysShowIntro};
  }

  void execute(CommandController *ctrl) const override {
    auto ctx = ctrl->context();
    auto alwaysShowIntro = ctrl->preferenceValues().value("alwaysShowIntro").toBool(false);

    if (alwaysShowIntro || !ctrl->storage().getItem("introCompleted").toBool()) {
      ctx->navigation->pushView(new VicinaeStoreIntroView);
      return;
    }

    ctx->navigation->pushView(new VicinaeStoreListingView);
  }
};
