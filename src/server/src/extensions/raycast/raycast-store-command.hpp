#include "qml/raycast-store-view-host.hpp"
#include "qml/store-intro-view-host.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class RaycastStoreCommand : public BuiltinCallbackCommand {
  QString id() const override { return "store"; }
  QString name() const override { return "Raycast Store"; }
  QString description() const override { return "Install compatible extensions from the Raycast store"; }
  QString extensionId() const override { return "raycast-compat"; }
  QString commandId() const override { return "store"; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("raycast");
    icon.setBackgroundTint(SemanticColor::Red);
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
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.

Please note that many extensions may not fully work at this time. This is either due to missing feature implementations in Vicinae, or extensions relying on macOS-specific tools and APIs.

Vicinae also has its own [extension store](vicinae://extensions/vicinae/vicinae/store), which does not suffer from these limitations.
)");
      auto icon = iconUrl();
      ctx->navigation->pushView(new StoreIntroViewHost(INTRO, icon, "Continue to store", [ctrl]() {
        ctrl->storage().setItem("introCompleted", true);
        ctrl->context()->navigation->replaceView<RaycastStoreViewHost>();
      }));
      return;
    }

    ctx->navigation->pushView<RaycastStoreViewHost>();
  }
};
