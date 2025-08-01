#pragma once
#include "command-database.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "create-shortcut-command.hpp"
#include "manage-shortcuts-command.hpp"

class ShortcutExtension : public BuiltinCommandRepository {
  QString id() const override { return "shortcut"; }
  QString name() const override { return "Shortcuts"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("link").setBackgroundTint(SemanticColor::Red);
  }

public:
  ShortcutExtension() {
    registerCommand<CreateShortcutCommand>();
    registerCommand<ManageShortcutsCommand>();
  }
};
