#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "calculator.hpp"
#include "command.hpp"
#include "extend/extension-command.hpp"
#include "extension_manager.hpp"
#include "manage-quicklinks-command.hpp"
#include "navigation-list-view.hpp"
#include "omnicast.hpp"
#include "ui/action_popover.hpp"
#include "ui/color_circle.hpp"
#include "ui/list-view.hpp"
#include "ui/native-list.hpp"
#include "ui/test-list.hpp"
#include <functional>
#include <memory>
#include <qcoreevent.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qmap.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qwidget.h>

struct OpenAppAction : public AbstractAction {
  std::shared_ptr<DesktopExecutable> application;
  QList<QString> args;

  void execute(AppWindow &app) override { application->launch(args); }

  OpenAppAction(const std::shared_ptr<DesktopExecutable> &app,
                const QString &title, const QList<QString> args)
      : AbstractAction(title, ThemeIconModel{.iconName = app->iconName()}),
        application(app), args(args) {}
};

struct Action {
  std::function<void(void)> execute;
};

struct OpenAppInFileBrowserAction {
  std::shared_ptr<DesktopEntry> app;
  std::shared_ptr<DesktopExecutable> fileBrowser;
};

using AppAction = std::variant<OpenAppAction, OpenAppInFileBrowserAction>;

struct ExtensionLoadAction {
  Extension::Command command;
};

struct BuiltinCommand {
  QString name;
  QString iconName;
  std::function<View *(AppWindow &app, const QString &)> factory;
};

struct OpenBuiltinCommandAction : public AbstractAction {
  BuiltinCommand cmd;
  QString text;

  void execute(AppWindow &app) override {
    emit app.pushView(cmd.factory(app, text));
  }

  OpenBuiltinCommandAction(const BuiltinCommand &cmd, const QString &text = "")
      : AbstractAction("Open command",
                       ThemeIconModel{.iconName = cmd.iconName}),
        cmd(cmd), text(text) {}
};

struct FallbackCommand {
  QString name;
  QString iconName;
  std::function<View *(AppWindow &app, const QString &query)> factory;
};

struct OpenHomeDirectoryAction {};

using ExtensionAction = std::variant<ExtensionLoadAction>;
using RootCalculatorAction = std::variant<CalculatorCopyResultAction>;
using ActionType = std::variant<AppAction, ExtensionAction,
                                RootCalculatorAction, OpenHomeDirectoryAction>;

struct AppItem {
  std::shared_ptr<DesktopEntry> entry;
};

struct CalculatorItem {
  QString expression;
  double result;
  std::optional<Unit> unit;
};

struct ColorItem {
  QColor color;
};

class CalculatorListItem : public AbstractNativeListItem {
  CalculatorItem item;

  QWidget *createItem() const override {
    auto exprLabel = new QLabel(item.expression);

    exprLabel->setProperty("class", "transform-left");

    auto answerLabel = new QLabel(QString::number(item.result));
    answerLabel->setProperty("class", "transform-left");

    auto left = new VStack(exprLabel, new Chip("Expression"));
    auto right = new VStack(
        answerLabel, new Chip(item.unit ? QString(item.unit->displayName.data())
                                        : "Answer"));

    return new TransformResult(left, right);
  }

  QList<AbstractAction *> createActions() const override { return {}; }

public:
  CalculatorListItem(const CalculatorItem &item) : item(item) {}
};

class AppListItem : public AbstractNativeListItem {
  std::shared_ptr<DesktopEntry> app;
  Service<AppDatabase> appDb;

  QWidget *createItem() const override {
    return new ListItemWidget(
        ImageViewer::createFromModel(
            ThemeIconModel{.iconName = app->iconName()}, {25, 25}),
        app->name, "", "Application");
  }

  QList<AbstractAction *> createActions() const override {
    QList<AbstractAction *> actions;
    auto fileBrowser = appDb.defaultFileBrowser();
    auto textEditor = appDb.defaultTextEditor();

    actions.push_back(new OpenAppAction(app, "Open", {}));

    if (fileBrowser) {
      actions.push_back(
          new OpenAppAction(fileBrowser, "Open in folder", {app->path}));
    }

    if (textEditor) {
      actions.push_back(
          new OpenAppAction(textEditor, "Open desktop file", {app->path}));
    }

    return actions;
  }

public:
  AppListItem(const std::shared_ptr<DesktopEntry> &app,
              Service<AppDatabase> appDb)
      : app(app), appDb(appDb) {}
  ~AppListItem() { qDebug() << "destroy app list item"; }
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

  QList<AbstractAction *> createActions() const override { return {}; }

public:
  ColorListItem(QColor color) : color(color) {}
};

class BuiltinCommandListItem : public AbstractNativeListItem {
  BuiltinCommand cmd;

  QWidget *createItem() const override {
    return new ListItemWidget(
        ImageViewer::createFromModel(ThemeIconModel{.iconName = cmd.iconName},
                                     {25, 25}),
        cmd.name, "", "Command");
  }

  QList<AbstractAction *> createActions() const override {
    return {new OpenBuiltinCommandAction(cmd)};
  }

public:
  BuiltinCommandListItem(const BuiltinCommand &cmd) : cmd(cmd) {}
};

class FallbackCommandListItem : public AbstractNativeListItem {
  FallbackCommand cmd;

  QWidget *createItem() const override {
    return new ListItemWidget(
        ImageViewer::createFromModel(ThemeIconModel{.iconName = cmd.iconName},
                                     {25, 25}),
        cmd.name, "", "Command");
  }

public:
  FallbackCommandListItem(const FallbackCommand &cmd) : cmd(cmd) {}
};

using RootItem =
    std::variant<AppItem, ColorItem, CalculatorItem, Extension::Command,
                 BuiltinCommand, FallbackCommand>;

class RootView : public NavigationListView {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  QHash<QString, std::variant<Extension::Command>> itemMap;
  QMimeDatabase mimeDb;

  NativeList *list = new NativeList();

  NewActionPannelModel *actionModel = new NewActionPannelModel;

  QList<BuiltinCommand> builtinCommands{
      {.name = "Search files",
       .iconName = ":assets/icons/files.png",
       .factory =
           [](AppWindow &app, const QString &s) {
             return new CalculatorHistoryView(app);
           }},
      {.name = "Calculator history",
       .iconName = ":assets/icons/calculator.png",
       .factory =
           [](AppWindow &app, const QString &s) {
             return new CalculatorHistoryView(app);
           }},
      {.name = "Manage quicklinks",
       .iconName = ":assets/icons/quicklink.png",
       .factory =
           [](AppWindow &app, const QString &s) {
             return new ManageQuicklinksView(app);
           }},
      {.name = "Create quicklink",
       .iconName = ":assets/icons/quicklink.png",
       .factory =
           [](AppWindow &app, const QString &s) {
             return new CalculatorHistoryView(app);
           }},

  };

  QList<BuiltinCommand> fallbackCommands{
      {.name = "Search files",
       .iconName = ":assets/icons/files.png",
       .factory =
           [](AppWindow &app, const QString &query) {
             return new CalculatorHistoryView(app);
           }},
  };

public:
  void onSearchChanged(const QString &s) override {
    auto start = std::chrono::high_resolution_clock::now();
    auto fileBrowser = appDb.defaultFileBrowser();

    model->beginReset();

    if (s.size() > 1) {
      model->beginSection("Calculator");
      Parser parser;

      if (auto result = parser.evaluate(s.toLatin1().data())) {
        auto value = result.value();
        auto data = CalculatorItem{
            .expression = s, .result = value.value, .unit = value.unit};
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
      for (auto &app : appDb.apps) {
        if (!app->displayable())
          continue;

        for (const auto &word : app->name.split(" ")) {
          if (!word.startsWith(s, Qt::CaseInsensitive))
            continue;

          auto appItem = std::make_shared<AppListItem>(app, appDb);

          model->addItem(appItem);
          break;
        }
      }
    }

    for (const auto &extension : extensionManager.extensions()) {
      for (const auto &cmd : extension.commands) {
        if (!cmd.name.contains(s, Qt::CaseInsensitive))
          continue;

        auto item = std::make_shared<Extension::Command>(cmd);
      }
    }

    for (const auto &cmd : builtinCommands) {
      if (!cmd.name.contains(s, Qt::CaseInsensitive))
        continue;

      auto item = std::make_shared<BuiltinCommandListItem>(cmd);

      model->addItem(item);
    }

    model->beginSection(QString("Use \"%1\" with...").arg(s));

    for (const auto &cmd : fallbackCommands) {
      auto item = std::make_shared<BuiltinCommandListItem>(cmd);

      model->addItem(item);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    qDebug() << "root searched in " << duration << "ms";

    model->endReset();
  }

  void launchFallbackCommand(const FallbackCommand &cmd, const QString &query) {
    emit pushView(cmd.factory(app, query));
  }

  void openApp(std::shared_ptr<DesktopEntry> app) { app->launch(); }

  void openAppInFileBrowser(std::shared_ptr<DesktopEntry> application,
                            std::shared_ptr<DesktopExecutable> fileBrowser) {
    // fileBrowser->launch({app->path});
    emit pushView(new CalculatorHistoryView(app));
  }

  void openAppDesktopFile(std::shared_ptr<DesktopExecutable> textEditor,
                          QString path) {
    qDebug() << "Open desktop file" << path;
    textEditor->launch({path});
  }

  void copyCalculatorResult() {}

  void loadExtension(Extension::Command command) {
    emit launchCommand(
        new ExtensionCommand(app, command.extensionId, command.name));
  }

  void onMount() override {
    setSearchPlaceholderText("Search for apps or commands...");
  }

  RootView(AppWindow &app)
      : NavigationListView(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()) {}

  ~RootView() { delete model; }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
