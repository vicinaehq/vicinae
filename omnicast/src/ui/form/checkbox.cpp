#include "ui/form/checkbox.hpp"
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qevent.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qwidget.h>

void Checkbox::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  OmniPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(hasFocus() ? theme.colors.subtext : theme.colors.border);

  if (m_value) {
    painter.setBrush(painter.colorBrush(ColorTint::Blue));
  } else {
    painter.setBrush(Qt::transparent);
  }

  painter.drawRoundedRect(rect(), 4, 4);

  if (m_value) {
    auto check = rect().marginsRemoved(contentsMargins());

    m_svg->render(&painter, check);
  }
}

void Checkbox::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    toggle();
    return;
  }

  QWidget::keyPressEvent(event);
}

void Checkbox::mousePressEvent(QMouseEvent *event) {
  toggle();
  QWidget::mousePressEvent(event);
}

QSize Checkbox::sizeHint() const { return {20, 20}; }

QJsonValue Checkbox::asJsonValue() const { return value(); }

void Checkbox::setValueAsJson(const QJsonValue &value) { return setValue(value.toBool(false)); }

void Checkbox::toggle() { setValue(!m_value); }

void Checkbox::setValue(bool v) {
  if (v == m_value) return;

  m_value = v;
  update();
  emit valueChanged(v);
}

bool Checkbox::value() const { return m_value; }

Checkbox::Checkbox(QWidget *parent) : QWidget(parent) {
  setContentsMargins(1, 1, 1, 1);
  setFocusPolicy(Qt::StrongFocus);
  m_svg->load(QString(":icons/checkmark.svg"));
}
