#include "ui/tooltip.hpp"
#include "theme.hpp"
#include "ui/typography.hpp"

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

void Tooltip::setText(const QString &s) { m_label->setText(s); }

QString Tooltip::text() { return m_label->text(); }

Tooltip::Tooltip(QWidget *parent) : QWidget(parent), m_label(new TypographyWidget) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
  setAttribute(Qt::WA_TranslucentBackground);

  auto layout = new QVBoxLayout;

  layout->addWidget(m_label);
  setLayout(layout);
}
