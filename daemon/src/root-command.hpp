#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "command.hpp"
#include "extend/action-model.hpp"
#include "extend/extension-command.hpp"
#include "extension_manager.hpp"
#include "ui/list-view.hpp"
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

using ExtensionAction = std::variant<ExtensionLoadAction>;
using ActionType = std::variant<AppAction, ExtensionAction>;

class RootView : public View {
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  ListView *list;
  AppWindow &app;
  QHash<QString, std::variant<Extension::Command>> itemMap;
  QHash<QString, ActionType> actionMap;
  QMimeDatabase mimeDb;

public:
  virtual void onSearchChanged(const QString &s) override {
    auto start = std::chrono::high_resolution_clock::now();

    ListModel model;
    ListSectionModel results{.title = "Results"};

    auto textEditor = appDb.defaultTextEditor();
    auto fileBrowser = appDb.defaultFileBrowser();

    itemMap.clear();

    if (!s.isEmpty()) {
      for (auto &app : appDb.apps) {
        if (!app->displayable())
          continue;

        for (const auto &word : app->name.split(" ")) {
          if (!word.startsWith(s, Qt::CaseInsensitive))
            continue;

          ListItemViewModel item;

          item.id = app->id;
          item.title = app->name;
          item.icon = ThemeIconModel{.iconName = app->iconName()};

          ActionPannelModel panelModel;

          ActionModel openAction{
              .title = "Open application",
              .onAction = app->id + ".open",
              .icon = ThemeIconModel{.iconName = app->iconName()},
          };

          actionMap.insert(openAction.onAction, OpenAppAction{app});
          panelModel.children.push_back(openAction);

          if (fileBrowser) {
            ActionModel openInFileBrowser{
                .title = QString("Open in %1").arg(fileBrowser->name),
                .onAction = app->id + ".open." + fileBrowser->id,
                .icon = ThemeIconModel{.iconName = fileBrowser->iconName()}};

            actionMap.insert(openInFileBrowser.onAction,
                             OpenAppInFileBrowserAction{app, fileBrowser});
            panelModel.children.push_back(openInFileBrowser);
          }

          if (textEditor) {
            ActionModel openDesktopFile{
                .title = "Open desktop file",
                .icon = ThemeIconModel{.iconName = textEditor->iconName()}};

            panelModel.children.push_back(openDesktopFile);
          }

          item.actionPannel = panelModel;

          results.children.push_back(item);
          break;
        }
      }

      model.items.push_back(results);
    }

    ListSectionModel extensionSection{.title = "Extensions"};

    for (const auto &extension : extensionManager.extensions()) {

      for (const auto &cmd : extension.commands) {
        if (!cmd.name.contains(s, Qt::CaseInsensitive))
          continue;

        ListItemViewModel item;

        item.id = cmd.name;
        item.title = cmd.name;
        item.subtitle = cmd.subtitle;
        item.icon = ThemeIconModel{.iconName = "chromium"};

        itemMap.insert(item.id, cmd);

        ActionPannelModel panelModel;
        ActionModel loadAction{.title = "Load command",
                               .onAction = item.id + ".load",
                               .icon = item.icon};

        panelModel.children.push_back(loadAction);
        actionMap.insert(loadAction.onAction, ExtensionLoadAction{cmd});
        item.actionPannel = panelModel;

        extensionSection.children.push_back(item);
      }
    }

    model.items.push_back(extensionSection);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    qDebug() << "root searched in " << duration << "ms";

    list->dispatchModel(model);
  }

  RootView(AppWindow &app)
      : View(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()), list(new ListView) {
    forwardInputEvents(list->listWidget());

    connect(list, &ListView::itemChanged, this, &RootView::itemChanged);
    connect(list, &ListView::itemActivated, this, &RootView::itemActivated);
    connect(list, &ListView::setActions, this, &RootView::setActions);

    widget = list;
  }

private slots:
  void itemChanged(const QString &id) {}

  void itemActivated(const QString &id) {}

  void onActionActivated(ActionModel model) override {
    const auto &action = actionMap.value(model.onAction);

    if (auto appAction = std::get_if<AppAction>(&action)) {
      if (auto openAppAction = std::get_if<OpenAppAction>(appAction)) {
        qDebug() << "launch app";
        openAppAction->app->launch();
      }
      if (auto openAppAction =
              std::get_if<OpenAppInFileBrowserAction>(appAction)) {
        qDebug() << "launch in filebrowser";
        openAppAction->fileBrowser->launch({openAppAction->app->path});
      }
    }

    if (auto extAction = std::get_if<ExtensionAction>(&action)) {
      if (auto loadCmdAction = std::get_if<ExtensionLoadAction>(extAction)) {
        auto cmd = &loadCmdAction->command;

        emit launchCommand(
            new ExtensionCommand(app, cmd->extensionId, cmd->name));
      }
    }

    qDebug() << "action activated in root command";
  }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
