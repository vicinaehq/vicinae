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
public:
  enum Inset { Small, Medium, Large };

private:
  Q_OBJECT
  bool m_selected;
  Inset m_inset;
  Tooltip *m_tooltip;
  QWidget *m_widget;

protected:
  int borderWidth() const;
  void paintEvent(QPaintEvent *event) override;
  void hideEvent(QHideEvent *event) override;

  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  QSize innerWidgetSize() const;
  int insetForSize(Inset inset, QSize size) const;
  void repositionCenterWidget();

public:
  GridItemContentWidget();
  virtual ~GridItemContentWidget() override;

  void setTooltipText(const QString &text);
  void showTooltip();
  void hideTooltip();
  void setSelected(bool selected);
  void setInset(Inset inset);

  void setWidget(QWidget *widget);
  QWidget *widget() const;

signals:
  void clicked();
  void doubleClicked();
};
