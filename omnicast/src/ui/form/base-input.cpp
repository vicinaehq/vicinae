#include <qboxlayout.h>
#include <qcoreevent.h>
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

  QWidget::paintEvent(event);
}

bool BaseInput::event(QEvent *event) { return QWidget::event(event); }

void BaseInput::resizeEvent(QResizeEvent *event) {
  JsonFormItemWidget::resizeEvent(event);
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

  m_input->setTextMargins(margins);
}

void BaseInput::setValueAsJson(const QJsonValue &value) { m_input->setText(value.toString()); }

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

QJsonValue BaseInput::asJsonValue() const { return m_input->text(); }

void BaseInput::clear() { m_input->clear(); };

void BaseInput::setText(const QString &text) { m_input->setText(text); };

QLineEdit *BaseInput::input() const { return m_input; }

void BaseInput::setPlaceholderText(const QString &text) { m_input->setPlaceholderText(text); }

void BaseInput::setReadOnly(bool value) { m_input->setReadOnly(value); }

bool BaseInput::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_input) {
    switch (event->type()) {
    case QEvent::FocusIn:
      m_focusNotifier->focusChanged(true);
      break;
    case QEvent::FocusOut:
      m_focusNotifier->focusChanged(false);
      break;
    default:
      break;
    }
  }

  return JsonFormItemWidget::eventFilter(watched, event);
}

BaseInput::BaseInput(QWidget *parent) : leftAccessory(nullptr), rightAccessory(nullptr) {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_input);
  m_input->installEventFilter(this);
  m_input->setContentsMargins(8, 8, 8, 8);
  setLayout(layout);
  setFocusProxy(m_input);
  connect(m_input, &QLineEdit::textChanged, this, &BaseInput::textChanged);
}
