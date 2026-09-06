#pragma once
#include <QCoreApplication>
#include "command/command-database.hpp"
#include "extensions/shortcut/manage-shortcuts-view-host.hpp"
#include "ui/image/url.hpp"
#include "command/single-view-command-context.hpp"
#include "extensions/shortcut/shortcut-form-view-host.hpp"

namespace {
class CreateShortcutCommand : public BuiltinViewCommand<ShortcutFormViewHost> {
  QString id() const override { return "create"; }
  QString name() const override {
    return QCoreApplication::translate("CreateShortcutCommand", "Create Shortcut");
  }

  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Bolt).setBackgroundTint(SemanticColor::Purple);
  }
};

class ManageShortcutsCommand : public BuiltinViewCommand<ManageShortcutsViewHost> {
  QString id() const override { return "manage"; }
  QString name() const override {
    return QCoreApplication::translate("ManageShortcutsCommand", "Manage Shortcuts");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Bolt).setBackgroundTint(SemanticColor::Purple);
  }
};
} // namespace

class ShortcutExtension : public BuiltinCommandRepository {
  QString id() const override { return "manage-shortcuts"; }
  QString displayName() const override {
    return QCoreApplication::translate("ShortcutExtension", "Manage Shortcuts");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Bolt).setBackgroundTint(SemanticColor::Purple);
  }

public:
  ShortcutExtension() {
    registerCommand<CreateShortcutCommand>();
    registerCommand<ManageShortcutsCommand>();
  }
};
