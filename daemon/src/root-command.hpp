#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "command.hpp"
#include "extend/extension-command.hpp"
#include "extension_manager.hpp"
#include "omnicast.hpp"
#include <qlabel.h>
#include <qlistwidget.h>
#include <qmap.h>
#include <qnamespace.h>
#include <qwidget.h>

class RootView : public View {
  Service<AppDatabase> appDb;
  Service<ExtensionManager> extensionManager;
  QListWidget *list;
  QMap<QListWidgetItem *, Extension::Command> itemToCommand;
  AppWindow &app;

public:
  virtual void onSearchChanged(const QString &s) override {
    qDebug() << "search changed " << s;

    list->clear();
    itemToCommand.clear();

    for (const auto &extension : extensionManager.extensions()) {
      for (const auto &cmd : extension.commands) {

        auto widget = new GenericListItem(QIcon::fromTheme("chrome"), cmd.name,
                                          "", "Command");
        auto item = new QListWidgetItem();

        itemToCommand.insert(item, cmd);

        list->addItem(item);
        list->setItemWidget(item, widget);
        item->setSizeHint(widget->sizeHint());
      }
    }

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

  RootView(AppWindow &app)
      : View(app), app(app), appDb(service<AppDatabase>()),
        extensionManager(service<ExtensionManager>()), list(new QListWidget) {
    forwardInputEvents(list);
    list->setFocusPolicy(Qt::NoFocus);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(list, &QListWidget::currentItemChanged, this,
            &RootView::currentItemChanged);
    connect(list, &QListWidget::itemActivated, this, &RootView::itemActivate);
    connect(&extensionManager, &ExtensionManager::commandLoaded, this,
            &RootView::commandLoaded);

    widget = list;
  }

private slots:
  void commandLoaded(const LoadedCommand &command) {
    qDebug() << "command loaded" << command.sessionId << command.command.name;
  }

  void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
  }

  void itemActivate(QListWidgetItem *item) {
    auto cmd = itemToCommand[item];

    emit launchCommand(new ExtensionCommand(app, cmd.extensionId, cmd.name));
  }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
