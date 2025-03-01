#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "command.hpp"
#include "create-quicklink-command.hpp"
#include "emoji-command.hpp"
#include "extension_manager.hpp"
#include "files-command.hpp"
#include "icon-browser-command.hpp"
#include "manage-processes-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "navigation-list-view.hpp"
#include "ollama-command.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "tinyexpr.hpp"
#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/color_circle.hpp"
#include "ui/grid-view.hpp"
#include "ui/list-view.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "ui/test-list.hpp"
#include "quicklink-actions.hpp"
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
#include <qthreadpool.h>
#include <qwidget.h>

struct BuiltinCommand {
  QString name;
  QString iconName;
  std::function<ViewCommand *(AppWindow &app, const QString &)> factory;
};

struct OpenBuiltinCommandAction : public AbstractAction {
  BuiltinCommand cmd;
  QString text;

  void execute(AppWindow &app) override {
    emit app.launchCommand(cmd.factory(app, text),
                           {.searchQuery = text,
                            .navigation = NavigationStatus{
                                .title = cmd.name, .icon = ThemeIconModel{.iconName = cmd.iconName}}});
  }

  OpenBuiltinCommandAction(const BuiltinCommand &cmd, const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, ThemeIconModel{.iconName = cmd.iconName}), cmd(cmd), text(text) {}
};

using CommandFactory = std::function<ViewCommand *(AppWindow &app, const QString &arg)>;

struct OpenCommandAction : public AbstractAction {
  CommandFactory factory;
  QString arg;

  void execute(AppWindow &app) override { app.launchCommand(factory(app, arg), {}); }

  OpenCommandAction(CommandFactory factory, const QString &title, const QString &iconName,
                    const QString &arg = "")
      : AbstractAction(title, ThemeIconModel{.iconName = iconName}), factory(factory), arg(arg) {}
};

class ColorListItem : public AbstractNativeListItem {
  QColor color;

  QWidget *createItem() const override {
    auto circle = new ColorCircle(color.name(), QSize(60, 60));

    circle->setStroke("#BBB", 3);

    auto colorLabel = new QLabel(color.name());

    colorLabel->setProperty("class", "transform-left");

    auto left = new VStack(colorLabel, new Chip("HEX"));
    auto right = new VStack(circle, new Chip(color.name()));

    return new TransformResult(left, right);
  }

  int height() const override { return 120; }

  int role() const override { return 2; }

  QList<AbstractAction *> createActions() const override { return {}; }

public:
  ColorListItem(QColor color) : color(color) {}
};

class BuiltinCommandListItem : public OmniListView::AbstractActionnableItem {
protected:
  BuiltinCommand cmd;
  QString text;

  QList<AbstractAction *> generateActions() const override {
    return {new OpenBuiltinCommandAction(cmd, "Open command", text)};
  }

  QString id() const override { return cmd.name; }

  ItemData data() const override { return {.icon = cmd.iconName, .name = cmd.name, .kind = "Command"}; }

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
    .iconName = ":assets/icons/calculator.png",
    .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }};

class QuicklinkRootListItem : public OmniListView::AbstractActionnableItem {
public:
  std::shared_ptr<Quicklink> link;

  CompleterData *createCompleter() const {
    return new CompleterData{.placeholders = link->placeholders,
                             .model = ThemeIconModel{.iconName = link->iconName}};
  }

  QList<AbstractAction *> generateActions() const override {
    auto open = new OpenCompletedQuicklinkAction(link);
    auto edit = new EditQuicklinkAction(link);
    auto duplicate = new DuplicateQuicklinkAction(link);
    auto remove = new RemoveQuicklinkAction(link);

    qDebug() << "createActions";

    // connect(edit, &EditQuicklinkAction::edited, this, &QuicklinkRootListItem::edited);
    // connect(duplicate, &DuplicateQuicklinkAction::duplicated, this, &QuicklinkRootListItem::duplicated);
    // connect(remove, &AbstractAction::didExecute, this, &QuicklinkRootListItem::removed);

    return {open, edit, duplicate, remove};
  }

  ItemData data() const override { return {.icon = link->iconName, .name = link->name, .kind = "Quicklink"}; }

  QString id() const override { return QString("link-%1").arg(link->id); }

public:
  QuicklinkRootListItem(const std::shared_ptr<Quicklink> &link) : link(link) {}

  ~QuicklinkRootListItem() { qDebug() << "[-LINK] link with id" << link->id; }

  // void edited();
  // void removed();
  // void duplicated();
};

class RootView : public OmniListView {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  Service<QuicklistDatabase> quicklinkDb;

  class AppListItem : public OmniListView::AbstractActionnableItem {
    std::shared_ptr<DesktopEntry> app;
    Service<AppDatabase> appDb;

    QList<AbstractAction *> generateActions() const override {
      QList<AbstractAction *> actions;
      auto fileBrowser = appDb.defaultFileBrowser();
      auto textEditor = appDb.defaultTextEditor();

      actions << new OpenAppAction(app, "Open", {});

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
          .icon = app->iconName(),
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

  class FallbackQuicklinkListItem : public OmniListView::AbstractActionnableItem {
    QString query;

  public:
    std::shared_ptr<Quicklink> link;

    QList<AbstractAction *> generateActions() const override {
      return {new OpenQuicklinkAction(link, {query}), new EditQuicklinkAction(link),
              new DuplicateQuicklinkAction(link)};
    }

    ItemData data() const override {
      return {.icon = link->iconName, .name = link->name, .kind = "Quicklink"};
    }

    QString id() const override { return QString("fallback-link-%1").arg(link->id); }

  public:
    FallbackQuicklinkListItem(const std::shared_ptr<Quicklink> &link, const QString &fallbackQuery)
        : link(link), query(fallbackQuery) {}
  };

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

  class RootCalculatorListItem : public BaseCalculatorListItem {
    QList<AbstractAction *> createActions() const override {
      auto actions = BaseCalculatorListItem::createActions();

      actions << new OpenBuiltinCommandAction(calculatorHistoryCommand, "Open in history", item.expression);

      return actions;
    }

  public:
    RootCalculatorListItem(const CalculatorItem &item) : BaseCalculatorListItem(item) {}
  };

  QList<BuiltinCommand> builtinCommands{
      {.name = "Search files",
       .iconName = ":assets/icons/files.png",
       .factory =
           [](AppWindow &app, const QString &s) {
             auto file = new SingleViewCommand<FilesView>;

             return file;
           }},
      {.name = "Calculator history",
       .iconName = ":assets/icons/calculator.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }},
      {.name = "Create quicklink",
       .iconName = ":assets/icons/quicklink.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<QuicklinkCommandView>; }},
      {.name = "Manage quicklinks",
       .iconName = ":assets/icons/quicklink.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<ManageQuicklinksView>; }},
      {.name = "Search Emoji & Symbols",
       .iconName = ":assets/icons/emoji.png",
       .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<EmojiView>; }},
      {.name = "Manage processes",
       .iconName = ":assets/icons/process-manager.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<ManageProcessesMainView>; }},
      {.name = "Browse bundled icons",
       .iconName = ":/icons/link.svg",
       .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<IconBrowserView>; }},

  };

  QList<BuiltinCommand> fallbackCommands{
      {.name = "Search files",
       .iconName = ":assets/icons/files.png",
       .factory = [](AppWindow &app, const QString &query) { return new SingleViewCommand<FilesView>(); }},
  };

  /*
  void removeQuicklink(QuicklinkRootListItem *item) {
    grid->removeByKey(item->key());
    grid->removeByKey(item->fallbackKey());
    grid->calculateLayout();
  }

  void editedQuicklink(QuicklinkRootListItem *item) {
    grid->invalidateCacheKey(item->key());
    grid->invalidateCacheKey(item->fallbackKey());

    if (auto completer = item->createCompleter()) {
      auto values = app.topBar->quickInput->collectArgs();

      app.topBar->destroyQuicklinkCompleter();
      app.topBar->activateQuicklinkCompleter(*completer);
      app.topBar->quickInput->setValues(values);
      delete completer;
    }

    grid->calculateLayout();
  }
  */

public:
  void executeSearch(ItemList &list, const QString &s) override {
    auto start = std::chrono::high_resolution_clock::now();
    auto fileBrowser = appDb.defaultFileBrowser();

    if (s.size() > 1) {
      list.push_back(std::make_unique<OmniList::VirtualSection>("Calculator"));

      te_parser parser;
      double result = parser.evaluate(s.toLatin1().data());

      if (!std::isnan(result)) {
        auto data = CalculatorItem{.expression = s, .result = result};
        auto item = std::make_shared<RootCalculatorListItem>(data);

        // calculator->addItem(new BasicCalculatorItem(data));
      }
    }

    /*
  if (QColor(s).isValid()) {
  model->beginSection("Color");
  auto item = std::make_shared<ColorListItem>(s);

  model->addItem(item);
  }
    */

    list.push_back(std::make_unique<OmniList::VirtualSection>("Results"));

    if (!s.isEmpty()) {
      for (const auto &link : quicklinkDb.list()) {
        if (!link->name.startsWith(s, Qt::CaseInsensitive)) continue;

        auto quicklink = std::make_unique<QuicklinkRootListItem>(link);

        /*
connect(quicklink, &QuicklinkRootListItem::edited, this,
        [this, quicklink]() { editedQuicklink(quicklink); });
connect(quicklink, &QuicklinkRootListItem::duplicated, this, [this, s]() { reselect(); });
connect(quicklink, &QuicklinkRootListItem::removed, this,
        [this, quicklink]() { removeQuicklink(quicklink); });
        */

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

    for (const auto &cmd : builtinCommands) {
      if (!cmd.name.contains(s, Qt::CaseInsensitive)) continue;

      list.push_back(std::make_unique<BuiltinCommandListItem>(cmd));
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>(QString("Use \"%1\" with...").arg(s)));

    if (!s.isEmpty()) {
      for (const auto &cmd : fallbackCommands) {
        list.push_back(std::make_unique<FallbackBuiltinCommandListItem>(cmd, s));
      }

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
        extensionManager(service<ExtensionManager>()), quicklinkDb(service<QuicklistDatabase>()) {}

  ~RootView() {}
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
