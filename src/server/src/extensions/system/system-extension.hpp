#pragma once
#include "command-database.hpp"
#include "common/context.hpp"
#include "qml/browse-apps-view-host.hpp"
#include "single-view-command-context.hpp"
#include "qml/system-run-model.hpp"
#include "qml/system-run-view-host.hpp"
#include "theme/colors.hpp"
#include "utils.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include <QCoreApplication>

#ifndef Q_OS_WIN
#include "xdgpp/desktop-entry/exec.hpp"
#include "set-default-terminal-view-host.hpp"
#endif

namespace {

class SystemRunCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(SystemRunCommand)

  QString id() const override { return "run"; }
  QString name() const override { return tr("Run Terminal Program"); }
  QString description() const override { return tr("Run a program in a terminal window"); }
  std::vector<QString> keywords() const override { return {"shell command", "run program"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Terminal).setBackgroundTint(SemanticColor::Cyan);
  }
  CommandMode mode() const override { return CommandMode::CommandModeView; }
  virtual std::vector<CommandArgument> arguments() const override {
    return {CommandArgument{.name = "command", .placeholder = tr("command"), .required = false}};
  }

  virtual std::vector<Preference> preferences() const override {
    std::vector<Preference::DropdownData::Option> defaultActions = {
        {tr("Run in terminal"), "run-in-terminal"},
        {tr("Run in terminal (hold)"), "run-in-terminal-hold"},
        {tr("Run directly"), "run"},
    };
    Preference defaultAction = Preference::makeDropdown("default-action", defaultActions);

    defaultAction.setTitle(tr("Default Action"));
    defaultAction.setDescription(tr("The default action to run on pressing return"));
    defaultAction.setDefaultValue("run-in-terminal");

    return {defaultAction};
  }

  void execute(CommandController &ctrl) const override {
    auto ctx = ctrl.context();
    auto toast = ctx->services->toastService();
    auto args = ctrl.launchProps().arguments;

    if (args.empty() || args.front().second.isEmpty()) {
      ctrl.context()->navigation->pushView(new SystemRunViewHost);
      return;
    }

    auto command = args.front().second;
#ifdef Q_OS_WIN
    std::vector<std::string> parsedArgs;
    {
      std::istringstream iss(command.toStdString());
      for (std::string tok; iss >> tok;)
        parsedArgs.emplace_back(std::move(tok));
    }
#else
    auto parsedArgs = xdgpp::ExecParser("").parse(command.toStdString());
#endif

    if (parsedArgs.empty() || !ProgramDb::programPath(parsedArgs.front())) {
      toast->failure(tr("Not a valid executable"));
      return;
    }

    auto appDb = ctx->services->appDb();
    auto argv = Utils::toQStringVec(parsedArgs);

    using DA = SystemRunDefaultAction;

    switch (parseSystemRunDefaultAction(ctrl.preferenceValues().value("default-action").toString())) {
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

class SystemBrowseApps : public BuiltinViewCommand<BrowseAppsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SystemBrowseApps)

  QString id() const override { return "browse-apps"; }
  QString name() const override { return tr("Browse Apps"); }
  QString description() const override {
    return tr("Browse all applications that are installed on the system");
  }
  std::vector<QString> keywords() const override { return {}; }
  bool isDefaultDisabled() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Box).setBackgroundTint(SemanticColor::Cyan);
  }
  std::vector<Preference> preferences() const override {
    auto showHidden = Preference::makeCheckbox("showHidden", tr("Show hidden apps"));
    auto sortAlphabetically = Preference::makeCheckbox("sortAlphabetically", tr("Sort alphabetically"));
    showHidden.setDefaultValue(false);
    sortAlphabetically.setDefaultValue(true);
    return {sortAlphabetically, showHidden};
  }
};

#ifdef Q_OS_LINUX
class SetDefaultTerminal : public BuiltinViewCommand<SetDefaultTerminalViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SetDefaultTerminal)

  QString id() const override { return "set-default-terminal"; }
  QString name() const override { return tr("Set Default Terminal"); }
  QString description() const override { return tr("Change the default system terminal"); }
  std::vector<QString> keywords() const override { return {}; }
  ImageURL iconUrl() const override {
    return ImageURL::symbol("$").setBackgroundTint(QColor(128, 132, 138));
  }
};
#endif

} // namespace

class SystemExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(SystemExtension)

  QString id() const override { return "system"; }
  QString displayName() const override { return tr("System"); }
  QString description() const override { return tr("System-related commands"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Cog).setBackgroundTint(SemanticColor::Cyan);
  }

public:
  SystemExtension() {
    registerCommand<SystemRunCommand>();
    registerCommand<SystemBrowseApps>();

#ifdef Q_OS_LINUX
    // set default terminal using xdg-terminal-exec
    registerCommand<SetDefaultTerminal>();
#endif
  }

  std::vector<Preference> preferences() const override { return {}; }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {}
};
