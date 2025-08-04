#include "icon-browser-command.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class BrowseIconsCommand : public BuiltinViewCommand<IconBrowserView> {
  QString id() const override { return "browse-icons"; }
  QString name() const override { return "Browse builtin icons"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("vicinae").setBackgroundTint(SemanticColor::Red);
  }
};
