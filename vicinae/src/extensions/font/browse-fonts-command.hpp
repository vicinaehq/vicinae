#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include "vicinae/browse-fonts-view.hpp"

class BrowseFontsCommand : public BuiltinViewCommand<BrowseFontsView> {
  QString id() const override { return "browse"; }
  QString name() const override { return "Browse Fonts"; };
  ImageURL iconUrl() const override {
    return ImageURL::builtin("text").setBackgroundTint(SemanticColor::Orange);
  }
};
