#include "command-database.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "browse-fonts-command.hpp"

class FontExtension : public BuiltinCommandRepository {
  QString id() const override { return "font"; }
  QString name() const override { return "Font"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("text").setBackgroundTint(SemanticColor::Orange);
  }

public:
  FontExtension() { registerCommand<BrowseFontsCommand>(); }
};
