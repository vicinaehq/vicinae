#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "set-theme-command.hpp"
#include "theme.hpp"

class ThemeExtension : public BuiltinCommandRepository {
  QString id() const override { return "theme"; }
  QString displayName() const override { return "Theme"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("brush").setBackgroundTint(SemanticColor::Purple);
  }

public:
  ThemeExtension() { registerCommand<SetThemeCommand>(); }
};
