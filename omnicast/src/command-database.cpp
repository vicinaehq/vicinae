#include "command-database.hpp"
#include "calculator-history-command.hpp"
#include "clipboard-history-command.hpp"
#include "create-quicklink-command.hpp"
#include "configure-ai-providers-command.hpp"
#include "emoji-command.hpp"
#include "icon-browser-command.hpp"
#include "ask-ai-command.hpp"
#include "preference.hpp"
#include "switch-windows-command.hpp"
#include "test-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "command-builder.hpp"
#include "ui/peepobank-command.hpp"
#include <memory>
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

    auto storeAllOfferingsPreference = std::make_shared<CheckboxPreference>();

    storeAllOfferingsPreference->setName("store-all-offerings");
    storeAllOfferingsPreference->setTitle("Store all offerings");
    storeAllOfferingsPreference->setDescription("Store and index alternative mime type offerings. This will "
                                                "increase total storage size, but will refine the search.");
    storeAllOfferingsPreference->setLabel("Store all offerings");

    auto maximumClipboardStorageSizePreference = std::make_shared<TextFieldPreference>();

    maximumClipboardStorageSizePreference->setName("maximum-storage-size");
    maximumClipboardStorageSizePreference->setTitle("Maximum storage size");
    maximumClipboardStorageSizePreference->setDescription(
        "How much storage can be used to store clipboard history data, in MB.");
    maximumClipboardStorageSizePreference->setDefaultValue("1000");

    auto clipboardHistory = CommandBuilder("clipboard-history")
                                .withName("Clipboard History")
                                .withTintedIcon("copy-clipboard", ColorTint::Red)
                                .withPreference(storeAllOfferingsPreference)
                                .withPreference(maximumClipboardStorageSizePreference)
                                .toSingleView<ClipboardHistoryCommand>();

    auto iconSearch = CommandBuilder("browse-icons")
                          .withName("Search Omnicast Icons")
                          .withTintedIcon("omnicast", ColorTint::Red)
                          .toSingleView<IconBrowserView>();

    auto peepobank = CommandBuilder("peepobank")
                         .withName("Peepobank")
                         .withTintedIcon("emoji", ColorTint::Red)
                         .toSingleView<PeepobankView>();

    auto omnicast = CommandRepositoryBuilder("omnicast")
                        .withName("Omnicast")
                        .withCommand(clipboardHistory)
                        .withCommand(emoji)
                        .withCommand(iconSearch)
                        .withCommand(peepobank)
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
    auto testExtensionPref = std::make_shared<TextFieldPreference>();

    testExtensionPref->setName("test-extension-pref");
    testExtensionPref->setTitle("Test Extension Pref");
    testExtensionPref->setDefaultValue("ting ting");
    testExtensionPref->setDescription("A simple test, nothing more.");

    auto create = CommandBuilder("create").withName("Create Quicklink").toSingleView<QuicklinkCommandView>();
    auto manage = CommandBuilder("manage").withName("Manage Quicklinks").toSingleView<ManageQuicklinksView>();
    auto _export =
        CommandBuilder("export").withName("Export Quicklinks").toSingleView<ManageQuicklinksView>();
    auto _import =
        CommandBuilder("import").withName("Import Quicklinks").toSingleView<ManageQuicklinksView>();
    auto quicklinks = CommandRepositoryBuilder("quicklinks")
                          .withName("Quicklinks")
                          .withPreference(testExtensionPref)
                          .withTintedIcon("link", ColorTint::Red)
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
    auto manage = CommandBuilder("manage").withName("Manage Themes").toSingleView<ManageThemesView>();
    auto theme = CommandRepositoryBuilder("theme")
                     .withName("Theme")
                     .withTintedIcon("brush", ColorTint::Purple)
                     .withCommand(manage)
                     .makeShared();

    registerRepository(theme);
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
    auto quickAsk = CommandBuilder("quick").withName("Quick AI").toContext<AskAiCommand>();
    auto configureProviders = CommandBuilder("configure-providers")
                                  .withName("Configure AI providers")
                                  .toSingleView<ConfigureAIProvidersView>();

    auto ai = CommandRepositoryBuilder("ai")
                  .withName("AI")
                  .withTintedIcon("stars", ColorTint::Red)
                  .withCommand(quickAsk)
                  .withCommand(configureProviders)
                  .makeShared();

    registerRepository(ai);
  }
}
