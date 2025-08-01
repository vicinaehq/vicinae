#include "manage-quicklinks-command.hpp"
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class ManageShortcutsCommand : public BuiltinViewCommand<ManageBookmarksView> {
  QString id() const override { return "manage"; }
  QString name() const override { return "Manage Shortcuts"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("link").setBackgroundTint(SemanticColor::Red);
  }
};
