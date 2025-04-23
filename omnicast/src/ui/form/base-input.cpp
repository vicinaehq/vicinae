#include <qevent.h>
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qpainter.h>
#include "ui/form/base-input.hpp"
#include "theme.hpp"

void BaseInput::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 4;
  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);
  QPen pen(hasFocus() ? theme.colors.subtext : theme.colors.border, 1);
  painter.setPen(pen);
  painter.drawRoundedRect(rect(), borderRadius, borderRadius);

  QLineEdit::paintEvent(event);
}

bool BaseInput::event(QEvent *event) {
  switch (event->type()) {
  case QFocusEvent::FocusIn:
    m_focusNotifier->focusChanged(true);
    break;
  case QFocusEvent::FocusOut:
    m_focusNotifier->focusChanged(false);
    break;
  default:
    break;
  }

  return QWidget::event(event);
}

void BaseInput::resizeEvent(QResizeEvent *event) {
  QLineEdit::resizeEvent(event);
  recalculate();
}

void BaseInput::recalculate() {
  if (!size().isValid()) return;

  QMargins margins{5, 0, 5, 0};

  if (leftAccessory) {
    leftAccessory->move(8, (height() - leftAccessory->height()) / 2);
    leftAccessory->show();
    margins.setLeft(leftAccessory->width() + 10);
  }

  if (rightAccessory) {
    rightAccessory->move(width() - rightAccessory->width() - 8, (height() - rightAccessory->height()) / 2);
    rightAccessory->show();
    margins.setRight(rightAccessory->width() + 10);
  }

  setTextMargins(margins);
}

void BaseInput::setValueAsJson(const QJsonValue &value) { setText(value.toString()); }

void BaseInput::setLeftAccessory(QWidget *widget) {
  leftAccessory = widget;
  leftAccessory->setFixedSize(18, 18);
  leftAccessory->setParent(this);
}

void BaseInput::setRightAccessory(QWidget *widget) {
  rightAccessory = widget;
  rightAccessory->setFixedSize(18, 18);
  rightAccessory->setParent(this);
}

QJsonValue BaseInput::asJsonValue() const { return text(); }

BaseInput::BaseInput(QWidget *parent) : QLineEdit(parent), leftAccessory(nullptr), rightAccessory(nullptr) {
  setContentsMargins(8, 8, 8, 8);
}
