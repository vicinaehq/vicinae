#pragma once

#include "grid-item-content-widget.hpp"
#include "grid-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <qwidget.h>

class OmniGrid : public OmniList {
  int _ncols;
  int _inset;
  int _spacing;

public:
  class AbstractGridItem : public AbstractVirtualItem {
    GridItemContentWidget::Inset m_inset;

    virtual QString title() const { return {}; }
    virtual QString subtitle() const { return {}; }
    virtual QString tooltip() const { return {}; }
    virtual QWidget *centerWidget() const = 0;

    void recycle(QWidget *base) const override final {
      auto widget = static_cast<GridItemWidget *>(base);

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

    size_t recyclingId() const override { return typeid(AbstractGridItem).hash_code(); }

    bool recyclable() const override { return true; }

    int calculateHeight(int width) const final override {
      static GridItemWidget ruler;

      auto fm = ruler.fontMetrics();
      auto spacing = 10;
      int height = width / aspectRatio();

      if (!title().isEmpty()) { height += 15 + spacing; }
      if (!subtitle().isEmpty()) { height += 15 + spacing; }

      return height;
    }

    OmniListItemWidget *createWidget() const final override {
      auto widget = new GridItemWidget();

      widget->setAspectRatio(aspectRatio());
      widget->setInset(m_inset);
      widget->setTitle(title());
      widget->setSubtitle(subtitle());
      widget->setTooltipText(tooltip());
      widget->setWidget(centerWidget());

      return widget;
    }

    void refresh(QWidget *widget) const override {
      auto itemWidget = static_cast<GridItemWidget *>(widget);

      itemWidget->setAspectRatio(aspectRatio());
      itemWidget->setInset(m_inset);
      itemWidget->setTitle(title());
      itemWidget->setSubtitle(subtitle());
      itemWidget->setTooltipText(tooltip());
      refreshCenterWidget(itemWidget->widget());
    }

  public:
    void setInset(GridItemContentWidget::Inset inset) { m_inset = inset; }

    AbstractGridItem() : m_inset(GridItemContentWidget::Inset::Small) {}
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
