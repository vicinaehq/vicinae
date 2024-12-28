#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "calculator.hpp"
#include "command.hpp"
#include "extend/action-model.hpp"
#include "extend/extension-command.hpp"
#include "extend/list-model.hpp"
#include "extension_manager.hpp"
#include "files-command.hpp"
#include "omnicast.hpp"
#include "ui/color_circle.hpp"
#include "ui/custom-list-view.hpp"
#include "ui/list-view.hpp"
#include "ui/native-list.hpp"
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
using ActionType =
    std::variant<AppAction, ExtensionAction, OpenHomeDirectoryAction>;

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

using RootItem = std::variant<AppItem, ColorItem, CalculatorItem>;

class RootListItemDelegate : public VariantNativeListItemDelegate<RootItem> {
  QWidget *createItemFromVariant(const RootItem &variant) override {
    if (auto app = std::get_if<AppItem>(&variant)) {
      return new ListItemWidget(
          ImageViewer::createFromModel(
              ThemeIconModel{.iconName = app->entry->iconName()}, {25, 25}),
          app->entry->name, "", "Application");
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
};

class RootView : public View {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  QHash<QString, std::variant<Extension::Command>> itemMap;
  QMimeDatabase mimeDb;

  VariantNativeListModel<RootItem> *model =
      new VariantNativeListModel<RootItem>();
  VariantNativeListItemDelegate<RootItem> *itemDelegate =
      new RootListItemDelegate();
  NativeList *list = new VariantNativeList<RootItem>();

public:
  void onSearchChanged(const QString &s) override {
    auto start = std::chrono::high_resolution_clock::now();

    auto textEditor = appDb.defaultTextEditor();
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
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    qDebug() << "root searched in " << duration << "ms";

    model->endReset();
  }

  RootView(AppWindow &app)
      : View(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()) {
    list->setModel(model);
    list->setItemDelegate(itemDelegate);

    widget = list;
  }

  ~RootView() {
    delete itemDelegate;
    delete model;
  }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
