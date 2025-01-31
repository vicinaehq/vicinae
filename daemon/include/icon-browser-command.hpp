#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "ui/virtual-grid.hpp"
#include "view.hpp"
#include <qnamespace.h>

class IconBrowserView : public View {
  VirtualGridWidget *grid;

  class IconBrowserItem : public AbstractGridItem {
    QString name, displayName;

    QString iconName() const override { return name; }
    QString title() const override { return displayName; };

  public:
    IconBrowserItem(const QString &name, const QString &displayName) : name(name), displayName(displayName) {}
  };

  void onSearchChanged(const QString &s) override {
    QList<AbstractGridItem *> items;

    for (const auto &icon : BuiltinIconService::icons()) {
      auto ss = icon.split(".");
      ss = ss.at(0).split("/");

      auto displayName = ss.at(ss.size() - 1);

      if (displayName.contains(s, Qt::CaseInsensitive)) { items << new IconBrowserItem(icon, displayName); }
    }

    grid->setItems(items);
  }

public:
  IconBrowserView(AppWindow &app) : View(app), grid(new VirtualGridWidget) {
    grid->setColumns(8);
    widget = grid;
  }
};
