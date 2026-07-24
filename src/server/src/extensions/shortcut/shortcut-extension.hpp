#pragma once
#include <QCoreApplication>
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "create-shortcut-command.hpp"
#include "manage-shortcuts-command.hpp"

class ShortcutExtension : public BuiltinCommandRepository {
  QString id() const override { return "manage-shortcuts"; }
  QString displayName() const override {
    return QCoreApplication::translate("ShortcutExtension", "Manage Shortcuts");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  ShortcutExtension() {
    registerCommand<CreateShortcutCommand>();
    registerCommand<ManageShortcutsCommand>();
  }
};
