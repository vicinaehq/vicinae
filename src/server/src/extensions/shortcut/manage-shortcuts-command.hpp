#include "qml/qml-manage-shortcuts-view-host.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"

class ManageShortcutsCommand : public BuiltinViewCommand<QmlManageShortcutsViewHost> {
  QString id() const override { return "manage"; }
  QString name() const override { return "Manage Shortcuts"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
