#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "grid-view.hpp"
#include "ui/virtual-grid.hpp"
#include <qnamespace.h>

class IconBrowserView : public GridView {
  /*
class IconBrowserItem : public AbstractIconGridItem {
QString name, displayName;

QString iconName() const override { return name; }
QString tooltip() const override { return displayName; }

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
*/

public:
  IconBrowserView(AppWindow &app) : GridView(app) { grid->setColumns(8); }
};
