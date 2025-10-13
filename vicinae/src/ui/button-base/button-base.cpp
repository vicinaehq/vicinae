#include "button-base.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qpainterpath.h>

void ButtonBase::setColor(ButtonColor color) {
  switch (color) {
  case ButtonColor::Primary:
    setBackgroundColor(SemanticColor::SelectionBackground);
    setHoverBackgroundColor(SemanticColor::LighterBackground);
    break;
  case ButtonColor::Secondary:
    setBackgroundColor(SemanticColor::SelectionBackground);
    setHoverBackgroundColor(SemanticColor::LighterBackground);
    break;
  case ButtonColor::Transparent:
    setHoverBackgroundColor(SemanticColor::LighterBackground);
    break;
  }
}

void ButtonBase::mouseDoubleClickEvent(QMouseEvent *event) {
  if (m_disabled) return;

  emit doubleClicked();
  emit activated();
}

void ButtonBase::mousePressEvent(QMouseEvent *event) {
  if (m_disabled) return;

  emit clicked();
  emit activated();
}

void ButtonBase::setBackgroundColor(const ColorLike &color) {
  m_color = color;
  update();
}

void ButtonBase::setHoverBackgroundColor(const ColorLike &color) {
  m_hoverColor = color;
  update();
}

void ButtonBase::setFocused(bool value) {
  if (m_focused == value) return;

  m_focused = value;
  update();
}

void ButtonBase::setDisabled(bool disabled) {
  if (m_disabled == disabled) return;

  m_opacityEffect->setOpacity(disabled ? 0.6 : 1);
  setFocusPolicy(m_disabled ? Qt::NoFocus : Qt::TabFocus);
  m_disabled = disabled;
  update();
}

void ButtonBase::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    emit activated();
    return;
  }

  QWidget::keyPressEvent(event);
}

void ButtonBase::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 4;
  OmniPainter painter(this);
  QPainterPath path;
  QPen pen(theme.colors.text, 1);
  QBrush brush;

  if (underMouse() && !m_disabled) {
    brush = painter.colorBrush(m_hoverColor);
  } else {
    brush = painter.colorBrush(m_color);
  }

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  painter.setPen(m_focused ? pen : Qt::NoPen);
  painter.fillPath(path, brush);
  painter.drawPath(path);

  QWidget::paintEvent(event);
}

ButtonBase::ButtonBase(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_Hover);
  setFocusPolicy(Qt::FocusPolicy::TabFocus);
  setColor(Secondary);
  setGraphicsEffect(m_opacityEffect);
  m_opacityEffect->setOpacity(1);
}

bool ButtonBase::event(QEvent *event) {
  switch (event->type()) {
  case QEvent::HoverEnter:
    emit hoverChanged(true);
    break;
  case QEvent::HoverLeave:
    emit hoverChanged(false);
    break;
  default:
    break;
  }

  return QWidget::event(event);
}
