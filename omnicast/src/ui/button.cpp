#include "ui/button.hpp"
#include <qcolor.h>
#include <qevent.h>
#include <qpainter.h>

bool Button::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::HoverEnter:
    hovered(true);
    break;
  case QEvent::HoverLeave:
    hovered(false);
    break;
  default:
    break;
  }

  return QWidget::event(event);
}

void Button::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) { emit clicked(); }
  QWidget::mousePressEvent(event);
}

void Button::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (_bgColor.isValid()) {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(_bgColor));
    painter.drawRoundedRect(rect(), 5, 5);
  }

  QWidget::paintEvent(event);
}

void Button::setBackgroundColor(const QColor &color) { _bgColor = color; }

void Button::hovered(bool hovered) { update(); }

Button::Button() : _hovered(false) { setAttribute(Qt::WA_Hover); }
