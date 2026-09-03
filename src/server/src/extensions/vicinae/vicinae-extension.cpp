#include "vicinae-extension.hpp"
#include "builtin_icon.hpp"
#include "command-controller.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/script-command/script-command-service.hpp"
#include "extensions/vicinae/list-installed-extensions-command.hpp"
#include "qml/oauth-token-store-view-host.hpp"
#include "extensions/vicinae/report-bug-command.hpp"
#include "qml/builtin-icons-view-host.hpp"
#include "services/telemetry/telemetry-service.hpp"
#include "navigation-controller.hpp"
#include "config/config.hpp"
#include "qml/local-storage-view-host.hpp"
#include "refresh-apps-command.hpp"
#include "configure-fallback-command.hpp"
#include "extensions/vicinae/search-emoji-command.hpp"
#include "extensions/vicinae/search-tray-command.hpp"
#ifdef Q_OS_MACOS
#include "extensions/vicinae/search-menu-bar-command.hpp"
#endif
#include "extensions/vicinae/vicinae-store-command.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "builtin-url-command.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include <QCoreApplication>
#include <qurlquery.h>

class OpenDiscordCommand : public BuiltinUrlCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenDiscordCommand)

  QString id() const override { return "join-discord-server"; }
  QString name() const override { return tr("Join the Discord Server"); }
  QString description() const override {
    return tr("Open link to join the official Vicinae discord server.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Discord).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"help", "support"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DISCORD_INVITE_LINK; }
};

class SponsorVicinaeCommand : public BuiltinUrlCommand {
  Q_DECLARE_TR_FUNCTIONS(SponsorVicinaeCommand)

  QString id() const override { return "sponsor"; }
  QString name() const override { return tr("Donate to Vicinae"); }
  QString description() const override { return tr("Open link to Vicinae's GitHub sponsor page"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Heart).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"sponsor", "donate"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::GH_SPONSOR_LINK; }
};

class OpenVicinaeConfig : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenVicinaeConfig)

  QString id() const override { return "open-config-file"; }
  QString name() const override { return tr("Open Config File"); }
  QString description() const override { return tr("Open the main vicinae configuration file"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Pencil).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    ctx->services->appDb()->openTarget(QString::fromStdString(ctx->services->config()->path().string()));
    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }
};

class ShowLogs : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenVicinaeConfig)

  QString id() const override { return "show-logs"; }
  QString name() const override { return tr("Show Log File"); }
  QString description() const override { return tr("Open the Vicinae log file in your file browser"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Paragraph).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    ctx->services->appDb()->showInFileBrowser(Omnicast::stateDir() / "vicinae.log", true);
    ctx->navigation->closeWindow();
  }
};

class OpenDefaultVicinaeConfig : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenDefaultVicinaeConfig)

  QString id() const override { return "open-default-config"; }
  QString name() const override { return tr("Open Default Config File"); }
  QString description() const override { return tr("Open the default vicinae configuration file"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Pencil).setBackgroundTint(SemanticColor::Accent);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto toast = ctx->services->toastService();
    auto path = Omnicast::runtimeDir() / "default-config.jsonc";

    QFile::remove(path);

    QFile file(path);
    auto configFile = QFile(":config.jsonc");

    if (!file.open(QIODevice::WriteOnly)) {
      toast->failure(tr("Failed to open temporary file"));
      return;
    }

    if (!configFile.open(QIODevice::ReadOnly)) {
      toast->failure(tr("Failed to open default config file"));
      return;
    }

    file.write(configFile.readAll());
    file.flush();
    file.setPermissions(QFileDevice::ReadOwner);
    ctx->services->appDb()->openTarget(file.fileName());
    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }
};

class OpenSettingsCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenSettingsCommand)

  QString id() const override { return "settings"; }
  QString name() const override { return tr("Open Vicinae Settings"); }
  QString description() const override {
    return tr("Open the vicinae settings window, which is an independent floating window.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Cog).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"preferences"}; }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();

    ctx->navigation->closeWindow();
    ctx->settings->openWindow();
  }
};

class ReloadScriptDirectoriesCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ReloadScriptDirectoriesCommand)

  QString id() const override { return "reload-scripts"; }
  QString name() const override { return tr("Reload Script Directories"); }
  QString description() const override { return tr("Reload script directories"); }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Code).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();

    ctx->services->scriptDb()->triggerScan();
    ctx->services->toastService()->success(tr("New scan triggered, index will update shortly"));
  }
};

class ForgetTelemetryCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ForgetTelemetryCommand)

  QString id() const override { return "forget-telemetry"; }
  QString name() const override { return tr("Forget Past Vicinae Telemetry"); }
  QString description() const override {
    return tr("Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance "
              "ID attached. The ID is only linked to your vicinae install, which has no direct relationship "
              "with your system.");
  }

  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::XMarkCircle).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  bool isDefaultDisabled() const override { return true; }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto toast = ctx->services->toastService();
    auto telemetry = ctx->services->telemetry();
    auto config = ctx->services->config();

    ctx->navigation->showWindow();
    ctx->navigation->setSearchText(">"); // force to exit out of compact mode if it is enabled
    toast->dynamic(tr("Processing..."));
    telemetry->forget().then([toast, config](bool ok) {
      if (ok) {
        config->mergeWithUser({.telemetry = config::Partial<config::TelemetryConfig>{.systemInfo = false}});
        toast->success(tr("Past telemetry was successfully detached from your vicinae user ID."));
      } else {
        toast->failure(tr("Failed to forget past telemetry data"));
      }
    });
  }
};

class OAuthTokenStoreCommand : public BuiltinViewCommand<OAuthTokenStoreViewHost> {
  Q_DECLARE_TR_FUNCTIONS(OAuthTokenStoreCommand)

  QString id() const override { return "oauth-token-store"; }
  QString name() const override { return tr("Manage OAuth Token Sets"); }
  QString description() const override {
    return tr("Manage OAuth token sets that have been saved by extensions providing OAuth integrations.");
  }
  bool isDefaultDisabled() const override { return true; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin(BuiltinIcon::Key);
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

class IconBrowserCommand : public BuiltinViewCommand<BuiltinIconsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(IconBrowserCommand)

  QString id() const override { return "search-builtin-icons"; }
  QString name() const override { return tr("Search Builtin Icons"); }
  QString description() const override { return tr("Search Vicinae builtin set of icons"); }
  bool isDefaultDisabled() const override { return true; }
  ImageURL iconUrl() const override {
    ImageURL icon{BuiltinIcon::Box};
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

class InspectLocalStorage : public BuiltinViewCommand<LocalStorageViewHost> {
  Q_DECLARE_TR_FUNCTIONS(InspectLocalStorage)

  QString id() const override { return "inspect-local-storage"; }
  QString name() const override { return tr("Inspect Local Storage"); }
  bool isDefaultDisabled() const override { return true; }
  QString description() const override {
    return tr("Browse data stored in Vicinae's local storage. This includes data stored for builtin "
              "extensions as well as third-party extensions making use of the LocalStorage API.");
  }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin(BuiltinIcon::Coin);
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

VicinaeExtension::VicinaeExtension() {
  registerCommand<RefreshAppsCommand>();
  registerCommand<ManageFallbackCommand>();
  registerCommand<SearchEmojiCommand>();
#ifdef Q_OS_LINUX
  registerCommand<SearchTrayCommand>();
#endif
#ifdef Q_OS_MACOS
  registerCommand<SearchMenuBarCommand>();
#endif
  registerCommand<ReportVicinaeBugCommand>();
  registerCommand<OpenSettingsCommand>();
  registerCommand<SponsorVicinaeCommand>();
  registerCommand<VicinaeStoreCommand>();
  registerCommand<VicinaeListInstalledExtensionsCommand>();
  registerCommand<OpenVicinaeConfig>();
  registerCommand<OpenDefaultVicinaeConfig>();
  registerCommand<ReloadScriptDirectoriesCommand>();
  registerCommand<ShowLogs>();

  // disabled by default
  registerCommand<ForgetTelemetryCommand>();
  registerCommand<IconBrowserCommand>();
  registerCommand<OAuthTokenStoreCommand>();
  registerCommand<InspectLocalStorage>();
}
