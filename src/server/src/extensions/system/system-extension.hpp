#pragma once
#include "command-database.hpp"
#include "common/context.hpp"
#include "qml/browse-apps-view-host.hpp"
#include "single-view-command-context.hpp"
#include "qml/system-run-model.hpp"
#include "qml/system-run-view-host.hpp"
#include "utils.hpp"
#include "services/app-service/app-service.hpp"
#include "services/audio-control/audio-control-service.hpp"
#include "services/toast/toast-service.hpp"
#include <QCoreApplication>
#include <sstream>
#ifndef Q_OS_WIN
#include "xdgpp/desktop-entry/exec.hpp"
#endif

class SystemRunCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(SystemRunCommand)

  QString id() const override { return "run"; }
  QString name() const override { return tr("Run Terminal Program"); }
  QString description() const override { return tr("Run a program in a terminal window"); }
  std::vector<QString> keywords() const override {
    return {"shell command", "run program", "Run Terminal Program"};
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("terminal").setBackgroundTint(SemanticColor::Accent);
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
  std::vector<QString> keywords() const override { return {"Browse Apps"}; }
  bool isDefaultDisabled() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("box").setBackgroundTint(SemanticColor::Accent);
  }
  std::vector<Preference> preferences() const override {
    auto showHidden = Preference::makeCheckbox("showHidden", tr("Show hidden apps"));
    auto sortAlphabetically = Preference::makeCheckbox("sortAlphabetically", tr("Sort alphabetically"));
    showHidden.setDefaultValue(false);
    sortAlphabetically.setDefaultValue(true);
    return {sortAlphabetically, showHidden};
  }
};

namespace {
constexpr auto VOLUME_COMMAND_TINT = SemanticColor::Cyan;

ImageURL volumeIcon(float level) {
  if (level <= 0.0f) return ImageURL{BuiltinIcon::SpeakerOff};
  if (level <= 0.33f) return ImageURL{BuiltinIcon::SpeakerLow};
  if (level <= 0.66f) return ImageURL{BuiltinIcon::SpeakerDown};
  return ImageURL{BuiltinIcon::SpeakerHigh};
}

void showVolumeHud(const ApplicationContext *ctx, float level) {
  auto vol = qRound(level * 100);
  ctx->navigation->showHud(QCoreApplication::translate("system-extension", "Volume %1%").arg(vol),
                           volumeIcon(level));
}
} // namespace

class VolumeUpCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(VolumeUpCommand)

  QString id() const override { return "volume-up"; }
  QString name() const override { return tr("Turn Volume Up"); }
  QString description() const override { return tr("Increase system volume"); }
  std::vector<QString> keywords() const override { return {"audio", "sound", "louder", "Turn Volume Up"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::SpeakerUp}.setBackgroundTint(VOLUME_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override {
    return {{.name = "step", .type = CommandArgument::Text, .placeholder = "+5", .required = false}};
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    auto arg = controller.launchProps().arguments;
    int step = 5;
    if (!arg.empty() && !arg.front().second.isEmpty()) {
      bool ok = false;
      step = arg.front().second.toInt(&ok);
      if (!ok) {
        ctx->services->toastService()->failure(tr("Invalid step value"));
        return;
      }
    }
    auto result = audio->adjustVolume(static_cast<float>(step) / 100.0f);
    if (!result) {
      ctx->services->toastService()->failure(tr("Failed to adjust volume"));
      return;
    }
    showVolumeHud(ctx, *result);
  }
};

class VolumeDownCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(VolumeDownCommand)

  QString id() const override { return "volume-down"; }
  QString name() const override { return tr("Turn Volume Down"); }
  QString description() const override { return tr("Decrease system volume"); }
  std::vector<QString> keywords() const override { return {"audio", "sound", "quieter", "Turn Volume Down"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::SpeakerDown}.setBackgroundTint(VOLUME_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override {
    return {{.name = "step", .type = CommandArgument::Text, .placeholder = "-5", .required = false}};
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    auto arg = controller.launchProps().arguments;
    int step = -5;
    if (!arg.empty() && !arg.front().second.isEmpty()) {
      bool ok = false;
      step = arg.front().second.toInt(&ok);
      if (!ok) {
        ctx->services->toastService()->failure(tr("Invalid step value"));
        return;
      }
    }
    auto result = audio->adjustVolume(static_cast<float>(step) / 100.0f);
    if (!result) {
      ctx->services->toastService()->failure(tr("Failed to adjust volume"));
      return;
    }
    showVolumeHud(ctx, *result);
  }
};

template <int Percent, BuiltinIcon Icon> class SetVolumeCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(SetVolumeCommand)

  static constexpr float LEVEL = static_cast<float>(Percent) / 100.0f;

  QString id() const override { return "volume-" + QString::number(Percent); }
  QString name() const override { return tr("Set Volume to %1%").arg(Percent); }
  QString description() const override { return tr("Set system volume to %1%").arg(Percent); }
  std::vector<QString> keywords() const override {
    return {"audio", "sound", "volume", QString("Set Volume to %1%").arg(Percent)};
  }
  ImageURL iconUrl() const override { return ImageURL{Icon}.setBackgroundTint(VOLUME_COMMAND_TINT); }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    auto result = audio->setVolume(LEVEL);
    if (!result) {
      ctx->services->toastService()->failure(tr("Failed to set volume"));
      return;
    }
    showVolumeHud(ctx, *result);
  }
};

class ToggleMuteCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ToggleMuteCommand)

  QString id() const override { return "toggle-mute"; }
  QString name() const override { return tr("Toggle Mute"); }
  QString description() const override { return tr("Mute or unmute system audio"); }
  std::vector<QString> keywords() const override {
    return {"audio", "sound", "volume", "mute", "unmute", "Toggle Mute"};
  }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::SpeakerOff}.setBackgroundTint(VOLUME_COMMAND_TINT);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    if (!audio->toggleMute()) {
      ctx->services->toastService()->failure(tr("Failed to toggle mute"));
      return;
    }
    if (audio->isMuted()) {
      ctx->navigation->showHud(tr("Muted"), ImageURL{BuiltinIcon::SpeakerOff});
    } else {
      showVolumeHud(ctx, audio->getVolume());
    }
  }
};

class SystemExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(SystemExtension)

  QString id() const override { return "system"; }
  QString displayName() const override { return tr("System"); }
  QString description() const override { return tr("System-related commands"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("cog").setBackgroundTint(SemanticColor::Accent);
  }

public:
  SystemExtension() {
    registerCommand<SystemRunCommand>();
    registerCommand<SystemBrowseApps>();
    registerCommand<VolumeUpCommand>();
    registerCommand<VolumeDownCommand>();
    registerCommand<SetVolumeCommand<100, BuiltinIcon::SpeakerHigh>>();
    registerCommand<SetVolumeCommand<75, BuiltinIcon::SpeakerHigh>>();
    registerCommand<SetVolumeCommand<50, BuiltinIcon::SpeakerLow>>();
    registerCommand<SetVolumeCommand<25, BuiltinIcon::SpeakerLow>>();
    registerCommand<SetVolumeCommand<0, BuiltinIcon::SpeakerOff>>();
    registerCommand<ToggleMuteCommand>();
  }

  std::vector<Preference> preferences() const override { return {}; }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {}
};
