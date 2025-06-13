#pragma once
#include "ui/tooltip.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class GridItemContentWidget : public QWidget {
  Q_OBJECT

  bool selected;
  bool hovered;
  int _inset;
  Tooltip *tooltip;
  QWidget *_widget;

protected:
  int borderWidth() const;
  void paintEvent(QPaintEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void moveEvent(QMoveEvent *event) override {
    QWidget::moveEvent(event);
    // reposition the tooltip relative to the widget
    if (tooltip->isVisible()) { showTooltip(); }
  }
  bool event(QEvent *event) override;

  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  QSize innerWidgetSize() const;

public:
  GridItemContentWidget();
  virtual ~GridItemContentWidget() override;

  void setTooltipText(const QString &text);
  void showTooltip();
  void hideTooltip();
  void setHovered(bool hovered);
  void setSelected(bool selected);
  void setInset(int inset);

  void setWidget(QWidget *widget);
  QWidget *widget() const;

signals:
  void clicked();
  void doubleClicked();
};
