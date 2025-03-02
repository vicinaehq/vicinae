#pragma once

#include <qboxlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpainterpath.h>

class Tooltip : public QWidget {
  QLabel *label;

  void paintEvent(QPaintEvent *event) override;

public:
  Tooltip(QWidget *parent = nullptr);
  void setText(const QString &s);
  QString text();
};
