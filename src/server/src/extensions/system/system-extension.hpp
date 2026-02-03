#pragma once
#include "command-database.hpp"
#include "browse-apps/browse-apps-view.hpp"
#include "single-view-command-context.hpp"
#include "run/system-run-view.hpp"
#include "utils.hpp"
#include "services/toast/toast-service.hpp"
#include "xdgpp/desktop-entry/exec.hpp"

class SystemRunCommand : public BuiltinCallbackCommand {
  QString id() const override { return "run"; }
  QString name() const override { return "Run Terminal Program"; }
  QString description() const override { return "Run a program in a terminal window"; }
  std::vector<QString> keywords() const override { return {"shell command", "run program"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("terminal").setBackgroundTint(SemanticColor::Orange);
  }
  virtual std::vector<CommandArgument> arguments() const override {
    return {CommandArgument{.name = "command", .placeholder = "command", .required = false}};
  }

  virtual std::vector<Preference> preferences() const override {
    std::vector<Preference::DropdownData::Option> defaultActions = {
        {"Run in terminal", "run-in-terminal"},
        {"Run in terminal (hold)", "run-in-terminal-hold"},
        {"Run directly", "run"},
    };
    Preference defaultAction = Preference::makeDropdown("default-action", defaultActions);

    defaultAction.setTitle("Default Action");
    defaultAction.setDescription("The default action to run on pressing return");
    defaultAction.setDefaultValue("run-in-terminal");

    return {defaultAction};
  }

  void execute(CommandController *ctrl) const override {
    auto ctx = ctrl->context();
    auto toast = ctx->services->toastService();
    auto args = ctrl->launchProps().arguments;

    if (args.empty() || args.front().second.isEmpty()) {
      ctrl->context()->navigation->pushView(new SystemRunView);
      return;
    }

    auto command = args.front().second;
    auto parsedArgs = xdgpp::ExecParser("").parse(command.toStdString());

    if (parsedArgs.empty() || !ProgramDb::programPath(parsedArgs.front())) {
      toast->failure("Not a valid executable");
      return;
    }

    auto appDb = ctx->services->appDb();
    auto argv = Utils::toQStringVec(parsedArgs);

    using DA = SystemRunView::DefaultAction;

    switch (SystemRunView::parseDefaultAction(ctrl->preferenceValues().value("default-action").toString())) {
    case DA::Run:
      appDb->launchRaw(argv);
      break;
    case DA::RunInTerminal:
      appDb->launchTerminalCommand(argv);
      break;
    case DA::RunInTerminalHold:
      appDb->launchTerminalCommand(argv, {.hold = true});
      break;
    }

    ctx->navigation->closeWindow({.clearRootSearch = true});
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
    auto sortAlphabetically = Preference::makeCheckbox("sortAlphabetically", "Sort alphabetically");
    showHidden.setDefaultValue(false);
    sortAlphabetically.setDefaultValue(true);
    return {sortAlphabetically, showHidden};
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
