#pragma once
#include "qml/store-intro-view-host.hpp"
#include "qml/vicinae-store-view-host.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include <QCoreApplication>

class VicinaeStoreCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(VicinaeStoreCommand)

  QString id() const override { return "store"; }
  QString name() const override { return QStringLiteral("Extension Store"); }
  QString description() const override { return tr("Install extensions from the Vicinae store"); }
  std::vector<QString> keywords() const override { return {"Extension Store"}; }
  QString extensionId() const override { return "vicinae"; }
  QString commandId() const override { return "store"; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("cart");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
  std::vector<Preference> preferences() const override {
    auto alwaysShowIntro = Preference::makeCheckbox("alwaysShowIntro", tr("Always show intro"));
    alwaysShowIntro.setDefaultValue(false);
    return {alwaysShowIntro};
  }

  void execute(CommandController &ctrl) const override {
    auto ctx = ctrl.context();
    auto alwaysShowIntro = ctrl.preferenceValues().value("alwaysShowIntro").toBool(false);

    if (alwaysShowIntro || !ctrl.storage().getItem("introCompleted").toBool()) {
      static const QString INTRO = tr(R"(
# Welcome to the vicinae extension store

The vicinae extension store features community-built extensions that have been approved by our core contributors.

Every extension listed here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository.

If you're looking to build your own extension, take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would be a good fit for the store, feel free to submit it!
)");
      auto icon = ImageURL::builtin("cart");
      icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
      auto storage = ctrl.storage();
      ctx->navigation->pushView(
          new StoreIntroViewHost(INTRO, icon, tr("Continue to store"), [storage, ctx]() mutable {
            storage.setItem("introCompleted", true);
            ctx->navigation->replaceView<VicinaeStoreViewHost>();
          }));
      return;
    }

    ctx->navigation->pushView(new VicinaeStoreViewHost);
  }
};
