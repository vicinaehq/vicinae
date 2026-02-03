#include "vicinae-extension.hpp"
#include "builtin_icon.hpp"
#include "command-controller.hpp"
#include "services/script-command/script-command-service.hpp"
#include "extensions/vicinae/list-installed-extensions-command.hpp"
#include "extensions/vicinae/oauth-token-store/oauth-token-store-view.hpp"
#include "extensions/vicinae/report-bug-command.hpp"
#include "extensions/vicinae/browse-icons/search-builtin-icons-view.hpp"

#include "navigation-controller.hpp"
#include "local-storage/browse-local-storage.hpp"
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
#include <malloc.h>
#include <qpixmapcache.h>
#include <qsqlquery.h>
#include <qurlquery.h>

class OpenDocumentationCommand : public BuiltinUrlCommand {
  QString id() const override { return "documentation"; }
  QString name() const override { return "Open Online Documentation"; }
  QString description() const override { return "Navigate to the official vicinae documentation website."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("book").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DOC_URL; }
};

class OpenDiscordCommand : public BuiltinUrlCommand {
  QString id() const override { return "join-discord-server"; }
  QString name() const override { return "Join the Discord Server"; }
  QString description() const override { return "Open link to join the official Vicinae discord server."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("discord").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"help", "support"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DISCORD_INVITE_LINK; }
};

class SponsorVicinaeCommand : public BuiltinUrlCommand {
  QString id() const override { return "sponsor"; }
  QString name() const override { return "Donate to Vicinae"; }
  QString description() const override { return "Open link to Vicinae's GitHub sponsor page"; }
  ImageURL iconUrl() const override { return ImageURL::builtin("heart").setFill(SemanticColor::Magenta); }
  std::vector<QString> keywords() const override { return {"sponsor", "donate"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::GH_SPONSOR_LINK; }
};

class OpenVicinaeConfig : public BuiltinCallbackCommand {
  QString id() const override { return "open-config-file"; }
  QString name() const override { return "Open Config File"; }
  QString description() const override { return "Open the main vicinae configuration file"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("pencil").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();
    ctx->services->appDb()->openTarget(ctx->services->config()->path().c_str());
    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }
};

class OpenDefaultVicinaeConfig : public BuiltinCallbackCommand {
  QString id() const override { return "open-default-config"; }
  QString name() const override { return "Open Default Config File"; }
  QString description() const override { return "Open the default vicinae configuration file"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("pencil").setBackgroundTint(SemanticColor::Orange);
  }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();
    auto toast = ctx->services->toastService();
    auto path = Omnicast::runtimeDir() / "default-config.jsonc";

    QFile::remove(path);

    QFile file(path);
    auto configFile = QFile(":config.jsonc");

    if (!file.open(QIODevice::WriteOnly)) {
      toast->failure("Failed to open temporary file");
      return;
    }

    if (!configFile.open(QIODevice::ReadOnly)) {
      toast->failure("Failed to open default config file");
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
  QString id() const override { return "prune-memory"; }
  QString name() const override { return "Prune Vicinae Memory Usage"; }
  QString description() const override {
    return "Try pruning vicinae's memory usage by clearing pixmap cache and calling malloc_trim(). Mostly "
           "provided for internal testing.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::emoji("🥊").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *controller) const override {
    QPixmapCache::clear();
    malloc_trim(0);
    controller->context()->services->toastService()->success("Pruned 🥊");
  }
};

class OpenSettingsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "settings"; }
  QString name() const override { return "Open Vicinae Settings"; }
  QString description() const override {
    return "Open the vicinae settings window, which is an independent floating window.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("cog").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  bool isDefaultDisabled() const override { return true; }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();

    ctx->navigation->closeWindow();
    ctx->settings->openWindow();
  }
};

class ReloadScriptDirectoriesCommand : public BuiltinCallbackCommand {
  QString id() const override { return "reload-scripts"; }
  QString name() const override { return "Reload Script Directories"; }
  QString description() const override { return "Reload script directories"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Code).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();

    ctx->services->scriptDb()->triggerScan();
    ctx->services->toastService()->success("New scan triggered, index will update shortly");
  }
};

class OpenKeybindSettingsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "keybind-settings"; }
  QString name() const override { return "Open Vicinae Keybind Settings"; }
  QString description() const override { return "Open the vicinae keybind settings window"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("keyboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();

    ctx->navigation->closeWindow();
    ctx->settings->openTab("keybinds");
  }
};

class OAuthTokenStoreCommand : public BuiltinViewCommand<OAuthTokenStoreView> {
  QString id() const override { return "oauth-token-store"; }
  QString name() const override { return "Manage OAuth Token Sets"; }
  QString description() const override {
    return "Manage OAuth token sets that have been saved by extensions providing OAuth integrations.";
  }
  ImageURL iconUrl() const override {
    auto icon = ImageURL::builtin("key");
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

class IconBrowserCommand : public BuiltinViewCommand<SearchBuiltinIconView> {
  QString id() const override { return "search-builtin-icons"; }
  QString name() const override { return "Search Builtin Icons"; }
  QString description() const override { return "Search Vicinae builtin set of icons"; }
  ImageURL iconUrl() const override {
    ImageURL icon{BuiltinIcon::Box};
    icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
    return icon;
  }
};

class InspectLocalStorage : public BuiltinViewCommand<BrowseLocalStorageView> {
  QString id() const override { return "inspect-local-storage"; }
  QString name() const override { return "Inspect Local Storage"; }
  bool isDefaultDisabled() const override { return true; }
  QString description() const override {
    return "Browse data stored in Vicinae's local storage. This includes data stored for builtin extensions "
           "as well as third-party extensions making use of the LocalStorage API.";
  }
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
}
