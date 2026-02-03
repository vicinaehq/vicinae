#pragma once
#include "ui/omni-list/omni-list-item-widget.hpp"
#include <qevent.h>
#include <qpainter.h>
#include <qpainterpath.h>

class SelectableOmniListWidget : public OmniListItemWidget {
public:
  SelectableOmniListWidget(QWidget *parent = nullptr);
  bool selected() const;
  bool hovered() const;
  void selectionChanged(bool selected) override;

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void setHovered(bool hovered);
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void clearTransientState() override { isHovered = false; }

private:
  bool isSelected;
  bool isHovered;
};
