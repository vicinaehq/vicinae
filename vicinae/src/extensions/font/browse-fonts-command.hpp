#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include "vicinae/browse-fonts-view.hpp"

class BrowseFontsCommand : public BuiltinViewCommand<BrowseFontsView> {
  QString id() const override { return "browse"; }
  QString name() const override { return "Browse Fonts"; };
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("text").setBackgroundTint(SemanticColor::Orange);
  }
};
