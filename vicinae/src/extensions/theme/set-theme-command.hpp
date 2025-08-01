#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"

class SetThemeCommand : public BuiltinViewCommand<ManageThemesView> {
  QString id() const override { return "set"; }
  QString name() const override { return "Set Theme"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("brush").setBackgroundTint(SemanticColor::Purple);
  }
};
