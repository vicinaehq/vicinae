#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "create-quicklink-command.hpp"
#include "theme.hpp"

class CreateShortcutCommand : public BuiltinViewCommand<ShortcutFormView> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Shortcut"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
