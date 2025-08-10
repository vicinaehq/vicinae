#include "manage-quicklinks-command.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class ManageShortcutsCommand : public BuiltinViewCommand<ManageBookmarksView> {
  QString id() const override { return "manage"; }
  QString name() const override { return "Manage Shortcuts"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
