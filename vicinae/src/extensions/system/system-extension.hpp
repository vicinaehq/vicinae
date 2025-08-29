#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "run/system-run-view.hpp"
#include "theme.hpp"

class SystemRunCommand : public BuiltinViewCommand<SystemRunView> {
  QString id() const override { return "run"; }
  QString name() const override { return "Run Program"; }
  QString description() const override { return "Run any system program, in typical dmenu fashion"; }
  std::vector<QString> keywords() const override { return {"dmenu", "binary", "execute"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("terminal").setBackgroundTint(SemanticColor::Orange);
  }
};

class SystemExtension : public BuiltinCommandRepository {
  QString id() const override { return "system"; }
  QString displayName() const override { return "System"; }
  QString description() const override { return "System-related commands"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("cog").setBackgroundTint(SemanticColor::Orange);
  }

public:
  SystemExtension() { registerCommand<SystemRunCommand>(); }

  std::vector<Preference> preferences() const override { return {}; }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {}
};
