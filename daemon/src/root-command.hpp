#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator-database.hpp"
#include "calculator-history-command.hpp"
#include "calculator.hpp"
#include "command.hpp"
#include "extend/extension-command.hpp"
#include "extend/extension-list-controller.hpp"
#include "extension_manager.hpp"
#include "manage-quicklinks-command.hpp"
#include "navigation-list-view.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
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

  OpenBuiltinCommandAction(const BuiltinCommand &cmd,
                           const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, ThemeIconModel{.iconName = cmd.iconName}),
        cmd(cmd), text(text) {}
};

using CommandFactory =
    std::function<ViewCommand *(AppWindow &app, const QString &arg)>;

struct OpenCommandAction : public AbstractAction {
  CommandFactory factory;
  QString arg;

  void execute(AppWindow &app) override {
    emit app.launchCommand(factory(app, arg));
  }

  OpenCommandAction(CommandFactory factory, const QString &title,
                    const QString &iconName, const QString &arg = "")
      : AbstractAction(title, ThemeIconModel{.iconName = iconName}),
        factory(factory), arg(arg) {}
};

struct CalculatorItem {
  QString expression;
  double result;
  std::optional<Unit> unit;
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

class CopyTextAction : public AbstractAction {
  QString text;

public:
  void execute(AppWindow &app) override {
    app.clipboardService->copyText(text);
    app.statusBar->setToast("Copied in clipboard");
  }

  CopyTextAction(const QString &title, const QString &text)
      : AbstractAction(title, ThemeIconModel{.iconName = "clipboard"}),
        text(text) {}
};

class CopyCalculatorResultAction : public CopyTextAction {
  CalculatorItem item;

public:
  void execute(AppWindow &app) override {
    app.calculatorDatabase->saveComputation(item.expression,
                                            QString::number(item.result));
    CopyTextAction::execute(app);
  }

  CopyCalculatorResultAction(const CalculatorItem &item, const QString &title,
                             const QString &copyText)
      : CopyTextAction(title, copyText), item(item) {}
};

static BuiltinCommand calculatorHistoryCommand{
    .name = "Calculator history",
    .iconName = ":assets/icons/calculator.png",
    .factory = [](AppWindow &app, const QString &s) {
      return new CalculatorHistoryView(app);
    }};

class RootView : public NavigationListView {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  Service<QuicklistDatabase> quicklinkDb;
  QHash<QString, std::variant<Extension::Command>> itemMap;
  QMimeDatabase mimeDb;

  class FallbackCommandListItem : public AbstractNativeListItem {
  public:
    QList<AbstractAction *> createActions() const override { return {}; }
  };

  class QuicklinkRootListItem : public AbstractNativeListItem {
    std::shared_ptr<Quicklink> link;

  public:
    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(
              ThemeIconModel{.iconName = link->iconName}, {25, 25}),
          link->name, "", "Quicklink");
    }

    std::unique_ptr<CompleterData> createCompleter() const override {
      return std::make_unique<CompleterData>(
          CompleterData{.placeholders = link->placeholders,
                        .model = ThemeIconModel{.iconName = link->iconName}});
    }

    QList<AbstractAction *> createActions() const override { return {}; }

  public:
    QuicklinkRootListItem(const std::shared_ptr<Quicklink> &link)
        : link(link) {}
  };

  class FallbackQuicklinkListItem : public FallbackCommandListItem {
    std::unique_ptr<QuicklinkRootListItem> quicklink;

    QList<AbstractAction *> createActions() const override {
      QList<AbstractAction *> actions{};

      actions << quicklink->createActions();

      return actions;
    }

    QWidget *createItem() const override { return quicklink->createItem(); }

  public:
    FallbackQuicklinkListItem(const std::shared_ptr<Quicklink> &link)
        : quicklink(std::make_unique<QuicklinkRootListItem>(link)) {}
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

      actions << new OpenAppAction(app, "Open", {});

      if (fileBrowser) {
        actions << new OpenAppAction(fileBrowser, "Open in folder",
                                     {app->path});
      }

      if (textEditor) {
        actions << new OpenAppAction(textEditor, "Open desktop file",
                                     {app->path});
      }

      actions << new CopyTextAction("Copy file path", app->path);

      return actions;
    }

  public:
    AppListItem(const std::shared_ptr<DesktopEntry> &app,
                Service<AppDatabase> appDb)
        : app(app), appDb(appDb) {}
    ~AppListItem() { qDebug() << "destroy app list item"; }
  };

  class ExtensionListItem : public AbstractNativeListItem {
    Extension::Command cmd;

    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = "folder"},
                                       {25, 25}),
          cmd.name, cmd.title, "Command");
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
    ExtensionListItem(const Extension::Command &cmd) : cmd(cmd) {}
  };

  class CalculatorListItem : public AbstractNativeListItem {
    CalculatorItem item;

    QWidget *createItem() const override {
      auto exprLabel = new QLabel(item.expression);

      exprLabel->setProperty("class", "transform-left");

      auto answerLabel = new QLabel(QString::number(item.result));
      answerLabel->setProperty("class", "transform-left");

      auto left = new VStack(exprLabel, new Chip("Expression"));
      auto right =
          new VStack(answerLabel,
                     new Chip(item.unit ? QString(item.unit->displayName.data())
                                        : "Answer"));

      return new TransformResult(left, right);
    }

    QList<AbstractAction *> createActions() const override {
      QString sresult = QString::number(item.result);

      return {
          new CopyCalculatorResultAction(item, "Copy result", sresult),
          new CopyCalculatorResultAction(
              item, "Copy expression",
              QString("%1 = %2").arg(item.expression).arg(sresult)),
          new OpenBuiltinCommandAction(calculatorHistoryCommand,
                                       "Open history"),
      };
    }

  public:
    CalculatorListItem(const CalculatorItem &item) : item(item) {}
  };

  NativeList *list = new NativeList();

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
      for (const auto &link : quicklinkDb.list()) {
        if (!link->name.contains(s, Qt::CaseInsensitive))
          continue;

        model->addItem(std::make_shared<QuicklinkRootListItem>(link));
      }
    }

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

        auto item = std::make_shared<ExtensionListItem>(cmd);

        model->addItem(item);
      }
    }

    for (const auto &cmd : builtinCommands) {
      if (!cmd.name.contains(s, Qt::CaseInsensitive))
        continue;

      auto item = std::make_shared<BuiltinCommandListItem>(cmd);

      model->addItem(item);
    }

    if (!s.isEmpty()) {
      model->beginSection(QString("Use \"%1\" with...").arg(s));

      for (const auto &cmd : fallbackCommands) {
        auto item = std::make_shared<BuiltinCommandListItem>(cmd);

        model->addItem(item);
      }

      for (const auto &link : quicklinkDb.list()) {
        auto item = std::make_shared<FallbackQuicklinkListItem>(link);

        model->addItem(item);
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    qDebug() << "root searched in " << duration << "ms";

    model->endReset();
  }

  void loadExtension(Extension::Command command) {
    emit launchCommand(
        new ExtensionCommand(app, command.extensionId, command.name));
  }

  void onMount() override {
    setSearchPlaceholderText("Search for apps or commands...");
  }

  RootView(AppWindow &app)
      : NavigationListView(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()),
        quicklinkDb(service<QuicklistDatabase>()) {}

  ~RootView() { delete model; }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
