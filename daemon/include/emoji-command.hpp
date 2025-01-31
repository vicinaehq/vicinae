#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "ui/virtual-grid.hpp"
#include "view.hpp"
#include <qnamespace.h>

class AppGridItem : public AbstractGridItem {
  std::shared_ptr<DesktopExecutable> app;

public:
  QString title() const override { return app->name; }
  QString iconName() const override { return app->iconName(); }

  AppGridItem(const std::shared_ptr<DesktopExecutable> &app) : app(app) {}
};

class EmojiView : public View {
  Service<AppDatabase> appDb;
  VirtualGridWidget *grid;

public:
  EmojiView(AppWindow &app) : View(app), appDb(service<AppDatabase>()), grid(new VirtualGridWidget) {
    widget = grid;
  }

  void onSearchChanged(const QString &s) override {
    QList<AbstractGridItem *> items;

    for (const auto &app : appDb.apps) {
      if (app->name.contains(s, Qt::CaseInsensitive)) { items.push_back(new AppGridItem(app)); }
    }

    grid->setItems(items);
  }
};
