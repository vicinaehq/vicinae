#include "ui/color-circle/color_circle.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qwidget.h>

ColorCircle::ColorCircle(QSize size, QWidget *parent) : QWidget(parent), size(size), strokeWidth(0) {
  setFixedSize(size);
}

void ColorCircle::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);

  painter.setPen(Qt::NoPen);

  int w = width();
  int h = height();

  qreal diam = strokeWidth * 2;

  if (strokeWidth > 0) {
    painter.setThemeBrush(strokeColor);
    painter.drawEllipse(0, 0, w, h);
  }

  painter.setBrush(painter.colorBrush(color));
  painter.drawEllipse(strokeWidth, strokeWidth, w - diam, h - diam);
}

ColorCircle::ColorCircle(QWidget *parent) : QWidget(parent) {}

QSize ColorCircle::sizeHint() const { return size; }

ColorCircle &ColorCircle::setStroke(const ColorLike &color, size_t width) {
  strokeColor = color;
  strokeWidth = width;

  return *this;
}
