#include "command-database.hpp"
#include "omni-icon.hpp"
#include "set-theme-command.hpp"
#include "theme.hpp"

class ThemeExtension : public BuiltinCommandRepository {
  QString id() const override { return "theme"; }
  QString name() const override { return "Theme"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("brush").setBackgroundTint(SemanticColor::Purple);
  }

public:
  ThemeExtension() { registerCommand<SetThemeCommand>(); }
};
