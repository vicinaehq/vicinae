#pragma once
#include <qlabel.h>
#include <qpainter.h>
#include <qwidget.h>

class EllidedLabel : public QLabel {
  void paintEvent(QPaintEvent *event) override;

public:
  EllidedLabel(const QString &text = "", QWidget *parent = nullptr);
};
