#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "command.hpp"
#include "create-quicklink-command.hpp"
#include "emoji-command.hpp"
#include "extension_manager.hpp"
#include "icon-browser-command.hpp"
#include "manage-processes-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "omni-icon.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "tinyexpr.hpp"
#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/color-transform-widget.hpp"
#include "ui/color_circle.hpp"
#include "ui/default-list-item-widget.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "ui/peepobank-command.hpp"
#include "quicklink-actions.hpp"
#include "ui/transform-result.hpp"
#include <QtConcurrent/QtConcurrent>
#include <cmath>
#include <functional>
#include <memory>
#include <qbrush.h>
#include <qcoreevent.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qmap.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qnetworkcookiejar.h>
#include <qthreadpool.h>
#include <qwidget.h>
#include "command-database.hpp"

struct OpenBuiltinCommandAction : public AbstractAction {
  BuiltinCommand cmd;
  QString text;

  void execute(AppWindow &app) override {
    emit app.launchCommand(
        cmd.factory(app, text),
        {.searchQuery = text, .navigation = NavigationStatus{.title = cmd.name, .iconUrl = cmd.iconUrl}});
  }

  OpenBuiltinCommandAction(const BuiltinCommand &cmd, const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, cmd.iconUrl), cmd(cmd), text(text) {}
};

using CommandFactory = std::function<ViewCommand *(AppWindow &app, const QString &arg)>;

struct OpenCommandAction : public AbstractAction {
  CommandFactory factory;
  QString arg;

  void execute(AppWindow &app) override { app.launchCommand(factory(app, arg), {}); }

  OpenCommandAction(CommandFactory factory, const QString &title, const QString &iconName,
                    const QString &arg = "")
      : AbstractAction(title, iconName), factory(factory), arg(arg) {}
};

class ColorListItem : public OmniList::AbstractVirtualItem, public OmniListView::IActionnable {
  QColor color;

  OmniListItemWidget *createWidget() const override {
    auto widget = new ColorTransformWidget();

    widget->setColor(color.name(), color);

    return widget;
  }

  QString id() const override { return color.name(); }

  int calculateHeight(int width) const override {
    static ColorTransformWidget *widget = nullptr;

    if (!widget) {
      widget = new ColorTransformWidget;
      widget->setColor("", "blue");
    }

    return widget->sizeHint().height();
  }

  QList<AbstractAction *> generateActions() const override { return {}; }

public:
  ColorListItem(QColor color) : color(color) {}
};

class BuiltinCommandListItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
protected:
  BuiltinCommand cmd;
  QString text;

  QList<AbstractAction *> generateActions() const override {
    return {new OpenBuiltinCommandAction(cmd, "Open command", text)};
  }

  QString id() const override { return cmd.name; }

  ItemData data() const override { return {.iconUrl = cmd.iconUrl, .name = cmd.name, .kind = "Command"}; }

public:
  BuiltinCommandListItem(const BuiltinCommand &cmd, const QString &text = "") : cmd(cmd), text(text) {}
};

class FallbackBuiltinCommandListItem : public BuiltinCommandListItem {
  QString id() const override { return BuiltinCommandListItem::id() + ".fallback"; }

public:
  FallbackBuiltinCommandListItem(const BuiltinCommand &cmd, const QString &text = "")
      : BuiltinCommandListItem(cmd, text) {}
};

static BuiltinCommand calculatorHistoryCommand{
    .name = "Calculator history",
    .iconUrl = OmniIconUrl(":assets/icons/calculator.png"),
    .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }};

class QuicklinkRootListItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
public:
  std::shared_ptr<Quicklink> link;

  std::unique_ptr<CompleterData> createCompleter() const override {
    return std::make_unique<CompleterData>(
        CompleterData{.placeholders = link->placeholders, .iconUrl = link->iconName});
  }

  QList<AbstractAction *> generateActions() const override {
    auto open = new OpenCompletedQuicklinkAction(link);
    auto edit = new EditQuicklinkAction(link);
    auto duplicate = new DuplicateQuicklinkAction(link);
    auto remove = new RemoveQuicklinkAction(link);

    return {open, edit, duplicate, remove};
  }

  ItemData data() const override {
    return {.iconUrl = link->iconName, .name = link->name, .kind = "Quicklink", ._iconColor = "red"};
  }

  QString id() const override { return QString("link-%1").arg(link->id); }

public:
  QuicklinkRootListItem(const std::shared_ptr<Quicklink> &link) : link(link) {}
  ~QuicklinkRootListItem() {}
};

class RootView : public OmniListView {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  Service<QuicklistDatabase> quicklinkDb;
  Service<CommandDatabase> commandDb;

  class AppListItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
    std::shared_ptr<DesktopEntry> app;
    Service<AppDatabase> appDb;

    QList<AbstractAction *> generateActions() const override {
      QList<AbstractAction *> actions;
      auto fileBrowser = appDb.defaultFileBrowser();
      auto textEditor = appDb.defaultTextEditor();

      actions << new OpenAppAction(app, "Open Application", {});

      for (const auto &desktopAction : app->actions) {
        actions << new OpenAppAction(desktopAction, desktopAction->name, {});
      }

      if (fileBrowser) { actions << new OpenAppAction(fileBrowser, "Open in folder", {app->path}); }

      if (textEditor) { actions << new OpenAppAction(textEditor, "Open desktop file", {app->path}); }

      actions << new CopyTextAction("Copy file path", app->path);

      return actions;
    }

    ItemData data() const override {
      return {
          .iconUrl = app->iconUrl(),
          .name = app->name,
          .kind = "Application",
      };
    }

    QString id() const override { return app->id; }

  public:
    AppListItem(const std::shared_ptr<DesktopEntry> &app, Service<AppDatabase> appDb)
        : app(app), appDb(appDb) {}
    ~AppListItem() {}
  };

  class FallbackQuicklinkListItem : public AbstractDefaultListItem, OmniListView::IActionnable {
    QString query;

  public:
    std::shared_ptr<Quicklink> link;

    QList<AbstractAction *> generateActions() const override {
      return {new OpenQuicklinkAction(link, {query}), new EditQuicklinkAction(link),
              new DuplicateQuicklinkAction(link)};
    }

    ItemData data() const override {
      return {.iconUrl = link->iconName, .name = link->name, .kind = "Quicklink"};
    }

    QString id() const override { return QString("fallback-link-%1").arg(link->id); }

  public:
    FallbackQuicklinkListItem(const std::shared_ptr<Quicklink> &link, const QString &fallbackQuery)
        : link(link), query(fallbackQuery) {}
  };

  /*
  class ExtensionListItem : public StandardListItem {
    Extension::Command cmd;

    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = ":icons/cog.svg"}, {25, 25}), cmd.name,
          cmd.title, "Command");
    }

    QList<AbstractAction *> createActions() const override {}

  public:
    ExtensionListItem(const Extension::Command &cmd)
        : StandardListItem(cmd.name, cmd.title, "Command", ThemeIconModel{.iconName = ":icons/cog.svg"}),
          cmd(cmd) {}
  };
  */

  class BaseCalculatorListItem : public OmniList::AbstractVirtualItem, public OmniListView::IActionnable {
  protected:
    CalculatorItem item;

    OmniListItemWidget *createWidget() const override { return new CalculatorListItemWidget(item); }

    int calculateHeight(int width) const override {
      static CalculatorListItemWidget ruler({});

      return ruler.sizeHint().height();
    }

    QString id() const override { return item.expression; }

    QList<AbstractAction *> generateActions() const override {
      QString sresult = QString::number(item.result);

      return {new CopyCalculatorResultAction(item, "Copy result", sresult),
              new CopyCalculatorResultAction(item, "Copy expression",
                                             QString("%1 = %2").arg(item.expression).arg(sresult)),
              new OpenBuiltinCommandAction(calculatorHistoryCommand, "Open in history", item.expression)

      };
    }

  public:
    BaseCalculatorListItem(const CalculatorItem &item) : item(item) {}
  };

public:
  // list without filtering
  void generateBaseSearch(ItemList &list) {
    list.push_back(std::make_unique<OmniList::VirtualSection>("Commands"));

    for (const auto &cmd : commandDb.list()) {
      list.push_back(std::make_unique<BuiltinCommandListItem>(cmd));
    }

    for (const auto &link : quicklinkDb.list()) {
      list.push_back(std::make_unique<QuicklinkRootListItem>(link));
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>("Apps"));

    for (auto &app : appDb.apps) {
      if (!app->displayable()) continue;

      list.push_back(std::make_unique<AppListItem>(app, appDb));
    }
  }

  void executeSearch(ItemList &list, const QString &s) override {
    if (s.isEmpty()) return generateBaseSearch(list);

    auto start = std::chrono::high_resolution_clock::now();

    auto fileBrowser = appDb.defaultFileBrowser();

    if (s.size() > 1) {
      list.push_back(std::make_unique<OmniList::VirtualSection>("Calculator"));

      te_parser parser;
      double result = parser.evaluate(s.toLatin1().data());

      if (!std::isnan(result)) {
        auto data = CalculatorItem{.expression = s, .result = result};
        auto item = std::make_unique<BaseCalculatorListItem>(data);

        list.push_back(std::move(item));
      }
    }

    if (QColor(s).isValid()) {
      list.push_back(std::make_unique<OmniList::VirtualSection>("Color"));
      list.push_back(std::make_unique<ColorListItem>(s));
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>("Results"));

    if (!s.isEmpty()) {
      for (const auto &link : quicklinkDb.list()) {
        if (!link->name.startsWith(s, Qt::CaseInsensitive)) continue;

        auto quicklink = std::make_unique<QuicklinkRootListItem>(link);

        list.push_back(std::move(quicklink));
      }
    }

    if (!s.isEmpty()) {
      for (auto &app : appDb.apps) {
        if (!app->displayable()) continue;

        for (const auto &word : app->name.split(" ")) {
          if (!word.startsWith(s, Qt::CaseInsensitive)) continue;

          list.push_back(std::make_unique<AppListItem>(app, appDb));
          break;
        }
      }
    }

    /*
  for (const auto &extension : extensionManager.extensions()) {
  for (const auto &cmd : extension.commands) {
    if (!cmd.name.contains(s, Qt::CaseInsensitive)) continue;

    auto item = std::make_shared<ExtensionListItem>(cmd);

    model->addItem(item);
  }
  }
    */

    auto &commandDb = service<CommandDatabase>();

    for (const auto &cmd : commandDb.list()) {
      if (!cmd.name.contains(s, Qt::CaseInsensitive)) continue;

      list.push_back(std::make_unique<BuiltinCommandListItem>(cmd));
    }

    if (s.isEmpty()) {
      for (const auto &app : appDb.apps) {
        if (app->displayable()) { list.push_back(std::make_unique<AppListItem>(app, appDb)); }
      }
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>(QString("Use \"%1\" with...").arg(s)));

    if (!s.isEmpty()) {
      for (const auto &link : quicklinkDb.list()) {
        if (link->placeholders.size() != 1) continue;

        list.push_back(std::make_unique<FallbackQuicklinkListItem>(link, s));
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    qDebug() << "root searched in " << duration << "ms";
  }

  void onMount() override { setSearchPlaceholderText("Search for apps or commands..."); }

  RootView(AppWindow &app)
      : OmniListView(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()), quicklinkDb(service<QuicklistDatabase>()),
        commandDb(service<CommandDatabase>()) {}

  ~RootView() {}
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
