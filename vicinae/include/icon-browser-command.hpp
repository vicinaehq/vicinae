#pragma once
#include "actions/theme/theme-actions.hpp"
#include "builtin_icon.hpp"
#include "ui/views/base-view.hpp"
#include "../src/ui/image/url.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-grid/omni-grid.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <ranges>
#include "ui/views/grid-view.hpp"

class IconBrowserView : public GridView {
  class IconBrowserItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
    QString _name;

    QString tooltip() const override { return _name; }

    QString navigationTitle() const override { return _name; }

    QWidget *centerWidget() const override {
      auto icon = new ImageWidget;

      icon->setFixedSize(30, 30);
      icon->setUrl(ImageURL::builtin(_name));

      return icon;
    }

    QString generateId() const override { return _name; }

    QList<AbstractAction *> generateActions() const override {
      return {new SetThemeAction("omnicast-light")};
    }

    void recycleCenterWidget(QWidget *widget) const override {
      auto icon = static_cast<ImageWidget *>(widget);

      icon->setUrl(ImageURL::builtin(_name));
    }

    bool centerWidgetRecyclable() const override { return true; }

  public:
    const QString &name() const { return _name; }

    IconBrowserItem(const QString &name) : _name(name) {}
  };

  void textChanged(const QString &s) override {
    int inset = 20;
    auto filter = [&](const QString &name) { return name.contains(s, Qt::CaseInsensitive); };

    m_grid->updateModel([&]() {
      auto &section = m_grid->addSection("Icons");

      section.setColumns(8);
      section.setSpacing(10);
      m_grid->setInset(GridItemContentWidget::Inset::Small);

      for (const auto &icon : BuiltinIconService::icons() | std::views::filter(filter)) {
        auto item = std::make_unique<IconBrowserItem>(icon);

        item->setInset(GridItemContentWidget::Inset::Small);
        section.addItem(std::make_unique<IconBrowserItem>(icon));
      }
    });
  }

  void initialize() override { textChanged(searchText()); }

public:
  IconBrowserView() { setSearchPlaceholderText("Search builtin icons..."); }
  ~IconBrowserView() {}
};
