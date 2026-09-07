#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "builtins/developer/create-extension-view-host.hpp"
#include "theme/theme.hpp"
#include <QCoreApplication>

class CreateExtensionCommand : public BuiltinViewCommand<CreateExtensionViewHost> {
  QString id() const override { return "create"; }
  QString name() const override {
    return QCoreApplication::translate("CreateExtensionCommand", "Create Extension");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Hammer).setBackgroundTint(SemanticColor::Green);
  }
};

class DeveloperExtension : public BuiltinCommandRepository {
  QString id() const override { return "developer"; }
  QString displayName() const override {
    return QCoreApplication::translate("DeveloperExtension", "Developer");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Hammer).setBackgroundTint(SemanticColor::Green);
  }

public:
  DeveloperExtension() { registerCommand<CreateExtensionCommand>(); }
};
