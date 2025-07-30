#pragma once

#include "ui/omni-list/omni-list-item-widget.hpp"
#include <qevent.h>
#include <qpainter.h>
#include <qpainterpath.h>

class SelectableOmniListWidget : public OmniListItemWidget {
  bool isSelected;
  bool isHovered;

protected:
  void paintEvent(QPaintEvent *event) override;
  void selectionChanged(bool selected) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void setHovered(bool hovered);
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

public:
  bool selected() const;
  bool hovered() const;
  SelectableOmniListWidget(QWidget *parent = nullptr);
};
