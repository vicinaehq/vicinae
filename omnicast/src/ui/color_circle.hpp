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
  ColorCircle(const ColorLike &color, QSize size, QWidget *parent = nullptr);

  ColorCircle &setStroke(QColor color, size_t width = 3);
};
