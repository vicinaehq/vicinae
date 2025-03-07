#include "command-database.hpp"
#include "calculator-history-command.hpp"
#include "emoji-command.hpp"
#include "manage-processes-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "ui/peepobank-command.hpp"

static std::vector<BuiltinCommand> builtinCommands{
    {.id = "calculator.history",
     .name = "Calculator history",
     .iconName = ":assets/icons/calculator.png",
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }},
    {.id = "quicklink.create",
     .name = "Create quicklink",
     .iconName = ":assets/icons/quicklink.png",
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<QuicklinkCommandView>; }},
    {.id = "quicklink.manage",
     .name = "Manage quicklinks",
     .iconName = ":assets/icons/quicklink.png",
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<ManageQuicklinksView>; }},
    {.id = "emoji.search",
     .name = "Search Emoji & Symbols",
     .iconName = ":assets/icons/emoji.png",
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<EmojiView>; }},
    {.id = "process.list",
     .name = "List processes",
     .iconName = ":assets/icons/process-manager.png",
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<ManageProcessesMainView>; }},
    {
        .id = "peepobank.search",
        .name = "Peepobank",
        .iconName = ":/icons/link.svg",
        .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<PeepobankView>; },
    }};

const std::vector<BuiltinCommand> &CommandDatabase::list() { return builtinCommands; }

const BuiltinCommand *CommandDatabase::findById(const QString &id) {
  for (const auto &cmd : list()) {
    if (cmd.id == id) { return &cmd; }
  }

  return nullptr;
}
