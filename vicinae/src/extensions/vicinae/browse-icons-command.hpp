#include "icon-browser-command.hpp"
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class BrowseIconsCommand : public BuiltinViewCommand<IconBrowserView> {
  QString id() const override { return "browse-icons"; }
  QString name() const override { return "Browse builtin icons"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("vicinae").setBackgroundTint(SemanticColor::Red);
  }
};
