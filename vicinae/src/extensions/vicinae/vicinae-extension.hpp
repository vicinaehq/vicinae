#include "command-database.hpp"
#include "open-about-command.hpp"
#include "refresh-apps-command.hpp"
#include "browse-icons-command.hpp"
#include "configure-fallback-command.hpp"
#include "extensions/vicinae/search-emoji-command.hpp"
#include "../../ui/image/url.hpp"
#include "builtin-url-command.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"

class GetVicinaeSourceCodeCommand : public BuiltinUrlCommand {
  QString id() const override { return "get-source-code"; }
  QString name() const override { return "Get Vicinae Source Code"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("code").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url() const override { return Omnicast::GH_REPO; }
};

class ReportVicinaeBugCommand : public BuiltinUrlCommand {
  QString id() const override { return "report-bug"; }
  QString name() const override { return "Report a Vicinae Bug"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("bug").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url() const override { return Omnicast::GH_REPO_CREATE_ISSUE; }
};

class OpenDocumentationCommand : public BuiltinUrlCommand {
  QString id() const override { return "documentation"; }
  QString name() const override { return "Open Vicinae Documentation"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("book").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QUrl url() const override { return Omnicast::DOC_URL; }
};

class OpenSettingsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "settings"; }
  QString name() const override { return "Open Vicinae Settings"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("cog").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(ApplicationContext *ctx) const override {
    ctx->navigation->closeWindow();
    ctx->settings->openWindow();
  }
};

class VicinaeExtension : public BuiltinCommandRepository {
  QString id() const override { return "vicinae"; }
  QString displayName() const override { return "Vicinae"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("vicinae").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  VicinaeExtension() {
    registerCommand<OpenDocumentationCommand>();
    registerCommand<OpenAboutCommand>();
    registerCommand<RefreshAppsCommand>();
    registerCommand<BrowseIconsCommand>();
    registerCommand<ManageFallbackCommand>();
    registerCommand<SearchEmojiCommand>();
    registerCommand<GetVicinaeSourceCodeCommand>();
    registerCommand<ReportVicinaeBugCommand>();
    registerCommand<OpenSettingsCommand>();
  }
};
