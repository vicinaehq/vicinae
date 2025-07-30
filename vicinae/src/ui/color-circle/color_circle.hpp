#pragma once
#include "theme.hpp"
#include <QPainter>
#include <QWidget>

class ColorCircle : public QWidget {
  ColorLike color;
  QSize size;
  QColor strokeColor;
  size_t strokeWidth;

protected:
  void paintEvent(QPaintEvent *event) override;

  QSize sizeHint() const override;

public:
  void setColor(const ColorLike &c) {
    color = c;
    update();
  }

  ColorCircle(QSize size, QWidget *parent = nullptr);

  ColorCircle &setStroke(QColor color, size_t width = 3);
};
