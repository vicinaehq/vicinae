#pragma once
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class IconBrowserView : public GridView {
  class IconBrowserItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
    QString _name;

    QString tooltip() const override { return _name; }

    QString navigationTitle() const override { return _name; }

    QWidget *centerWidget() const override {
      auto icon = new OmniIcon;

      icon->setFixedSize(30, 30);
      icon->setUrl(BuiltinOmniIconUrl(_name));

      return icon;
    }

    QString generateId() const override { return _name; }

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

  void onSearchChanged(const QString &s) override { m_grid->setFilter(std::make_unique<IconFilter>(s)); }

public:
  IconBrowserView() {
    setSearchPlaceholderText("Search builtin icons...");
    m_grid->setColumns(8);
    m_grid->setInset(20);
    m_grid->beginUpdate();
    m_grid->addSection("Icons");

    for (const auto &icon : BuiltinIconService::icons()) {
      m_grid->addItem(std::make_unique<IconBrowserItem>(icon));
    }

    m_grid->commitUpdate();
    m_grid->selectFirst();
  }
  ~IconBrowserView() {}
};
