#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator-history-command.hpp"
#include "calculator.hpp"
#include "command.hpp"
#include "commands/calculator-history/calculator-history.hpp"
#include "extend/action-model.hpp"
#include "extend/extension-command.hpp"
#include "extension_manager.hpp"
#include "omnicast.hpp"
#include "ui/action_popover.hpp"
#include "ui/color_circle.hpp"
#include "ui/list-view.hpp"
#include "ui/native-list.hpp"
#include <functional>
#include <qcoreevent.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qmap.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qwidget.h>

struct OpenAppAction {
  std::shared_ptr<DesktopEntry> app;

  OpenAppAction() : app(app) {}
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

using RootItem =
    std::variant<AppItem, ColorItem, CalculatorItem, Extension::Command>;

class RootView : public View, public TypedNativeListDelegate<RootItem> {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  QHash<QString, std::variant<Extension::Command>> itemMap;
  QMimeDatabase mimeDb;

  TypedNativeListModel<RootItem> *model = new TypedNativeListModel<RootItem>();
  NativeList *list = new NativeList();

  NewActionPannelModel *actionModel = new NewActionPannelModel;

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

        model->addItem(CalculatorItem{
            .expression = s, .result = value.value, .unit = value.unit});
      }
    }

    if (QColor(s).isValid()) {
      model->beginSection("Color");
      model->addItem(ColorItem{QColor(s)});
    }

    model->beginSection("Results");

    if (!s.isEmpty()) {
      for (auto &app : appDb.apps) {
        if (!app->displayable())
          continue;

        for (const auto &word : app->name.split(" ")) {
          if (!word.startsWith(s, Qt::CaseInsensitive))
            continue;

          model->addItem(AppItem{app});
          break;
        }
      }
    }

    for (const auto &extension : extensionManager.extensions()) {
      for (const auto &cmd : extension.commands) {
        if (!cmd.name.contains(s, Qt::CaseInsensitive))
          continue;

        model->addItem(cmd);
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    qDebug() << "root searched in " << duration << "ms";

    model->endReset();
  }

  void variantSelectionChanged(const RootItem &variant) override {
    QList<ActionData> actions;

    if (auto app = std::get_if<AppItem>(&variant)) {
      auto fileBrowser = appDb.defaultFileBrowser();
      auto textEditor = appDb.defaultTextEditor();

      auto fn = std::bind(&RootView::openApp, this, app->entry);
      actions = {
          ActionData{.title = "Open app",
                     .icon = {.iconName = app->entry->iconName()},
                     .execute = fn},
          ActionData{.title = "Open in " + fileBrowser->name,
                     .icon = {.iconName = fileBrowser->iconName()},
                     .execute = std::bind(&RootView::openAppInFileBrowser, this,
                                          app->entry, fileBrowser)},
          ActionData{.title = "Open desktop file",
                     .icon = {.iconName = textEditor->iconName()},
                     .execute = std::bind(&RootView::openAppDesktopFile, this,
                                          textEditor, app->entry->path)},
      };
    }

    if (auto calc = std::get_if<CalculatorItem>(&variant)) {
      actions = {ActionData{.title = "Copy result",
                            .icon = {.iconName = "pcbcalculator"},
                            .execute = std::bind(
                                &RootView::copyCalculatorResult, this)},
                 ActionData{.title = "Open history",
                            .icon = {.iconName = "pcbcalculator"},
                            .execute = [this]() {
                              emit pushView(new CalculatorHistoryView(app));
                            }}};
    }

    if (auto cmd = std::get_if<Extension::Command>(&variant)) {
      actions = {ActionData{
          .title = cmd->name,
          .icon = {.iconName = "folder"},
          .execute = std::bind(&RootView::loadExtension, this, *cmd)}};
    }

    setActions(actions);
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

  QWidget *createItemFromVariant(const RootItem &variant) override {
    if (auto app = std::get_if<AppItem>(&variant)) {
      return new ListItemWidget(
          ImageViewer::createFromModel(
              ThemeIconModel{.iconName = app->entry->iconName()}, {25, 25}),
          app->entry->name, "", "Application");
    }

    if (auto command = std::get_if<Extension::Command>(&variant)) {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = "folder"},
                                       {25, 25}),
          command->name, "Extension", "Command");
    }

    if (auto color = std::get_if<ColorItem>(&variant)) {
      auto circle = new ColorCircle(color->color.name(), QSize(60, 60));

      circle->setStroke("#BBB", 3);

      auto colorLabel = new QLabel(color->color.name());

      colorLabel->setProperty("class", "transform-left");

      auto left = new VStack(colorLabel, new Chip("HEX"));
      auto right = new VStack(circle, new Chip(color->color.name()));

      return new TransformResult(left, right);
    }

    if (auto result = std::get_if<CalculatorItem>(&variant)) {
      auto exprLabel = new QLabel(result->expression);

      exprLabel->setProperty("class", "transform-left");

      auto answerLabel = new QLabel(QString::number(result->result));
      answerLabel->setProperty("class", "transform-left");

      auto left = new VStack(exprLabel, new Chip("Expression"));
      auto right = new VStack(
          answerLabel,
          new Chip(result->unit ? QString(result->unit->displayName.data())
                                : "Answer"));

      return new TransformResult(left, right);
    }

    return nullptr;
  }

  RootView(AppWindow &app)
      : View(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()) {
    list->setModel(model);
    list->setItemDelegate(this);
    forwardInputEvents(list->listWidget());

    /*
connect(list, &NativeList::setActions, this,
        [this](const auto &actions) { actionModel->setItems(actions); });
    */

    connect(list, &NativeList::selectionChanged, this,
            [](const QVariant &variant) { qDebug() << "selection changed"; });

    widget = list;
  }

  ~RootView() { delete model; }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
