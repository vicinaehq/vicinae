#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "qml/qml-shortcut-form-view-host.hpp"
#include "theme.hpp"

class CreateShortcutCommand : public BuiltinViewCommand<QmlShortcutFormViewHost> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Shortcut"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
