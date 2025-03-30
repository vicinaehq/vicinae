#include "command-database.hpp"
#include "calculator-history-command.hpp"
#include "clipboard-history-command.hpp"
#include "emoji-command.hpp"
#include "icon-browser-command.hpp"
#include "ask-ai-command.hpp"
#include "switch-windows-command.hpp"
#include "test-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
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

const AbstractCommand *CommandDatabase::findById(const QString &id) {
  for (const auto &repository : list()) {
    for (const auto &cmd : repository->commands()) {
      if (cmd->id() == id) { return cmd.get(); }
    }
  }

  return nullptr;
}

CommandDatabase::CommandDatabase() {

  {
    auto emoji = ViewCommandBuilder<EmojiView>("emoji-symbols")
                     .withName("Search Emojis & Symbols")
                     .withTintedIcon("emoji", ColorTint::Red)
                     .makeShared();
    auto clipboardHistory = ViewCommandBuilder<ClipboardHistoryCommand>("clipboard-history")
                                .withName("Clipboard History")
                                .withTintedIcon("copy-clipboard", ColorTint::Red)
                                .makeShared();
    auto iconSearch = ViewCommandBuilder<IconBrowserView>("")
                          .withName("Search Omnicast Icons")
                          .withTintedIcon("omnicast", ColorTint::Red)
                          .makeShared();

    auto peepobank = ViewCommandBuilder<PeepobankView>("peepobank")
                         .withName("Peepobank")
                         .withTintedIcon("emoji", ColorTint::Red)
                         .makeShared();

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
        ViewCommandBuilder<CalculatorHistoryView>("history").withName("Calculator History").makeShared();
    auto calculator = CommandRepositoryBuilder("calculator")
                          .withName("Calculator")
                          .withTintedIcon("calculator", ColorTint::Red)
                          .withCommand(history)
                          .makeShared();

    registerRepository(calculator);
  }

  {
    auto create =
        ViewCommandBuilder<QuicklinkCommandView>("create").withName("Create Quicklink").makeShared();
    auto manage =
        ViewCommandBuilder<ManageQuicklinksView>("manage").withName("Manage Quicklinks").makeShared();
    auto _export =
        ViewCommandBuilder<ManageQuicklinksView>("export").withName("Export Quicklinks").makeShared();
    auto _import =
        ViewCommandBuilder<ManageQuicklinksView>("import").withName("Import Quicklinks").makeShared();
    auto quicklinks = CommandRepositoryBuilder("quicklinks")
                          .withName("Quicklinks")
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
        ViewCommandBuilder<SwitchWindowsCommand>("switch-windows").withName("Switch Windows").makeShared();
    auto wm = CommandRepositoryBuilder("window-management")
                  .withName("Window Management")
                  .withTintedIcon("app-window-list", ColorTint::Blue)
                  .withCommand(switchWindows)
                  .makeShared();

    registerRepository(wm);
  }

  {
    auto manage = ViewCommandBuilder<ManageThemesView>("manage").withName("Manage Themes").makeShared();
    auto theme = CommandRepositoryBuilder("theme")
                     .withName("Theme")
                     .withTintedIcon("brush", ColorTint::Purple)
                     .withCommand(manage)
                     .makeShared();

    registerRepository(theme);
  }

  { auto peepobank = ViewCommandBuilder<PeepobankView>("peepobank").withName("Peepobank").makeShared(); }

  { auto quickAsk = ViewCommandBuilder<AskAiCommandView>("quick").withName("Quick AI").makeShared(); }
}
