#include "command-database.hpp"
#include "calculator-history-command.hpp"
#include "emoji-command.hpp"
#include "icon-browser-command.hpp"
#include "manage-processes-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "ui/peepobank-command.hpp"

static std::vector<BuiltinCommand> builtinCommands{
    {.id = "calculator.history",
     .name = "Calculator History",
     .iconUrl = BuiltinOmniIconUrl("plus-minus-divide-multiply").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }},
    {.id = "clipboard.history",
     .name = "Clipboard History",
     .iconUrl = BuiltinOmniIconUrl("copy-clipboard").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }},
    {.id = "quicklink.create",
     .name = "Create Quicklink",
     .iconUrl = BuiltinOmniIconUrl("link").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<QuicklinkCommandView>; }},
    {.id = "quicklink.manage",
     .name = "Manage Quicklinks",
     .iconUrl = BuiltinOmniIconUrl("link").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<ManageQuicklinksView>; }},
    {.id = "emoji.search",
     .name = "Search Emoji & Symbols",
     .iconUrl = BuiltinOmniIconUrl("emoji").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<EmojiView>; }},
    {.id = "icon.search",
     .name = "Search Omnicast Icons",
     .iconUrl = BuiltinOmniIconUrl("omnicast").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<IconBrowserView>; }},
    {.id = "theme.manage",
     .name = "Manage themes",
     .iconUrl = BuiltinOmniIconUrl("brush").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<ManageThemesView>; }},

    {.id = "process.list",
     .name = "List Processes",
     .iconUrl = BuiltinOmniIconUrl("bar-chart").setBackgroundTint(ColorTint::Red),
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<ManageProcessesMainView>; }},
    {
        .id = "peepobank.search",
        .name = "Peepobank",
        .iconUrl = BuiltinOmniIconUrl("emoji").setBackgroundTint(ColorTint::Red),
        .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<PeepobankView>; },
    }};

const std::vector<BuiltinCommand> &CommandDatabase::list() { return builtinCommands; }

const BuiltinCommand *CommandDatabase::findById(const QString &id) {
  for (const auto &cmd : list()) {
    if (cmd.id == id) { return &cmd; }
  }

  return nullptr;
}
