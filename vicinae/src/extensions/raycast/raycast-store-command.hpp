#include "extensions/raycast/store/intro-view.hpp"
#include "single-view-command-context.hpp"
#include "store/store-listing-view.hpp"
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
      ctx->navigation->pushView<RaycastStoreIntroView>();
      return;
    }

    ctx->navigation->pushView<RaycastStoreListingView>();
  }
};
