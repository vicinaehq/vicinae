#include "color-box.hpp"
#include "ui/omni-painter/omni-painter.hpp"

void ColorBox::setColor(const ColorLike &color) {
  m_color = color;
  update();
}

void ColorBox::setBorderRadius(int radius) {
  m_radius = radius;
  update();
}

void ColorBox::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  QPainterPath path;

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setThemeBrush(m_color);
  path.addRoundedRect(rect(), m_radius, m_radius);
  painter.drawPath(path);
}

QSize ColorBox::sizeHint() const {
  if (auto w = parentWidget()) { return w->size(); }
  return {10, 10};
}
