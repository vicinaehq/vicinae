#include "qml/raycast-store-view-host.hpp"
#include "qml/store-intro-view-host.hpp"
#include "services/raycast/raycast-store.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include <QCoreApplication>

class RaycastStoreCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(RaycastStoreCommand)

  QString id() const override { return "store"; }
  QString name() const override { return tr("Raycast Store"); }
  std::vector<QString> keywords() const override { return {"Raycast Store"}; }
  QString description() const override { return tr("Install compatible extensions from the Raycast store"); }
  QString extensionId() const override { return "raycast-compat"; }
  QString commandId() const override { return "store"; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("raycast");
    icon.setBackgroundTint(SemanticColor::Red);
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
      static const QString INTRO = [] {
        QString intro = tr(R"(
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
)");
        if constexpr (Raycast::hasCompatSheet()) {
          intro += tr(R"(
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
)");
        } else {
          intro += tr(R"(
Vicinae also has its own [extension store](vicinae://launch/core/store).
)");
        }
        return intro;
      }();
      auto icon = iconUrl();
      auto storage = ctrl.storage();
      ctx->navigation->pushView(
          new StoreIntroViewHost(INTRO, icon, tr("Continue to store"), [storage, ctx]() mutable {
            storage.setItem("introCompleted", true);
            ctx->navigation->replaceView<RaycastStoreViewHost>();
          }));
      return;
    }

    ctx->navigation->pushView<RaycastStoreViewHost>();
  }
};
