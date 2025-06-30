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
    int _inset;

    virtual QString title() const { return {}; }
    virtual QString subtitle() const { return {}; }
    virtual QString tooltip() const { return {}; }
    virtual QWidget *centerWidget() const = 0;

    void recycle(QWidget *base) const override final {
      auto widget = static_cast<GridItemWidget2 *>(base);

      widget->setTitle(title());
      widget->setSubtitle(subtitle());
      widget->setTooltipText(tooltip());
      widget->setAspectRatio(aspectRatio());

      if (centerWidgetRecyclable()) {
        recycleCenterWidget(widget->widget());
      } else {
        widget->setWidget(centerWidget());
      }
    }

    virtual bool centerWidgetRecyclable() const { return true; }

    virtual void recycleCenterWidget(QWidget *widget) const {}

    virtual void refreshCenterWidget(QWidget *widget) const {}

    virtual double aspectRatio() const { return 1; }

    bool recyclable() const override { return true; }

    int calculateHeight(int width) const final override {
      static GridItemWidget2 ruler;

      auto fm = ruler.fontMetrics();
      auto spacing = ruler.spacing();
      int height = width / aspectRatio();

      if (!title().isEmpty()) { height += fm.ascent() + spacing; }
      if (!subtitle().isEmpty()) { height += fm.ascent() + spacing; }

      return height;
    }

    OmniListItemWidget *createWidget() const final override {
      auto widget = new GridItemWidget2();

      widget->setAspectRatio(aspectRatio());
      widget->setInset(_inset);
      widget->setTitle(title());
      widget->setSubtitle(subtitle());
      widget->setTooltipText(tooltip());
      widget->setWidget(centerWidget());

      return widget;
    }

    void refresh(QWidget *widget) const override {
      auto itemWidget = static_cast<GridItemWidget2 *>(widget);

      itemWidget->setAspectRatio(aspectRatio());
      itemWidget->setInset(_inset);
      itemWidget->setTitle(title());
      itemWidget->setSubtitle(subtitle());
      itemWidget->setTooltipText(tooltip());
      refreshCenterWidget(itemWidget->widget());
    }

  public:
    void setInset(int inset) { _inset = inset; }

    AbstractGridItem() : _inset(10) {}
  };

public:
  int columns() const { return _ncols; }
  int inset() const { return _inset; }
  int spacing() const { return _spacing; }

  void setColumns(int n) { _ncols = n; }
  void setInset(int inset) { _inset = inset; }
  void setSpacing(int n) { _spacing = n; }

  OmniGrid() : _ncols(8), _spacing(10), _inset(10) { setMargins(20, 10, 20, 10); }
};
