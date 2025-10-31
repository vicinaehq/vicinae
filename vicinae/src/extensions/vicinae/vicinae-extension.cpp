#include "vicinae-extension.hpp"
#include "command-controller.hpp"
#include "common.hpp"
#include "extensions/vicinae/list-installed-extensions-command.hpp"
#include "extensions/vicinae/report-bug-command.hpp"
#include "navigation-controller.hpp"
#include "open-about-command.hpp"
#include "refresh-apps-command.hpp"
#include "browse-icons-command.hpp"
#include "configure-fallback-command.hpp"
#include "extensions/vicinae/search-emoji-command.hpp"
#include "extensions/vicinae/vicinae-store-command.hpp"
#include "../../ui/image/url.hpp"
#include "builtin-url-command.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"
#include "vicinae.hpp"
#include <qsqlquery.h>
#include <qurlquery.h>

class GetVicinaeSourceCodeCommand : public BuiltinUrlCommand {
  QString id() const override { return "get-source-code"; }
  QString name() const override { return "Get Vicinae Source Code"; }
  QString description() const override { return "Navigate to the vicinae GitHub repository."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("code").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::GH_REPO; }
};

class OpenDocumentationCommand : public BuiltinUrlCommand {
  QString id() const override { return "documentation"; }
  QString name() const override { return "Open Vicinae Documentation"; }
  QString description() const override { return "Navigate to the official vicinae documentation website."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("book").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DOC_URL; }
};

class OpenDiscordCommand : public BuiltinUrlCommand {
  QString id() const override { return "join-discord-server"; }
  QString name() const override { return "Join Vicinae Discord Server"; }
  QString description() const override { return "Open link to join the official Vicinae discord server."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("discord").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<QString> keywords() const override { return {"help", "support"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::DISCORD_INVITE_LINK; }
};

class SponsorVicinaeCommand : public BuiltinUrlCommand {
  QString id() const override { return "sponsor"; }
  QString name() const override { return "Become a sponsor"; }
  QString description() const override { return "Open link to Vicinae's GitHub sponsor page"; }
  ImageURL iconUrl() const override { return ImageURL::builtin("heart").setFill(SemanticColor::Magenta); }
  std::vector<QString> keywords() const override { return {"sponsor", "donate"}; }
  QUrl url(const ArgumentValues &values) const override { return Omnicast::GH_SPONSOR_LINK; }
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

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();

    ctx->navigation->closeWindow();
    ctx->settings->openWindow();
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

VicinaeExtension::VicinaeExtension() {
  registerCommand<OpenDocumentationCommand>();
  registerCommand<OpenAboutCommand>();
  registerCommand<RefreshAppsCommand>();
  registerCommand<BrowseIconsCommand>();
  registerCommand<ManageFallbackCommand>();
  registerCommand<SearchEmojiCommand>();
  registerCommand<GetVicinaeSourceCodeCommand>();
  registerCommand<ReportVicinaeBugCommand>();
  registerCommand<OpenSettingsCommand>();
  registerCommand<OpenDiscordCommand>();
  registerCommand<SponsorVicinaeCommand>();
  registerCommand<OpenKeybindSettingsCommand>();
  registerCommand<VicinaeStoreCommand>();
  registerCommand<VicinaeListInstalledExtensionsCommand>();
}
