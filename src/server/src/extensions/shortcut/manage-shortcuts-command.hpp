#include <QCoreApplication>
#include "qml/manage-shortcuts-view-host.hpp"
#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"

class ManageShortcutsCommand : public BuiltinViewCommand<ManageShortcutsViewHost> {
  QString id() const override { return "manage"; }
  QString name() const override {
    return QCoreApplication::translate("ManageShortcutsCommand", "Manage Shortcuts");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Link).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
