#include "command-database.hpp"
#include "calculator-history-command.hpp"
#include "clipboard-history-command.hpp"
#include "emoji-command.hpp"
#include "icon-browser-command.hpp"
#include "manage-processes-command.hpp"
#include "switch-windows-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "manage-themes-command.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/peepobank-command.hpp"

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

static std::vector<BuiltinCommand> builtinCommands{
    {.id = "calculator.history",
     .name = "Calculator History",
     .iconUrl = tintedCommandIcon("plus-minus-divide-multiply", ColorTint::Red),
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }},
    {.id = "clipboard.history",
     .name = "Clipboard History",
     .iconUrl = tintedCommandIcon("copy-clipboard", ColorTint::Red),
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<ClipboardHistoryCommand>; }},
    {.id = "quicklink.create",
     .name = "Create Quicklink",
     .iconUrl = tintedCommandIcon("link", ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<QuicklinkCommandView>; }},
    {.id = "quicklink.manage",
     .name = "Manage Quicklinks",
     .iconUrl = tintedCommandIcon("link", ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<ManageQuicklinksView>; }},
    {.id = "quicklink.export",
     .name = "Export Quicklinks",
     .iconUrl = tintedCommandIcon("link", ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<QuicklinkCommandView>; }},
    {.id = "quicklink.import",
     .name = "Import Quicklinks",
     .iconUrl = tintedCommandIcon("link", ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<ManageQuicklinksView>; }},
    {.id = "wm.switch-windows",
     .name = "Switch windows",
     .iconUrl = tintedCommandIcon("app-window-list", ColorTint::Blue),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<SwitchWindowsCommand>; }},

    {.id = "emoji.search",
     .name = "Search Emoji & Symbols",
     .iconUrl = tintedCommandIcon("emoji", ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<EmojiView>; }},
    {.id = "icon.search",
     .name = "Search Omnicast Icons",
     .iconUrl = tintedCommandIcon("omnicast", ColorTint::Red),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<IconBrowserView>; }},
    {.id = "theme.manage",
     .name = "Manage themes",
     .iconUrl = tintedCommandIcon("brush", ColorTint::Purple),
     .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<ManageThemesView>; }},

    {.id = "process.list",
     .name = "List Processes",
     .iconUrl = tintedCommandIcon("bar-chart", ColorTint::Red),
     .factory = [](AppWindow &app,
                   const QString &s) { return new SingleViewCommand<ManageProcessesMainView>; }},
    {
        .id = "peepobank.search",
        .name = "Peepobank",
        .iconUrl = tintedCommandIcon("emoji", ColorTint::Red),
        .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<PeepobankView>; },
    }};

const std::vector<BuiltinCommand> &CommandDatabase::list() { return builtinCommands; }

const BuiltinCommand *CommandDatabase::findById(const QString &id) {
  for (const auto &cmd : list()) {
    if (cmd.id == id) { return &cmd; }
  }

  return nullptr;
}
