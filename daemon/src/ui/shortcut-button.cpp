#include "ui/shortcut-button.hpp"
#include "ui/ellided-label.hpp"
#include <qcolor.h>

bool ShortcutButton::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::HoverEnter:
    setHovered(true);
    break;
  case QEvent::HoverLeave:
    setHovered(false);
    break;
  default:
    break;
  }

  return QWidget::event(event);
}

void ShortcutButton::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) { emit clicked(); }
  QWidget::mousePressEvent(event);
}

void ShortcutButton::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  qDebug() << "shortcut button" << size();

  if (_hovered) {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush("#222222"));
    painter.drawRoundedRect(rect(), 5, 5);
  }

  QWidget::paintEvent(event);
}

void ShortcutButton::setHovered(bool hovered) {
  if (hovered == _hovered) { return; }

  _shortcut_indicator->setBackgroundColor(hovered ? "#333333" : "#222222");
  _hovered = hovered;
  update();
}

void ShortcutButton::setText(const QString &text) {
  _label->setText(text);
  updateGeometry();
}

void ShortcutButton::setTextColor(const QColor &color) {
  _label->setStyleSheet(QString("color: %1").arg(color.name()));
  updateGeometry();
}

void ShortcutButton::setShortcut(const KeyboardShortcutModel &model) {
  _shortcut_indicator->setShortcut(model);
  updateGeometry();
}

ShortcutButton::ShortcutButton()
    : _label(new EllidedLabel), _shortcut_indicator(new KeyboardShortcutIndicatorWidget), _hovered(false) {
  setAttribute(Qt::WA_Hover);
  auto layout = new QHBoxLayout;

  layout->setAlignment(Qt::AlignVCenter);
  layout->addWidget(_label, 0, Qt::AlignLeft);
  layout->addWidget(_shortcut_indicator, 0, Qt::AlignRight);
  layout->setContentsMargins(8, 4, 8, 4);

  setLayout(layout);
}
