#include "manage-themes-command.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"

class SetThemeCommand : public BuiltinViewCommand<ManageThemesView> {
  QString id() const override { return "set"; }
  QString name() const override { return "Set Theme"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("brush").setBackgroundTint(SemanticColor::Purple);
  }
};
