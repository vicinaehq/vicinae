#include "tooltip.hpp"
#include "ui/typography/typography.hpp"

bool TooltipWidget::eventFilter(QObject *watched, QEvent *event) {
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

void TooltipWidget::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  painter.fillPath(path, theme.colors.base00);

  // Draw the border
  QPen pen(theme.resolveTint(SemanticColor::Border), 1); // Border with a thickness of 2
  painter.setPen(pen);
  painter.drawPath(path);
}

QPoint TooltipWidget::calculatePosition(Qt::Alignment align) const {
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

void TooltipWidget::position() {
  auto pos = calculatePosition(m_alignment);
  move(pos);
}

void TooltipWidget::showEvent(QShowEvent *event) {
  if (!m_content) return;

  position();
  QWidget::showEvent(event);
}

void TooltipWidget::setWidget(QWidget *widget) {
  if (auto item = m_layout->itemAt(0)) {
    if (auto previous = item->widget()) {
      m_layout->replaceWidget(previous, widget);
      previous->deleteLater();
    }
  }

  m_content = widget;
}

void TooltipWidget::setText(const QString &s) {
  if (s.isEmpty()) {
    if (m_content) m_content->deleteLater();
    m_content = nullptr;
    return;
  }

  auto typography = new TypographyWidget();

  typography->setText(s);
  setWidget(typography);
}

void TooltipWidget::setTarget(QWidget *target) {
  if (m_target) { m_target->removeEventFilter(this); }

  m_target = target;
  m_target->setAttribute(Qt::WA_Hover);
  m_target->installEventFilter(this);
}

TooltipWidget::TooltipWidget(QWidget *parent) {
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

void TooltipWidget::setAlignment(Qt::Alignment align) {
  m_alignment = align;
  position();
}
