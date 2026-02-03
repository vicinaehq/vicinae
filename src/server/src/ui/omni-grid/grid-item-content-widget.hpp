#pragma once
#include "ui/tooltip/tooltip.hpp"
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

signals:
  void clicked();
  void doubleClicked();

public:
  enum class Inset { None, Small, Medium, Large };

  GridItemContentWidget();
  virtual ~GridItemContentWidget() override;

  void setTooltipText(const QString &text);
  void showTooltip();
  void hideTooltip();
  void setSelected(bool selected);
  void setInset(Inset inset);

  void setWidget(QWidget *widget);
  QWidget *widget() const;

protected:
  int borderWidth() const;
  void paintEvent(QPaintEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  // bool event(QEvent *event) override;

  bool event(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  QSize innerWidgetSize() const;
  int insetForSize(Inset inset, QSize size) const;
  void repositionCenterWidget();
  void recalculate();

private:
  bool m_selected;
  Inset m_inset = Inset::None;
  TooltipWidget *m_tooltip;
  QWidget *m_widget;
};
