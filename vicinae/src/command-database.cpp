#include "command-database.hpp"
#include "create-quicklink-command.hpp"
#include "extensions/clipboard/clipboard-extension.hpp"
#include "extensions/calculator/calculator-extension.hpp"
#include "emoji-command.hpp"
#include "extensions/file/file-extension.hpp"
#include "extensions/vicinae/open-documentation-command.hpp"
#include "extensions/vicinae/refresh-apps-command.hpp"
#include "extensions/raycast/raycast-compat-extension.hpp"
#include "icon-browser-command.hpp"
#include "manage-fallback-commands.hpp"
#include "vicinae/browse-fonts-view.hpp"
#include "preference.hpp"
#include "switch-windows-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "command-builder.hpp"
#include <memory>
#include <qdnslookup.h>
#include <qfuture.h>
#include <qlocale.h>

OmniIconUrl tintedCommandIcon(const QString &iconName, SemanticColor tint) {
  BuiltinOmniIconUrl url(iconName);
  QColor color("#FFFFFF");

  color.setAlphaF(0.8);

  return url.setFill(color).setBackgroundTint(tint);
}

const AbstractCmd *CommandDatabase::findCommand(const QString &id) {
  if (auto repo = findRepository(id)) {
    for (const auto &cmd : repo->commands()) {
      if (cmd->uniqueId() == id) { return cmd.get(); }
    }
  }

  return nullptr;
}

const std::vector<std::shared_ptr<AbstractCommandRepository>> &CommandDatabase::repositories() const {
  return _repositories;
}

void CommandDatabase::registerRepository(const std::shared_ptr<AbstractCommandRepository> &repo) {
  _repositories.push_back(repo);
}

const AbstractCommandRepository *CommandDatabase::findRepository(const QString &id) {
  for (const auto &repository : repositories()) {
    if (repository->id() == id) return repository.get();
  }

  return nullptr;
}

CommandDatabase::CommandDatabase() {

  {
    auto emoji = CommandBuilder("emoji-symbols")
                     .withName("Search Emojis & Symbols")
                     .withTintedIcon("emoji", SemanticColor::Red)
                     .toSingleView<EmojiView>();

    auto clipboard = std::make_shared<ClipboardExtension>();

    registerRepository(clipboard);

    registerRepository(std::make_shared<FileExtension>());
    registerRepository(std::make_shared<RaycastCompatExtension>());

    auto iconSearch =
        CommandBuilder("browse-icons")
            .withName("Search Vicinae Icons")
            .withDescription(
                R"("Browse the list of icons that are built into Omnicast. Useful when building extensions.")")
            .withTintedIcon("vicinae", SemanticColor::Red)
            .toSingleView<IconBrowserView>();

    auto configureFallbackCommands =
        CommandBuilder("configure-fallback-commands")
            .withName("Configure Fallback Commands")
            .withDescription(
                R"("Configure what commands are to be presented as fallback options when nothing matches the search in the root search.)")
            .withTintedIcon("magnifying-glass", SemanticColor::Red)
            .toSingleView<ManageFallbackCommands>();

    auto refreshApps =
        CommandBuilder("refresh-apps")
            .withName("Refresh Applications")
            .withDescription(
                R"(Refresh applications installed on the system and update the root search index accordingly. Running this command manually is usually not needed but can help work around some quirks.)")
            .withTintedIcon("redo", SemanticColor::Red)
            .toContext<RefreshAppsCommandContext>();

    auto openDocumentation = CommandBuilder("open-documentation")
                                 .withName("Open Documentation")
                                 .withDescription(R"(Open the Omnicast documentation in the default browser)")
                                 .withTintedIcon("book", SemanticColor::Red)
                                 .toContext<OpenDocumentationCommand>();

    auto omnicast = CommandRepositoryBuilder("omnicast")
                        .withTintedIcon("vicinae", SemanticColor::Red)
                        .withName("Vicinae")
                        .withCommand(emoji)
                        .withCommand(iconSearch)
                        .withCommand(configureFallbackCommands)
                        .withCommand(refreshApps)
                        .withCommand(openDocumentation)
                        .makeShared();

    registerRepository(omnicast);
  }

  {
    auto calculator = std::make_shared<CalculatorExtension>();

    registerRepository(calculator);
  }

  {
    auto textExtensionPref = Preference::makeText("test-extension-pref");

    textExtensionPref.setName("test-extension-pref");
    textExtensionPref.setTitle("Test Extension Pref");
    textExtensionPref.setDefaultValue("ting ting");
    textExtensionPref.setDescription("A simple test, nothing more.");

    auto create = CommandBuilder("create").withName("Create Bookmark").toSingleView<BookmarkFormView>();
    auto manage = CommandBuilder("manage").withName("Manage Bookmarks").toSingleView<ManageBookmarksView>();
    auto _export = CommandBuilder("export").withName("Export Bookmarks").toSingleView<ManageBookmarksView>();
    auto _import = CommandBuilder("import").withName("Import Bookmarks").toSingleView<ManageBookmarksView>();
    auto quicklinks = CommandRepositoryBuilder("bookmarks")
                          .withName("Bookmarks")
                          .withPreference(textExtensionPref)
                          .withTintedIcon("bookmark", SemanticColor::Red)
                          .withCommand(create)
                          .withCommand(manage)
                          .withCommand(_export)
                          .withCommand(_import)
                          .makeShared();

    registerRepository(quicklinks);
  }

  {
    auto switchWindows =
        CommandBuilder("switch-windows").withName("Switch Windows").toSingleView<SwitchWindowsCommand>();
    auto wm = CommandRepositoryBuilder("window-management")
                  .withName("Window Management")
                  .withTintedIcon("app-window-list", SemanticColor::Blue)
                  .withCommand(switchWindows)
                  .makeShared();

    registerRepository(wm);
  }

  {
    auto manageThemes =
        CommandBuilder("manage")
            .withName("Manage Themes")
            .withDescription(
                "Change the theme of the entire application. <a href=\"#\">Learn more about theming</a>")
            .toSingleView<ManageThemesView>();
    auto theme = CommandRepositoryBuilder("theme")
                     .withName("Theme")
                     .withTintedIcon("brush", SemanticColor::Purple)
                     .withCommand(manageThemes)
                     .makeShared();

    registerRepository(theme);
  }

  {
    auto browseFonts = CommandBuilder("browser")
                           .withName("Browse Fonts")
                           .withDescription(R"("Browse system fonts and set the Omnicast default font.")")
                           .toSingleView<BrowseFontsView>();
    auto fonts = CommandRepositoryBuilder("fonts")
                     .withName("Font")
                     .withTintedIcon("text", SemanticColor::Orange)
                     .withCommand(browseFonts)
                     .makeShared();

    registerRepository(fonts);
  }

  {
    auto diagnostics = CommandBuilder("extension-diagnostics")
                           .withName("Extension Diagnostics")
                           .toSingleView<ManageThemesView>();
    auto create =
        CommandBuilder("create-extension").withName("Create Extension").toSingleView<ManageThemesView>();
    auto _import =
        CommandBuilder("import-extension").withName("Import Extension").toSingleView<ManageThemesView>();
    auto manage =
        CommandBuilder("manage-extensions").withName("Manage Extensions").toSingleView<ManageThemesView>();
    auto developer = CommandRepositoryBuilder("developer")
                         .withName("Developer")
                         .withTintedIcon("hammer", SemanticColor::Magenta)
                         .withCommand(diagnostics)
                         .withCommand(create)
                         .withCommand(_import)
                         .withCommand(manage)
                         .makeShared();

    registerRepository(developer);
  }

  {
    /*
auto quickAsk = CommandBuilder("quick").withName("Quick AI").toContext<AskAiCommand>();
auto configureProviders = CommandBuilder("configure-providers")
                            .withName("Configure AI providers")
                            .toSingleView<ConfigureAIProvidersView>();

auto ai = CommandRepositoryBuilder("ai")
            .withName("AI")
            .withTintedIcon("stars", ColorTint::Red)
            .withCommand(configureProviders)
            .makeShared();

registerRepository(ai);
  */
  }
}
