#pragma once
#include "command/command-database.hpp"
#include "common/context.hpp"
#include "builtins/system/browse-apps-preferences.hpp"
#include "builtins/system/browse-apps-view-host.hpp"
#include "command/single-view-command-context.hpp"
#include "command/typed-command.hpp"
#include "builtins/system/system-run-model.hpp"
#include "builtins/system/system-run-view-host.hpp"
#include "theme/colors.hpp"
#include "utils.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "builtins/system/trash.hpp"
#include <QCoreApplication>

#ifndef Q_OS_WIN
#include "xdgpp/desktop-entry/exec.hpp"
#include "builtins/system/set-default-browser-view-host.hpp"
#include "builtins/system/set-default-terminal-view-host.hpp"
#endif

struct EmptyTrashPreferences {
  bool confirm = true;
};

template <> struct PreferenceSchema<EmptyTrashPreferences> {
  Q_DECLARE_TR_FUNCTIONS(EmptyTrashPreferences)
public:
  PreferenceMeta confirm{.label = tr("Ask for confirmation")};
};

namespace {

class EmptyTrashCommand : public TypedCallbackCommand<EmptyTrashPreferences> {
  Q_DECLARE_TR_FUNCTIONS(EmptyTrashCommand)

  QString id() const override { return "empty-trash"; }
  QString name() const override { return tr("Empty Trash"); }
  QString description() const override { return tr("Permanently delete all items in the Trash"); }
  std::vector<QString> keywords() const override {
    return {"trash", "recycle bin", "bin", "wastebasket", "clear trash"};
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Trash).setBackgroundTint(SemanticColor::Red);
  }

  void execute(const Controller &ctrl) const override {
    auto ctx = ctrl.context();
    auto &nav = ctx->navigation;
    bool const shouldConfirm = ctrl.preferences().confirm;

    auto doEmpty = [ctx]() {
      auto toast = ctx->services->toastService();
      std::string error;
      if (SystemTrash::emptyTrash(error)) {
        toast->success(tr("Trash emptied"));
      } else {
        toast->failure(error.empty() ? tr("Failed to empty trash") : QString::fromStdString(error));
      }
    };

    if (shouldConfirm) {
      nav->confirmAlert(tr("Empty Trash"),
                        tr("Are you sure you want to permanently delete all items in the Trash?"),
                        [&nav, doEmpty]() {
                          nav->closeWindow({.clearRootSearch = true});
                          doEmpty();
                        });
      return;
    }

    nav->closeWindow({.clearRootSearch = true});
    doEmpty();
  }
};

class OpenTrashCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenTrashCommand)

  QString id() const override { return "open-trash"; }
  QString name() const override { return tr("Open Trash"); }
  QString description() const override { return tr("Open the Trash in the default file manager"); }
  std::vector<QString> keywords() const override { return {"trash", "recycle bin", "bin", "wastebasket"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Trash).setBackgroundTint(SemanticColor::Yellow);
  }

  void execute(CommandController &ctrl) const override {
    auto ctx = ctrl.context();
    auto toast = ctx->services->toastService();
    std::string error;

    if (SystemTrash::openTrash(error)) {
      ctx->navigation->closeWindow({.clearRootSearch = true});
    } else {
      toast->failure(error.empty() ? tr("Failed to open trash") : QString::fromStdString(error));
    }
  }
};

class SystemRunCommand : public TypedCallbackCommand<SystemRunPreferences> {

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

  void execute(const Controller &ctrl) const override {
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

    switch (ctrl.preferences().defaultAction) {
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

class SystemBrowseApps : public TypedViewCommand<BrowseAppsViewHost, BrowseAppsPreferences> {
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
};

#ifdef Q_OS_LINUX
class SetDefaultTerminal : public BuiltinViewCommand<SetDefaultTerminalViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SetDefaultTerminal)

  QString id() const override { return "set-default-terminal"; }
  QString name() const override { return tr("Set Default Terminal"); }
  QString description() const override { return tr("Change the default system terminal"); }
  std::vector<QString> keywords() const override { return {}; }
  ImageURL iconUrl() const override { return ImageURL::symbol("$").setBackgroundTint(QColor(128, 132, 138)); }
};

class SetDefaultBrowser : public BuiltinViewCommand<SetDefaultBrowserViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SetDefaultBrowser)

  QString id() const override { return "set-default-browser"; }
  QString name() const override { return tr("Set Default Browser"); }
  QString description() const override { return tr("Change the default system web browser"); }
  std::vector<QString> keywords() const override { return {}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Globe01).setBackgroundTint(QColor(128, 132, 138));
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
    registerCommand<EmptyTrashCommand>();
    registerCommand<OpenTrashCommand>();

#ifdef Q_OS_LINUX
    // set default terminal using xdg-terminal-exec
    registerCommand<SetDefaultTerminal>();
    registerCommand<SetDefaultBrowser>();
#endif
  }
};
