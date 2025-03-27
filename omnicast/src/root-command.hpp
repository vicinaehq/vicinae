#pragma once
#include "app/app-database.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/open-with-action.hpp"
#include "wm/hyprland/hyprland.hpp"
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
#include "theme.hpp"
#include "tinyexpr.hpp"
#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/color-transform-widget.hpp"
#include "ui/color_circle.hpp"
#include "ui/declarative-omni-list-view.hpp"
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
#include "wm/window-manager.hpp"

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

class ColorListItem : public OmniList::AbstractVirtualItem, public DeclarativeOmniListView::IActionnable {
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

class BuiltinCommandListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
protected:
  BuiltinCommand cmd;
  QString text;

  QList<AbstractAction *> generateActions() const override {
    return {new OpenBuiltinCommandAction(cmd, "Open command", text)};
  }

  QString id() const override { return cmd.name; }

  ItemData data() const override {
    return {.iconUrl = cmd.iconUrl, .name = cmd.name, .accessories = {{.text = "Command"}}};
  }

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

class QuicklinkRootListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
public:
  std::shared_ptr<Quicklink> link;
  Service<AbstractAppDatabase> appDb;

  std::unique_ptr<CompleterData> createCompleter() const override {
    return std::make_unique<CompleterData>(
        CompleterData{.placeholders = link->placeholders, .iconUrl = iconUrl()});
  }

  QList<AbstractAction *> generateActions() const override {
    auto open = new OpenCompletedQuicklinkAction(link);
    auto edit = new EditQuicklinkAction(link);
    auto duplicate = new DuplicateQuicklinkAction(link);
    auto remove = new RemoveQuicklinkAction(link);

    return {open, edit, duplicate, remove};
  }

  std::vector<ActionItem> generateActionPannel() const override {
    std::vector<ActionItem> items;

    items.push_back(ActionLabel("Quicklink"));
    items.push_back(std::make_unique<OpenCompletedQuicklinkAction>(link));
    items.push_back(std::make_unique<EditQuicklinkAction>(link));
    items.push_back(std::make_unique<DuplicateQuicklinkAction>(link));
    items.push_back(std::make_unique<OpenWithAction>(std::vector<QString>({}), appDb));
    items.push_back(std::make_unique<RemoveQuicklinkAction>(link));

    return items;
  }

  OmniIconUrl iconUrl() const {
    OmniIconUrl url(link->iconName);

    if (url.type() == OmniIconType::Builtin) { url.setBackgroundTint(ColorTint::Red); }

    return url;
  }

  ItemData data() const override {
    return {.iconUrl = iconUrl(), .name = link->name, .accessories = {{.text = "Quicklink"}}};
  }

  QString id() const override { return QString("link-%1").arg(link->id); }

public:
  QuicklinkRootListItem(Service<AbstractAppDatabase> appDb, const std::shared_ptr<Quicklink> &link)
      : appDb(appDb), link(link) {}
  ~QuicklinkRootListItem() {}
};

class RootView : public DeclarativeOmniListView {
  AppWindow &app;
  Service<AbstractAppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  Service<QuicklistDatabase> quicklinkDb;
  Service<CommandDatabase> commandDb;

  class AppListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
    std::shared_ptr<Application> app;
    Service<AbstractAppDatabase> appDb;

    QList<AbstractAction *> generateActions() const override {
      QList<AbstractAction *> actions;
      auto fileBrowser = appDb.fileBrowser();
      auto textEditor = appDb.textEditor();

      actions << new OpenAppAction(app, "Open Application", {});

      for (const auto &desktopAction : app->actions()) {
        actions << new OpenAppAction(desktopAction, desktopAction->name(), {});
      }

      if (fileBrowser) { actions << new OpenAppAction(fileBrowser, "Open in folder", {app->id()}); }

      if (textEditor) { actions << new OpenAppAction(textEditor, "Open desktop file", {app->id()}); }

      actions << new CopyTextAction("Copy file path", app->id());

      return actions;
    }

    ItemData data() const override {
      return {.iconUrl = app->iconUrl(), .name = app->name(), .accessories = {{.text = "Application"}}};
    }

    QString id() const override { return app->id(); }

  public:
    AppListItem(const std::shared_ptr<Application> &app, Service<AbstractAppDatabase> appDb)
        : app(app), appDb(appDb) {}
    ~AppListItem() {}
  };

  class FallbackQuicklinkListItem : public AbstractDefaultListItem, DeclarativeOmniListView::IActionnable {
    QString query;

  public:
    std::shared_ptr<Quicklink> link;

    QList<AbstractAction *> generateActions() const override {
      return {new OpenQuicklinkAction(link, {query}), new EditQuicklinkAction(link),
              new DuplicateQuicklinkAction(link)};
    }

    ItemData data() const override {
      return {.iconUrl = link->iconName, .name = link->name, .accessories = {{.text = "Quicklink"}}};
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

  class BaseCalculatorListItem : public OmniList::AbstractVirtualItem,
                                 public DeclarativeOmniListView::IActionnable {
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
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> generateBaseSearch() {
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> list;
    list.push_back(std::make_unique<OmniList::VirtualSection>("Commands"));

    for (const auto &cmd : commandDb.list()) {
      list.push_back(std::make_unique<BuiltinCommandListItem>(cmd));
    }

    for (const auto &link : quicklinkDb.list()) {
      list.push_back(std::make_unique<QuicklinkRootListItem>(appDb, link));
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>("Apps"));

    for (auto &app : appDb.list()) {
      if (!app->displayable()) continue;

      list.push_back(std::make_unique<AppListItem>(app, appDb));
    }

    return list;
  }

  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> generateList(const QString &s) override {
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> list;

    if (s.isEmpty()) return generateBaseSearch();

    auto start = std::chrono::high_resolution_clock::now();

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

        auto quicklink = std::make_unique<QuicklinkRootListItem>(appDb, link);

        list.push_back(std::move(quicklink));
      }
    }

    if (!s.isEmpty()) {
      for (auto &app : appDb.list()) {
        if (!app->displayable()) continue;

        for (const auto &word : app->name().split(" ")) {
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
      for (const auto &app : appDb.list()) {
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

    return list;
  }

  void onMount() override {
    setSearchPlaceholderText("Search for apps or commands...");

    auto watcher = new QFutureWatcher<std::vector<AiModel>>;

    connect(watcher, &QFutureWatcher<std::vector<AiModel>>::finished, this, [watcher]() {
      if (watcher->isCanceled()) {
        qDebug() << "cancelled";
        return;
      }

      if (watcher->isFinished()) {
        for (const auto &model : watcher->result()) {
          qDebug() << "MODEL " << model.id;
        }
      }
    });

    watcher->setFuture(app.aiProvider->models());
  }

  RootView(AppWindow &app)
      : DeclarativeOmniListView(app), app(app), appDb(service<AbstractAppDatabase>()),
        extensionManager(service<ExtensionManager>()), quicklinkDb(service<QuicklistDatabase>()),
        commandDb(service<CommandDatabase>()) {}
  ~RootView() {}
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
