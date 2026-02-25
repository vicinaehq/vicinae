#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "qml/shortcut-form-view-host.hpp"
#include "theme.hpp"

class CreateShortcutCommand : public BuiltinViewCommand<ShortcutFormViewHost> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Shortcut"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
