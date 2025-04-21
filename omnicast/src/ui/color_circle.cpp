#include "ui/color_circle.hpp"
#include "theme.hpp"
#include "ui/omni-painter.hpp"

ColorCircle::ColorCircle(const ColorLike &color, QSize size, QWidget *parent)
    : QWidget(parent), color(color), size(size), strokeWidth(0) {
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
    painter.setBrush(strokeColor);
    painter.drawEllipse(0, 0, w, h);
  }

  painter.setBrush(painter.colorBrush(color));
  painter.drawEllipse(strokeWidth, strokeWidth, w - diam, h - diam);
}

QSize ColorCircle::sizeHint() const { return size; }

ColorCircle &ColorCircle::setStroke(QColor color, size_t width) {
  strokeColor = color;
  strokeWidth = width;

  return *this;
}
