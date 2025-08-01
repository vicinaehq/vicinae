#include "manage-fallback-commands.hpp"
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class ManageFallbackCommand : public BuiltinViewCommand<ManageFallbackCommandsView> {
  QString id() const override { return "manage-fallback"; }
  QString name() const override { return "Configure Fallback Commands"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("undo").setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override {
    return R"("Configure what commands are to be presented as fallback options when nothing matches the
search in the root search.)";
  }
};
