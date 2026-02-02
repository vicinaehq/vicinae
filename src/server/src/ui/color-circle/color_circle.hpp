#pragma once
#include "theme.hpp"
#include <QPainter>
#include <QWidget>
#include <qwidget.h>

class ColorCircle : public QWidget {
  ColorLike color;
  QSize size;
  ColorLike strokeColor;
  size_t strokeWidth = 0;

protected:
  void paintEvent(QPaintEvent *event) override;

  QSize sizeHint() const override;

public:
  void setColor(const ColorLike &c) {
    color = c;
    update();
  }

  ColorCircle(QSize size, QWidget *parent = nullptr);
  ColorCircle(QWidget *parent = nullptr);

  ColorCircle &setStroke(const ColorLike &color, size_t width = 3);
};
