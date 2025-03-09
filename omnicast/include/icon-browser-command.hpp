#pragma once
#include "omni-icon.hpp"
#include "ui/omni-grid-view.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include "ui/virtual-list.hpp"
#include <qlabel.h>
#include <qnamespace.h>

class IconBrowserView : public OmniGridView {

  class IconBrowserItem : public OmniGrid::AbstractGridItem {
    QString m_name, m_displayName;

    QString tooltip() const override { return m_displayName; }

    QWidget *centerWidget() const override {
      auto icon = new OmniIcon;

      icon->setUrl(BuiltinOmniIconUrl(m_displayName));

      return icon;
    }

    QString id() const override { return m_name; }

  public:
    const QString &name() const { return m_name; }
    const QString &displayName() const { return m_displayName; }

    IconBrowserItem(const QString &name, const QString &displayName)
        : m_name(name), m_displayName(displayName) {}
  };

  class IconFilter : public OmniList::AbstractItemFilter {
    QString _query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      return static_cast<const IconBrowserItem &>(item).displayName().contains(_query, Qt::CaseInsensitive);
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
      auto ss = icon.split(".");
      ss = ss.at(0).split("/");

      auto displayName = ss.at(ss.size() - 1);

      grid->addItem(std::make_unique<IconBrowserItem>(icon, displayName));
    }

    QTimer::singleShot(0, [this]() {
      grid->commitUpdate();
      grid->selectFirst();
    });
  }

public:
  IconBrowserView(AppWindow &app) : OmniGridView(app) {}
  ~IconBrowserView() {}
};
