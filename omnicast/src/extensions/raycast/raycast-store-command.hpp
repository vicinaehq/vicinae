#include "single-view-command-context.hpp"
#include "store/store-listing-view.hpp"

class RaycastStoreCommand : public AbstractViewCommand<RaycastStoreListingView> {
  QString uniqueId() const override { return "raycast-compat.store"; }
  QString name() const override { return "Raycast Store"; }
  QString description() const override { return "Install compatible extensions from the Raycast store"; }
  QString extensionId() const override { return "raycast-compat"; }
  QString commandId() const override { return "store"; }
  OmniIconUrl iconUrl() const override {
    auto icon = BuiltinOmniIconUrl("raycast");
    icon.setBackgroundTint(SemanticColor::Red);
    return icon;
  }
  std::vector<Preference> preferences() const override { return {}; }
};
