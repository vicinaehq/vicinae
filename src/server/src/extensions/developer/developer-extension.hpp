#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "qml/create-extension-view-host.hpp"
#include "theme.hpp"

class CreateExtensionCommand : public BuiltinViewCommand<CreateExtensionViewHost> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Extension"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("hammer").setBackgroundTint(SemanticColor::Green);
  }
};

class DeveloperExtension : public BuiltinCommandRepository {
  QString id() const override { return "developer"; }
  QString displayName() const override { return "Developer"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("hammer").setBackgroundTint(SemanticColor::Green);
  }

public:
  DeveloperExtension() { registerCommand<CreateExtensionCommand>(); }
};
