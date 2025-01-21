#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "calculator.hpp"
#include "command.hpp"
#include "create-quicklink-command.hpp"
#include "extend/extension-command.hpp"
#include "extension_manager.hpp"
#include "files-command.hpp"
#include "manage-processes-command.hpp"
#include "manage-quicklinks-command.hpp"
#include "navigation-list-view.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/color_circle.hpp"
#include "ui/list-view.hpp"
#include "ui/test-list.hpp"
#include "ui/toast.hpp"
#include <QtConcurrent/QtConcurrent>
#include <functional>
#include <memory>
#include <numbers>
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

struct OpenQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto linkApp = app.appDb->getById(link->app);

    if (!linkApp) {
      app.statusBar->setToast("No app with id " + link->app, ToastPriority::Danger);
      return;
    }

    QString url = link->url;

    for (const auto &arg : args)
      url = url.arg(arg);

    if (!app.appDb->launch(*linkApp.get(), {url})) {
      app.statusBar->setToast("Failed to launch app", ToastPriority::Danger);
      return;
    }

    app.closeWindow(true);
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  OpenQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : AbstractAction("Open link", ThemeIconModel{.iconName = ":icons/link.svg"}), link(link), args(args) {}
};

struct EditQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto view = new EditCommandQuicklinkView(app, *link);

    emit app.pushView(
        view, {.navigation = NavigationStatus{
                   .title = "Edit link", .icon = ThemeIconModel{.iconName = ":assets/icons/quicklink.png"}}});
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  EditQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : AbstractAction("Edit link", ThemeIconModel{.iconName = ":icons/pencil.svg"}), link(link), args(args) {
  }
};

struct RemoveQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

public:
  void execute(AppWindow &app) override {
    bool removeResult = app.quicklinkDatabase->removeOne(link->id);

    if (removeResult) {
      app.statusBar->setToast("Removed link");
    } else {
      app.statusBar->setToast("Failed to remove link", ToastPriority::Danger);
    }
  }

  RemoveQuicklinkAction(const std::shared_ptr<Quicklink> &link)
      : AbstractAction("Remove link", ThemeIconModel{.iconName = ":icons/trash.svg"}), link(link) {}
};

struct DuplicateQuicklinkAction : public AbstractAction {
  std::shared_ptr<Quicklink> link;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto view = new DuplicateQuicklinkCommandView(app, *link);

    emit app.pushView(view, {.navigation = NavigationStatus{
                                 .title = "Duplicate link",
                                 .icon = ThemeIconModel{.iconName = ":assets/icons/quicklink.png"}}});
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  DuplicateQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : AbstractAction("Duplicate link", ThemeIconModel{.iconName = ":icons/duplicate.svg"}), link(link),
        args(args) {}
};

struct OpenCompletedQuicklinkAction : public OpenQuicklinkAction {
  void execute(AppWindow &app) {
    if (app.topBar->quickInput) { setArgs(app.topBar->quickInput->collectArgs()); }

    OpenQuicklinkAction::execute(app);
  }

  OpenCompletedQuicklinkAction(const std::shared_ptr<Quicklink> &link, const QList<QString> &args = {})
      : OpenQuicklinkAction(link) {}
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

class BuiltinCommandListItem : public StandardListItem {
  BuiltinCommand cmd;
  QString text;

  QList<AbstractAction *> createActions() const override {
    return {new OpenBuiltinCommandAction(cmd, "Open command", text)};
  }

public:
  BuiltinCommandListItem(const BuiltinCommand &cmd, const QString &text = "")
      : StandardListItem(cmd.name, "", "Command", ThemeIconModel{.iconName = cmd.iconName}), cmd(cmd),
        text(text) {}
};

static BuiltinCommand calculatorHistoryCommand{
    .name = "Calculator history",
    .iconName = ":assets/icons/calculator.png",
    .factory = [](AppWindow &app, const QString &s) { return new SingleViewCommand<CalculatorHistoryView>; }};

class QuicklinkRootListItem : public StandardListItem {
  Q_OBJECT

  std::shared_ptr<Quicklink> link;

public:
  std::unique_ptr<CompleterData> createCompleter() const override {
    return std::make_unique<CompleterData>(CompleterData{
        .placeholders = link->placeholders, .model = ThemeIconModel{.iconName = link->iconName}});
  }

  QList<AbstractAction *> createActions() const override {
    auto removeAction = new RemoveQuicklinkAction(link);

    connect(removeAction, &AbstractAction::didExecute, this, &QuicklinkRootListItem::removed);

    return {new OpenCompletedQuicklinkAction(link), new EditQuicklinkAction(link),
            new DuplicateQuicklinkAction(link), removeAction};
  }

  size_t id() const override {
    auto hash = qHash(QString("link-%1").arg(link->id));

    qDebug() << "quicklink id called for link id " << link->id << hash;

    return hash;
  }

public:
  QuicklinkRootListItem(const std::shared_ptr<Quicklink> &link)
      : StandardListItem(link->name, "", "Quicklink", ThemeIconModel{.iconName = link->iconName}),
        link(link) {}

  ~QuicklinkRootListItem() { qDebug() << "Destroyed quicklink item"; }

signals:
  void removed();
};

class RootView : public NavigationListView {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  Service<QuicklistDatabase> quicklinkDb;

  class FallbackQuicklinkListItem : public StandardListItem {
    std::shared_ptr<Quicklink> link;
    QString query;

  public:
    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = link->iconName}, {25, 25}), link->name, "",
          "Quicklink");
    }

    QList<AbstractAction *> createActions() const override {
      return {new OpenQuicklinkAction(link, {query}), new EditQuicklinkAction(link),
              new DuplicateQuicklinkAction(link)};
    }

  public:
    FallbackQuicklinkListItem(const std::shared_ptr<Quicklink> &link, const QString &fallbackQuery)
        : StandardListItem(link->name, "", "Quicklink", ThemeIconModel{.iconName = link->iconName}),
          link(link), query(fallbackQuery) {}
  };

  class ExtensionListItem : public StandardListItem {
    Extension::Command cmd;

    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = ":icons/cog.svg"}, {25, 25}), cmd.name,
          cmd.title, "Command");
    }

    QList<AbstractAction *> createActions() const override {
      auto open = new OpenCommandAction(
          [this](AppWindow &app, const QString &query) {
            return new ExtensionCommand(app, cmd.extensionId, cmd.name);
          },
          "Open command", "folder");

      return {open};
    }

  public:
    ExtensionListItem(const Extension::Command &cmd)
        : StandardListItem(cmd.name, cmd.title, "Command", ThemeIconModel{.iconName = ":icons/cog.svg"}),
          cmd(cmd) {}
  };

  class CalculatorListItem : public AbstractNativeListItem {
    CalculatorItem item;

    QWidget *createItem() const override { return new CalculatorListItemWidget(item); }

    int role() const override { return 1; }

    int height() const override { return 100; }

    QList<AbstractAction *> createActions() const override {
      QString sresult = QString::number(item.result);

      return {
          new CopyCalculatorResultAction(item, "Copy result", sresult),
          new CopyCalculatorResultAction(item, "Copy expression",
                                         QString("%1 = %2").arg(item.expression).arg(sresult)),
          new OpenBuiltinCommandAction(calculatorHistoryCommand, "Open in history", item.expression),
      };
    }

  public:
    CalculatorListItem(const CalculatorItem &item) : item(item) {}
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
      {.name = "Manage quicklinks",
       .iconName = ":assets/icons/quicklink.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<ManageQuicklinksView>; }},
      {.name = "Create quicklink",
       .iconName = ":assets/icons/quicklink.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<QuicklinkCommandView>; }},
      {.name = "Manage processes",
       .iconName = ":assets/icons/process-manager.png",
       .factory = [](AppWindow &app,
                     const QString &s) { return new SingleViewCommand<ManageProcessesMainView>; }},

  };

  QList<BuiltinCommand> fallbackCommands{
      {.name = "Search files",
       .iconName = ":assets/icons/files.png",
       .factory = [](AppWindow &app, const QString &query) { return new SingleViewCommand<FilesView>(); }},
  };

  QFutureWatcher<void> searchFutureWatcher;

  void handleLinkDeletion(std::shared_ptr<QuicklinkRootListItem> link) { model->removeItem(link); }

public:
  void onSearchChanged(const QString &s) override {
    if (searchFutureWatcher.isRunning()) {
      searchFutureWatcher.cancel();
      searchFutureWatcher.waitForFinished();
    }

    // auto future = QtConcurrent::run([this, s]() {
    auto start = std::chrono::high_resolution_clock::now();
    auto fileBrowser = appDb.defaultFileBrowser();

    model->beginReset();

    if (s.size() > 1) {
      model->beginSection("Calculator");
      Parser parser;

      if (auto result = parser.evaluate(s.toLatin1().data())) {
        auto value = result.value();
        auto data = CalculatorItem{.expression = s, .result = value.value, .unit = value.unit};
        auto item = std::make_shared<CalculatorListItem>(data);

        model->addItem(item);
      }
    }

    if (QColor(s).isValid()) {
      model->beginSection("Color");
      auto item = std::make_shared<ColorListItem>(s);

      model->addItem(item);
    }

    model->beginSection("Results");

    if (!s.isEmpty()) {
      for (const auto &link : quicklinkDb.list()) {
        if (!link->name.startsWith(s, Qt::CaseInsensitive)) continue;

        auto quicklink = std::make_shared<QuicklinkRootListItem>(link);

        connect(quicklink.get(), &QuicklinkRootListItem::removed, this,
                [this, quicklink]() { handleLinkDeletion(quicklink); });

        model->addItem(quicklink);
      }
    }

    if (!s.isEmpty()) {
      for (auto &app : appDb.apps) {
        if (!app->displayable()) continue;

        for (const auto &word : app->name.split(" ")) {
          if (!word.startsWith(s, Qt::CaseInsensitive)) continue;

          auto appItem = std::make_shared<AppListItem>(app, appDb);

          model->addItem(appItem);
          break;
        }
      }
    }

    for (const auto &extension : extensionManager.extensions()) {
      for (const auto &cmd : extension.commands) {
        if (!cmd.name.contains(s, Qt::CaseInsensitive)) continue;

        auto item = std::make_shared<ExtensionListItem>(cmd);

        model->addItem(item);
      }
    }

    for (const auto &cmd : builtinCommands) {
      if (!cmd.name.contains(s, Qt::CaseInsensitive)) continue;

      auto item = std::make_shared<BuiltinCommandListItem>(cmd);

      model->addItem(item);
    }

    if (!s.isEmpty()) {
      model->beginSection(QString("Use \"%1\" with...").arg(s));

      for (const auto &cmd : fallbackCommands) {
        auto item = std::make_shared<BuiltinCommandListItem>(cmd, s);

        model->addItem(item);
      }

      for (const auto &link : quicklinkDb.list()) {
        if (link->placeholders.size() != 1) continue;

        auto item = std::make_shared<FallbackQuicklinkListItem>(link, s);

        model->addItem(item);
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    qDebug() << "root searched in " << duration << "ms";

    model->endReset();
    //});

    // searchFutureWatcher.setFuture(future);
  }

  void onMount() override { setSearchPlaceholderText("Search for apps or commands..."); }

  RootView(AppWindow &app)
      : NavigationListView(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()), quicklinkDb(service<QuicklistDatabase>()),
        searchFutureWatcher(new QFutureWatcher<void>) {}

  ~RootView() { delete model; }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
