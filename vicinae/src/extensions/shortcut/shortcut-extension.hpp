#pragma once
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "theme.hpp"
#include "create-shortcut-command.hpp"
#include "manage-shortcuts-command.hpp"

class ShortcutExtension : public BuiltinCommandRepository {
  QString id() const override { return "shortcut"; }
  QString displayName() const override { return "Shortcuts"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  ShortcutExtension() {
    registerCommand<CreateShortcutCommand>();
    registerCommand<ManageShortcutsCommand>();
  }
};
