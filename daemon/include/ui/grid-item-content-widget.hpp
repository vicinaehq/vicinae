#pragma once
#include "ui/tooltip.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class GridItemContentWidget : public QWidget {
  Q_OBJECT

  QVBoxLayout *layout;
  bool selected;
  bool hovered;
  Tooltip *tooltip;

protected:
  int borderWidth() const;
  QColor borderColor() const;
  void paintEvent(QPaintEvent *event) override;
  void enterEvent(QEnterEvent *event) override { setHovered(true); }
  void leaveEvent(QEvent *event) override { setHovered(false); }

  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

public:
  GridItemContentWidget();
  ~GridItemContentWidget();

  void setTooltipText(const QString &text);
  void showTooltip();
  void hideTooltip();
  void setHovered(bool hovered);
  void setSelected(bool selected);
  void setInset(int inset);
  void setWidget(QWidget *widget);

signals:
  void clicked();
  void doubleClicked();
};
