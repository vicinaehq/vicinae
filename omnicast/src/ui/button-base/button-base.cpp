#include "button-base.hpp"
#include <qcolor.h>
#include <qevent.h>
#include <qpainter.h>

bool ButtonBase::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::HoverEnter:
    hoverChanged(true);
    break;
  case QEvent::HoverLeave:
    hoverChanged(false);
    break;
  default:
    break;
  }

  return QWidget::event(event);
}

void ButtonBase::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked();
    return;
  }
  QWidget::mousePressEvent(event);
}

void ButtonBase::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (_bgColor.isValid()) {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(_bgColor));
    painter.drawRoundedRect(rect(), 5, 5);
  }

  QWidget::paintEvent(event);
}

void ButtonBase::setBackgroundColor(const QColor &color) { _bgColor = color; }

void ButtonBase::hoverChanged(bool hovered) { update(); }

ButtonBase::ButtonBase() : _hovered(false) { setAttribute(Qt::WA_Hover); }
