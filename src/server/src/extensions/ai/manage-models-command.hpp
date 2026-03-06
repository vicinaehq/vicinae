#include "qml/manage-models-view-host.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"

class ManageModelsCommand : public BuiltinViewCommand<ManageModelsViewHost> {
  QString id() const override { return "manage-models"; }
  QString name() const override { return "Manage Models"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("computer-chip").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
