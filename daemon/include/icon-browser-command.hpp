#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "ui/grid-view.hpp"
#include "ui/virtual-grid.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class IconBrowserView : public GridView {

  class IconBrowserItem : public AbstractGridItem {
    QString name, displayName;

    QString tooltip() const override { return displayName; }

    QWidget *centerWidget() const override {
      auto iconLabel = new QLabel;

      iconLabel->setPixmap(QIcon::fromTheme(name).pixmap({32, 32}));

      return iconLabel;
    }

  public:
    IconBrowserItem(const QString &name, const QString &displayName) : name(name), displayName(displayName) {}
  };

  void onSearchChanged(const QString &s) override {
    VirtualGridSection section("Icons");

    for (const auto &icon : BuiltinIconService::icons()) {
      auto ss = icon.split(".");
      ss = ss.at(0).split("/");

      auto displayName = ss.at(ss.size() - 1);

      if (displayName.contains(s, Qt::CaseInsensitive)) {
        section.addItem(new IconBrowserItem(icon, displayName));
      }
    }

    grid->setSections({section});
  }

public:
  IconBrowserView(AppWindow &app) : GridView(app) { grid->setColumns(8); }
};
