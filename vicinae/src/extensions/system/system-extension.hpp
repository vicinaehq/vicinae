#include "command-database.hpp"
#include "browse-apps/browse-apps-view.hpp"
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

class SystemBrowseApps : public BuiltinViewCommand<BrowseAppsView> {
  QString id() const override { return "browse-apps"; }
  QString name() const override { return "Browse Apps"; }
  QString description() const override { return "Browse all applications that are installed on the system"; }
  std::vector<QString> keywords() const override { return {}; }
  bool isDefaultDisabled() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("box").setBackgroundTint(SemanticColor::Orange);
  }
  std::vector<Preference> preferences() const override {
    auto showHidden = Preference::makeCheckbox("showHidden", "Show hidden apps");
    showHidden.setDefaultValue(false);
    return {showHidden};
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
  SystemExtension() {
    registerCommand<SystemRunCommand>();
    registerCommand<SystemBrowseApps>();
  }

  std::vector<Preference> preferences() const override { return {}; }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {}
};
