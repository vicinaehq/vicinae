#pragma once
#include "qml/qml-store-intro-view-host.hpp"
#include "qml/qml-vicinae-store-view-host.hpp"
#include "single-view-command-context.hpp"
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
      static const QString INTRO = QStringLiteral(R"(
# Welcome to the Vicinae Extension Store

The Vicinae extension store features community-built extensions that have been approved by the core contributors of the Vicinae project.

Every extension listed here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository.

If you're looking to build your own extension, take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would be a good fit for the store, feel free to submit it!
)");
      auto icon = ImageURL::builtin("cart");
      icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
      ctx->navigation->pushView(new QmlStoreIntroViewHost(INTRO, icon, "Continue to store", [ctrl]() {
        ctrl->storage().setItem("introCompleted", true);
        ctrl->context()->navigation->replaceView<QmlVicinaeStoreViewHost>();
      }));
      return;
    }

    ctx->navigation->pushView(new QmlVicinaeStoreViewHost);
  }
};
