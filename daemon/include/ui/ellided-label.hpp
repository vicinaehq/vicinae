#pragma once
#include <qlabel.h>
#include <qpainter.h>

class EllidedLabel : public QLabel {
  void paintEvent(QPaintEvent *event) override;

public:
  EllidedLabel();
};
