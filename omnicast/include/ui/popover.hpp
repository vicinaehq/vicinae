#pragma once
#include <qwidget.h>

class Popover : public QWidget {
public:
  Popover(QWidget *parent = nullptr);

  void paintEvent(QPaintEvent *event) override;
};
