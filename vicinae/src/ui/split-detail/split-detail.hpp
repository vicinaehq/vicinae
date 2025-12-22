#pragma once
#include "ui/divider/divider.hpp"
#include <qwidget.h>

class SplitDetailWidget : public QWidget {
  bool m_detailVisible = false;
  QWidget *m_mainWidget = nullptr;
  QWidget *m_detailWidget = nullptr;
  VDivider *m_divider = new VDivider(this);
  double m_ratio = 0.65; // left takes up 70% of the space

protected:
  void relayout();
  void resizeEvent(QResizeEvent *event) override;

public:
  SplitDetailWidget(QWidget *parent = nullptr);
  SplitDetailWidget(QWidget *base, QWidget *detail, QWidget *parent = nullptr);

  double ratio() const;

  void setRatio(double ratio);
  void setDetailVisibility(bool value);

  void setMainWidget(QWidget *widget);
  void setDetailWidget(QWidget *widget);

  QWidget *mainWidget() const;
  QWidget *detailWidget() const;
  bool isDetailVisible() const;
};
