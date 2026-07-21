#include "vicinae-extension.hpp"
#include "builtin_icon.hpp"
#include "command-controller.hpp"
#include "services/script-command/script-command-service.hpp"
#include "extensions/vicinae/list-installed-extensions-command.hpp"
#include "qml/oauth-token-store-view-host.hpp"
#include "extensions/vicinae/report-bug-command.hpp"
#include "qml/builtin-icons-view-host.hpp"
#include "services/telemetry/telemetry-service.hpp"
#include "navigation-controller.hpp"
#include "config/config.hpp"
#include "qml/local-storage-view-host.hpp"
#include "open-about-command.hpp"
#include "refresh-apps-command.hpp"
#include "configure-fallback-command.hpp"
#include "extensions/vicinae/search-emoji-command.hpp"
#include "extensions/vicinae/vicinae-store-command.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "builtin-url-command.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#ifdef Q_OS_LINUX
#include <malloc.h>
#endif
#include <QCoreApplication>
#include <qpixmapcache.h>
#include <qurlquery.h>

class OpenDocumentationCommand : public BuiltinUrlCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenDocumentationCommand)

  QString id() const override { return "documentation"; }
  QString name() const override { return tr("Open Online Documentation"); }
  QString description() const override {
    return tr("Navigate to the official vicinae documentation website.");
  }
  std::vector<QString> keywords() const override { return {"Open Online Documentation"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("book").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DOC_URL; }
};

class OpenDiscordCommand : public BuiltinUrlCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenDiscordCommand)

  QString id() const override { return "join-discord-server"; }
  QString name() const override { return tr("Join the Discord Server"); }
  QString description() const override {
    return tr("Open link to join the official Vicinae discord server.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("discord").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"help", "support", "Join the Discord Server"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DISCORD_INVITE_LINK; }
};

class SponsorVicinaeCommand : public BuiltinUrlCommand {
  Q_DECLARE_TR_FUNCTIONS(SponsorVicinaeCommand)

  QString id() const override { return "sponsor"; }
  QString name() const override { return tr("Donate to Vicinae"); }
  QString description() const override { return tr("Open link to Vicinae's GitHub sponsor page"); }
  ImageURL iconUrl() const override { return ImageURL::builtin("heart").setFill(SemanticColor::Magenta); }
  std::vector<QString> keywords() const override { return {"sponsor", "donate", "Donate to Vicinae"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::GH_SPONSOR_LINK; }
};

class OpenVicinaeConfig : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenVicinaeConfig)

  QString id() const override { return "open-config-file"; }
  QString name() const override { return tr("Open Config File"); }
  QString description() const override { return tr("Open the main vicinae configuration file"); }
  std::vector<QString> keywords() const override { return {"Open Config File"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("pencil").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    ctx->services->appDb()->openTarget(QString::fromStdString(ctx->services->config()->path().string()));
    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }
};

class OpenDefaultVicinaeConfig : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenDefaultVicinaeConfig)

  QString id() const override { return "open-default-config"; }
  QString name() const override { return tr("Open Default Config File"); }
  QString description() const override { return tr("Open the default vicinae configuration file"); }
  std::vector<QString> keywords() const override { return {"Open Default Config File"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("pencil").setBackgroundTint(SemanticColor::Accent);
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

class PruneMemoryCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(PruneMemoryCommand)

  QString id() const override { return "prune-memory"; }
  QString name() const override { return tr("Prune Vicinae Memory Usage"); }
  QString description() const override {
    return tr("Try pruning vicinae's memory usage by clearing pixmap cache and calling malloc_trim(). "
              "Mostly provided for internal testing.");
  }
  std::vector<QString> keywords() const override { return {"Prune Vicinae Memory Usage"}; }
  ImageURL iconUrl() const override {
    return ImageURL::emoji("🥊").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    QPixmapCache::clear();
#ifdef Q_OS_LINUX
    malloc_trim(0);
#endif
    controller.context()->services->toastService()->success(tr("Pruned 🥊"));
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
    return ImageURL::builtin("cog").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"preferences", "Open Vicinae Settings"}; }

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
  std::vector<QString> keywords() const override { return {"Reload Script Directories"}; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Code).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();

    ctx->services->scriptDb()->triggerScan();
    ctx->services->toastService()->success(tr("New scan triggered, index will update shortly"));
  }
};

class OpenKeybindSettingsCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(OpenKeybindSettingsCommand)

  QString id() const override { return "keybind-settings"; }
  QString name() const override { return tr("Open Vicinae Keybind Settings"); }
  QString description() const override { return tr("Open the vicinae keybind settings window"); }
  std::vector<QString> keywords() const override { return {"Open Vicinae Keybind Settings"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("keyboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();

    ctx->navigation->closeWindow();
    ctx->settings->openTab("shortcuts");
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
  std::vector<QString> keywords() const override { return {"Forget Past Vicinae Telemetry"}; }

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
  std::vector<QString> keywords() const override { return {"Manage OAuth Token Sets"}; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("key");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

class IconBrowserCommand : public BuiltinViewCommand<BuiltinIconsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(IconBrowserCommand)

  QString id() const override { return "search-builtin-icons"; }
  QString name() const override { return tr("Search Builtin Icons"); }
  QString description() const override { return tr("Search Vicinae builtin set of icons"); }
  std::vector<QString> keywords() const override { return {"Search Builtin Icons"}; }
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
  std::vector<QString> keywords() const override { return {"Inspect Local Storage"}; }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("coin");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

VicinaeExtension::VicinaeExtension() {
  registerCommand<OpenDocumentationCommand>();
  registerCommand<OpenAboutCommand>();
  registerCommand<RefreshAppsCommand>();
  registerCommand<ManageFallbackCommand>();
  registerCommand<SearchEmojiCommand>();
  registerCommand<ReportVicinaeBugCommand>();
  registerCommand<OpenSettingsCommand>();
  registerCommand<SponsorVicinaeCommand>();
  registerCommand<OpenKeybindSettingsCommand>();
  registerCommand<VicinaeStoreCommand>();
  registerCommand<VicinaeListInstalledExtensionsCommand>();
  registerCommand<OAuthTokenStoreCommand>();
  registerCommand<OpenVicinaeConfig>();
  registerCommand<OpenDefaultVicinaeConfig>();
  registerCommand<InspectLocalStorage>();
  registerCommand<ReloadScriptDirectoriesCommand>();
  registerCommand<PruneMemoryCommand>();
  registerCommand<IconBrowserCommand>();
  registerCommand<ForgetTelemetryCommand>();
}
