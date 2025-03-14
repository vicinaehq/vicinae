#pragma once
#include "omni-icon.hpp"
#include "ui/omni-grid-view.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class IconBrowserView : public OmniGridView {
  class IconBrowserItem : public OmniGrid::AbstractGridItem {
    QString _name;

    QString tooltip() const override { return _name; }

    QWidget *centerWidget() const override {
      auto icon = new OmniIcon;

      icon->setFixedSize(30, 30);
      icon->setUrl(BuiltinOmniIconUrl(_name));

      return icon;
    }

    QString id() const override { return _name; }

  public:
    const QString &name() const { return _name; }

    IconBrowserItem(const QString &name) : _name(name) {}
  };

  class IconFilter : public OmniList::AbstractItemFilter {
    QString _query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      return static_cast<const IconBrowserItem &>(item).name().contains(_query, Qt::CaseInsensitive);
    }

  public:
    IconFilter(const QString &query) : _query(query) {}
  };

  void onSearchChanged(const QString &s) override { grid->setFilter(std::make_unique<IconFilter>(s)); }

  void onMount() override {
    grid->setColumns(10);
    grid->setInset(20);
    grid->beginUpdate();
    grid->addSection("Icons");

    for (const auto &icon : BuiltinIconService::icons()) {
      grid->addItem(std::make_unique<IconBrowserItem>(icon));
    }

    grid->commitUpdate();
    grid->selectFirst();
  }

public:
  IconBrowserView(AppWindow &app) : OmniGridView(app) {}
  ~IconBrowserView() {}
};
