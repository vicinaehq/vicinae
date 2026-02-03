#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "theme.hpp"
#include "browse-fonts-command.hpp"

class FontExtension : public BuiltinCommandRepository {
  QString id() const override { return "font"; }
  QString displayName() const override { return "Font"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("text").setBackgroundTint(SemanticColor::Orange);
  }

public:
  FontExtension() { registerCommand<BrowseFontsCommand>(); }
};
