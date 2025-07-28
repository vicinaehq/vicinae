#pragma once
#include <qpainter.h>
#include <qstyleoption.h>
#include <qtmetamacros.h>
#include <QTimer>
#include <QPaintEvent>
#include <QScrollBar>

class OmniScrollBar : public QScrollBar {
  Q_OBJECT

  bool isSliderShown = false;
  bool isHovered = false;
  QTimer dismissTimer;

  void handleValueChanged(int value);
  void setSliderVisibility(bool visible);
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

public:
  OmniScrollBar(QWidget *parent = nullptr);
};
