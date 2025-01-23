#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "navigation-list-view.hpp"
#include <qnamespace.h>

class IconBrowserView : public NavigationListView {
  class IconBrowserItem : public StandardListItem {

  public:
    IconBrowserItem(const QString &name, const QString &displayName)
        : StandardListItem(displayName, "", "", ThemeIconModel{.iconName = name}) {}
  };

  void onSearchChanged(const QString &s) override {
    model->beginReset();
    for (const auto &icon : BuiltinIconService::icons()) {
      auto ss = icon.split(".");
      ss = ss.at(0).split("/");

      auto displayName = ss.at(ss.size() - 1);

      if (displayName.contains(s, Qt::CaseInsensitive)) {
        model->addItem(std::make_shared<IconBrowserItem>(icon, displayName));
      }
    }
    model->endReset();
  }

public:
  IconBrowserView(AppWindow &app) : NavigationListView(app) {}
};
