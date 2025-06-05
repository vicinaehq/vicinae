#pragma once
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <ranges>

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

  void onSearchChanged(const QString &s) override {
    qCritical() << "onSearchChanged" << s;

    int inset = 20;
    auto filter = [&](const QString &name) { return name.contains(s, Qt::CaseInsensitive); };
    auto makeIcon = [&](auto &&icon) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
      auto item = std::make_unique<IconBrowserItem>(icon);

      item->setInset(inset);
      return item;
    };

    m_grid->updateModel([&]() {
      auto &section = m_grid->addSection("Icons");

      section.setColumns(8);
      section.setSpacing(10);
      m_grid->setInset(20);

      auto items = BuiltinIconService::icons() | std::views::filter(filter) |
                   std::views::transform(makeIcon) | std::ranges::to<std::vector>();

      section.addItems(std::move(items));
    });
  }

  void initialize() override {}

public:
  IconBrowserView() { setSearchPlaceholderText("Search builtin icons..."); }
  ~IconBrowserView() {}
};
