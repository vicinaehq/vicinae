#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "ui/grid-view.hpp"
#include "ui/virtual-grid.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class IconBrowserView : public GridView {

  class IconBrowserItem : public AbstractGridItem {
    QString m_name, m_displayName;

    QString tooltip() const override { return m_displayName; }

    QWidget *centerWidget() const override {
      auto iconLabel = new QLabel;

      iconLabel->setPixmap(QIcon::fromTheme(m_name).pixmap({32, 32}));

      return iconLabel;
    }

    int key() const override { return qHash(m_name); }

  public:
    const QString &name() { return m_name; }
    const QString &displayName() { return m_displayName; }

    IconBrowserItem(const QString &name, const QString &displayName)
        : m_name(name), m_displayName(displayName) {}
  };

  std::vector<IconBrowserItem *> items;

  void onSearchChanged(const QString &s) override {
    grid->clear();

    auto icons = grid->section("Icons");

    for (const auto &item : items) {
      if (item->displayName().contains(s, Qt::CaseInsensitive)) { icons->addItem(item); }
    }

    grid->calculateLayout();
    grid->setSelected(0);
  }

  void onMount() override {
    for (const auto &icon : BuiltinIconService::icons()) {
      auto ss = icon.split(".");
      ss = ss.at(0).split("/");

      auto displayName = ss.at(ss.size() - 1);

      items.push_back(new IconBrowserItem(icon, displayName));
    }
  }

public:
  IconBrowserView(AppWindow &app) : GridView(app) { grid->setColumns(8); }
  ~IconBrowserView() {
    for (const auto &item : items) {
      item->deleteLater();
    }
  }
};
