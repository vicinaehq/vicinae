#include "ui/tooltip.hpp"
#include "theme.hpp"
#include "ui/typography.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>

bool Tooltip::eventFilter(QObject *watched, QEvent *event) {
  if (watched != m_target) return QWidget::eventFilter(watched, event);

  if (event->type() == QEvent::HoverEnter) {
    if (m_target->isVisible()) { show(); }
  }
  if (event->type() == QEvent::HoverLeave) { hide(); }
  if (event->type() == QEvent::Hide) { hide(); }
  if (event->type() == QEvent::HideToParent) { hide(); }
  if (event->type() == QEvent::Resize) {
    if (isVisible()) position();
  }
  if (event->type() == QEvent::Move) {
    if (isVisible()) position();
  }

  return QWidget::eventFilter(watched, event);
}

void Tooltip::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  painter.fillPath(path, theme.colors.mainBackground);

  // Draw the border
  QPen pen(theme.colors.border, 1); // Border with a thickness of 2
  painter.setPen(pen);
  painter.drawPath(path);
}

QPoint Tooltip::calculatePosition(Qt::Alignment align) const {
  if (!m_target) { return {}; }

  QPoint baseTargetPos;
  int yOffset = 0;

  if (align.testFlag(Qt::AlignTop)) {
    baseTargetPos = m_target->geometry().topLeft();
    yOffset = -10;
  } else if (align.testFlag(Qt::AlignBottom)) {
    baseTargetPos = m_target->geometry().bottomLeft();
    yOffset = 10;
  }

  auto pos = m_target->mapToGlobal(baseTargetPos);
  auto ypos = pos.y() + yOffset;

  int gap = width() - m_target->width();
  auto xpos = pos.x() - gap / 2;

  return {xpos, ypos};
}

void Tooltip::position() {
  auto pos = calculatePosition(m_alignment);
  move(pos);
}

void Tooltip::showEvent(QShowEvent *event) {
  position();
  QWidget::showEvent(event);
}

void Tooltip::setWidget(QWidget *widget) {
  if (auto item = m_layout->itemAt(0)) {
    if (auto previous = item->widget()) {
      m_layout->replaceWidget(previous, widget);
      previous->deleteLater();
    }
  }
}

void Tooltip::setText(const QString &s) {
  auto typography = new TypographyWidget();

  typography->setText(s);
  setWidget(typography);
}

void Tooltip::setTarget(QWidget *target) {
  if (m_target) { m_target->removeEventFilter(this); }

  m_target = target;
  m_target->setAttribute(Qt::WA_Hover);
  m_target->installEventFilter(this);
}

Tooltip::Tooltip(QWidget *parent) {
  if (parent) setParent(parent->window());
  setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setAttribute(Qt::WA_X11DoNotAcceptFocus); // X11 specific focus prevention
  setAttribute(Qt::WA_AlwaysStackOnTop);    // Stay on top without stealing focus
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  m_layout->setContentsMargins(5, 5, 5, 5);
  m_layout->setSpacing(0);
  m_layout->addWidget(new QWidget);
  setLayout(m_layout);
}

void Tooltip::setAlignment(Qt::Alignment align) {
  m_alignment = align;
  position();
}
