#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "command.hpp"
#include "extend/action-model.hpp"
#include "extend/extension-command.hpp"
#include "extend/list-model.hpp"
#include "extension_manager.hpp"
#include "files-command.hpp"
#include "ui/custom-list-view.hpp"
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

class RootView : public CustomList<ActionType> {
  AppWindow &app;
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  QHash<QString, std::variant<Extension::Command>> itemMap;
  QMimeDatabase mimeDb;

public:
  ListModel search(const QString &s) override {
    auto start = std::chrono::high_resolution_clock::now();

    ListModel model;
    ListSectionModel results{.title = "Results"};

    model.searchPlaceholderText = "Search for apps or commands...";

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
              .shortcut = KeyboardShortcutModel{.key = "return"}};

          addAction(openAction.onAction, OpenAppAction{app});
          panelModel.children.push_back(openAction);

          if (fileBrowser) {
            ActionModel openInFileBrowser{
                .title = QString("Open in %1").arg(fileBrowser->name),
                .onAction = app->id + ".open." + fileBrowser->id,
                .icon = ThemeIconModel{.iconName = fileBrowser->iconName()},
                .shortcut = KeyboardShortcutModel{.key = "return",
                                                  .modifiers = {"cmd"}}};

            addAction(openInFileBrowser.onAction,
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
    }

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
                               .icon = item.icon,
                               .shortcut =
                                   KeyboardShortcutModel{.key = "return"}};

        panelModel.children.push_back(loadAction);
        addAction(loadAction.onAction, ExtensionLoadAction{cmd});
        item.actionPannel = panelModel;

        results.children.push_back(item);
      }
    }

    ListItemViewModel item;

    item.id = "home-explorer";
    item.title = "Home directory";
    item.icon = ThemeIconModel{.iconName = "folder"};
    item.actionPannel = ActionPannelModel{
        .children = {ActionModel{.title = "Open",
                                 .onAction = "open-home",
                                 .icon = item.icon,
                                 .shortcut = KeyboardShortcutModel{
                                     .key = "return", .modifiers = {}}}}};

    addAction("open-home", OpenHomeDirectoryAction{});

    results.children.push_back(item);

    model.items.push_back(results);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    qDebug() << "root searched in " << duration << "ms";

    return model;
  }

  void action(const ActionType &action) override {
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

    if (auto homeAction = std::get_if<OpenHomeDirectoryAction>(&action)) {
      emit pushView(new FilesView(app));
    }

    qDebug() << "action activated in root command";
  }

  RootView(AppWindow &app)
      : CustomList(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()) {}
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
