#pragma once

#include "ui/grid-item-widget.hpp"
#include "ui/omni-list.hpp"
#include <qwidget.h>

class OmniGrid : public OmniList {
  int _ncols;
  int _inset;
  int _spacing;

public:
  class AbstractGridItem : public AbstractVirtualItem {
    virtual QString title() const { return {}; }
    virtual QString subtitle() const { return {}; }
    virtual QString tooltip() const { return {}; }
    virtual QWidget *centerWidget() const = 0;

    void recycle(QWidget *base) const override {
      auto widget = static_cast<GridItemWidget2 *>(base);

      widget->setTitle(title());
      widget->setSubtitle(subtitle());
      widget->setTooltipText(tooltip());

      if (centerWidgetRecyclable()) {
        recycleCenterWidget(widget->widget());
      } else {
        widget->setWidget(centerWidget());
      }
    }

    virtual bool centerWidgetRecyclable() const { return false; }

    virtual void recycleCenterWidget(QWidget *widget) const {}

    bool recyclable() const override { return true; }

    int calculateHeight(int width) const override {
      static GridItemWidget2 ruler;

      auto fm = ruler.fontMetrics();
      auto spacing = ruler.spacing();
      int height = width;

      if (!title().isEmpty()) { height += fm.ascent() + spacing; }
      if (!subtitle().isEmpty()) { height += fm.ascent() + spacing; }

      return height;
    }

    OmniListItemWidget *createWidget() const override {
      auto widget = new GridItemWidget2();

      widget->setTitle(title());
      widget->setSubtitle(subtitle());
      widget->setTooltipText(tooltip());
      widget->setWidget(centerWidget());

      return widget;
    }
  };

  class GridSection : public OmniList::VirtualSection {
    int _columns;
    int _spacing;

    int spaceWidth() const { return _spacing * (_columns - 1); }

    virtual int spacing() const override { return _spacing; }

    int calculateItemWidth(int width, int index) const override {
      int availableWidth = width - spaceWidth();

      return availableWidth / _columns;
    }

    int calculateItemX(int x, int index) const override {
      if (index % _columns) { return x + _spacing; }

      return x;
    }

  public:
    GridSection(const QString &name, int columns, int spacing)
        : VirtualSection(name), _columns(columns), _spacing(spacing) {}
  };

  void recalculateFull() { invalidateCache(); }

public:
  int columns() const { return _ncols; }
  int inset() const { return _inset; }
  int spacing() const { return _spacing; }

  void setColumns(int n) { _ncols = n; }
  void setInset(int inset) { _inset = inset; }
  void setSpacing(int n) { _spacing = n; }

  void addSection(const QString &name) {
    OmniList::addItem(std::make_unique<GridSection>(name, _ncols, _spacing));
  }

  void addItem(std::unique_ptr<AbstractGridItem> item) { OmniList::addItem(std::move(item)); }

  OmniGrid() : _ncols(8), _spacing(10), _inset(10) { setMargins(20, 10, 20, 10); }
};
