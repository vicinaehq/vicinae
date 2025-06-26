#include "command-database.hpp"
#include "calculator-history-command.hpp"
#include "create-quicklink-command.hpp"
#include "configure-ai-providers-command.hpp"
#include "emoji-command.hpp"
#include "icon-browser-command.hpp"
#include "manage-fallback-commands.hpp"
#include "omnicast/browse-fonts-view.hpp"
#include "preference.hpp"
#include "switch-windows-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "command-builder.hpp"
#include <memory>
#include <qdnslookup.h>
#include <qfuture.h>
#include <qlocale.h>

QColor getWashedUpWhite() {
  QColor color("#FFFFFF");

  color.setAlphaF(0.6);

  return color;
}

OmniIconUrl tintedCommandIcon(const QString &iconName, ColorTint tint) {
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
                     .withTintedIcon("emoji", ColorTint::Red)
                     .toSingleView<EmojiView>();

    auto history = std::make_shared<ClipboardHistoryCommand>();

    auto clipboard = CommandRepositoryBuilder("clipboard")
                         .withName("Clipboard")
                         .withTintedIcon("copy-clipboard", ColorTint::Red)
                         .withCommand(history)
                         .makeShared();

    registerRepository(clipboard);

    auto iconSearch = CommandBuilder("browse-icons")
                          .withName("Search Omnicast Icons")
                          .withTintedIcon("omnicast", ColorTint::Red)
                          .toSingleView<IconBrowserView>();

    auto configureFallbackCommands = CommandBuilder("configure-fallback-commands")
                                         .withName("Configure Fallback Commands")
                                         .withTintedIcon("arrow-counter-clockwise", ColorTint::Red)
                                         .toSingleView<ManageFallbackCommands>();

    auto omnicast = CommandRepositoryBuilder("omnicast")
                        .withTintedIcon("omnicast", ColorTint::Red)
                        .withName("Omnicast")
                        .withCommand(emoji)
                        .withCommand(iconSearch)
                        .withCommand(configureFallbackCommands)
                        .makeShared();

    registerRepository(omnicast);
  }

  {
    auto history =
        CommandBuilder("history").withName("Calculator History").toSingleView<CalculatorHistoryView>();
    auto calculator = CommandRepositoryBuilder("calculator")
                          .withName("Calculator")
                          .withTintedIcon("calculator", ColorTint::Red)
                          .withCommand(history)
                          .makeShared();

    registerRepository(calculator);
  }

  {
    auto textExtensionPref = Preference::makeText();

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
                          .withTintedIcon("bookmark", ColorTint::Red)
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
                  .withTintedIcon("app-window-list", ColorTint::Blue)
                  .withCommand(switchWindows)
                  .makeShared();

    registerRepository(wm);
  }

  {
    auto manageThemes = CommandBuilder("manage").withName("Manage Themes").toSingleView<ManageThemesView>();
    auto setAppearance =
        CommandBuilder("appearance").withName("Set omnicast appearance").toSingleView<ManageThemesView>();
    auto theme = CommandRepositoryBuilder("theme")
                     .withName("Theme")
                     .withTintedIcon("brush", ColorTint::Purple)
                     .withCommand(manageThemes)
                     .withCommand(setAppearance)
                     .makeShared();

    registerRepository(theme);
  }

  {
    auto browseFonts = CommandBuilder("browser").withName("Browse Fonts").toSingleView<BrowseFontsView>();
    auto fonts = CommandRepositoryBuilder("fonts")
                     .withName("Font")
                     .withTintedIcon("text", ColorTint::Orange)
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
                         .withTintedIcon("hammer", ColorTint::Magenta)
                         .withCommand(diagnostics)
                         .withCommand(create)
                         .withCommand(_import)
                         .withCommand(manage)
                         .makeShared();

    registerRepository(developer);
  }

  {
    // auto quickAsk = CommandBuilder("quick").withName("Quick AI").toContext<AskAiCommand>();
    auto configureProviders = CommandBuilder("configure-providers")
                                  .withName("Configure AI providers")
                                  .toSingleView<ConfigureAIProvidersView>();

    auto ai = CommandRepositoryBuilder("ai")
                  .withName("AI")
                  .withTintedIcon("stars", ColorTint::Red)
                  .withCommand(configureProviders)
                  .makeShared();

    registerRepository(ai);
  }
}
