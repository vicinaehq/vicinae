#include "button.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-painter.hpp"
#include <qpainterpath.h>

void OmniButtonWidget::setColor(ButtonColor color) {
  switch (color) {
  case ButtonColor::Primary:
    setBackgroundColor(SemanticColor::ButtonPrimary);
    setHoverBackgroundColor(SemanticColor::ButtonPrimaryHover);
    break;
  case ButtonColor::Secondary:
    setBackgroundColor(SemanticColor::ButtonSecondary);
    setHoverBackgroundColor(SemanticColor::ButtonSecondaryHover);
    break;
  case ButtonColor::Transparent:
    setHoverBackgroundColor(SemanticColor::ButtonSecondaryHover);
    break;
  }
}

void OmniButtonWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  emit doubleClicked();
  emit activated();
}

void OmniButtonWidget::mousePressEvent(QMouseEvent *event) {
  emit clicked();
  emit activated();
}

void OmniButtonWidget::setBackgroundColor(const ColorLike &color) {
  m_color = color;
  update();
}

void OmniButtonWidget::setHoverBackgroundColor(const ColorLike &color) {
  m_hoverColor = color;
  update();
}

void OmniButtonWidget::setFocused(bool value) {
  if (_focused == value) return;

  _focused = value;
  update();
}

void OmniButtonWidget::setLeftAccessory(QWidget *w) {
  if (auto leftItem = _layout->itemAt(0)) {
    _layout->replaceWidget(leftItem->widget(), w);
    leftItem->widget()->deleteLater();
  }
}

void OmniButtonWidget::setRightAccessory(QWidget *w) {
  if (auto rightItem = _layout->itemAt(2)) {
    _layout->replaceWidget(rightItem->widget(), w);
    rightItem->widget()->deleteLater();
  }
}

void OmniButtonWidget::setLeftIcon(const OmniIconUrl &url, QSize size) {
  auto icon = new Omnimg::ImageWidget(url);

  icon->setFixedSize(size);
  setLeftAccessory(icon);
}

void OmniButtonWidget::setColor(const ColorLike &color) { label->setColor(color); }

void OmniButtonWidget::setText(const QString &text) { label->setText(text); }

void OmniButtonWidget::setRightAccessory(const OmniIconUrl &url, QSize size) {
  auto icon = new Omnimg::ImageWidget(url);

  icon->setFixedSize(size);
  setRightAccessory(icon);
}

void OmniButtonWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    emit activated();
    return;
  }

  QWidget::keyPressEvent(event);
}

void OmniButtonWidget::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 4;
  OmniPainter painter(this);
  QPainterPath path;
  QPen pen(theme.colors.text, 1);
  QBrush brush;

  if (underMouse()) {
    brush = painter.colorBrush(m_hoverColor);
  } else {
    brush = painter.colorBrush(m_color);
  }

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  painter.setPen(_focused ? pen : Qt::NoPen);
  painter.fillPath(path, brush);
  painter.drawPath(path);
}

OmniButtonWidget::OmniButtonWidget(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_Hover);
  setFocusPolicy(Qt::StrongFocus);
  setColor(Secondary);

  _layout->setContentsMargins(5, 5, 5, 5);
  _layout->addWidget(leftAccessory);
  _layout->addWidget(label);
  _layout->addWidget(rightAccessory, 0, Qt::AlignRight);

  setLayout(_layout);
}
