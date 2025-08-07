#include "command-database.hpp"
#include "open-about-command.hpp"
#include "open-documentation-command.hpp"
#include "refresh-apps-command.hpp"
#include "browse-icons-command.hpp"
#include "configure-fallback-command.hpp"
#include "extensions/vicinae/search-emoji-command.hpp"
#include "../../ui/image/url.hpp"

class VicinaeExtension : public BuiltinCommandRepository {
  QString id() const override { return "vicinae"; }
  QString displayName() const override { return "Vicinae"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("vicinae").setBackgroundTint(SemanticColor::Red);
  }

public:
  VicinaeExtension() {
    registerCommand<OpenDocumentationCommand>();
    registerCommand<OpenAboutCommand>();
    registerCommand<RefreshAppsCommand>();
    registerCommand<BrowseIconsCommand>();
    registerCommand<ManageFallbackCommand>();
    registerCommand<SearchEmojiCommand>();
  }
};
