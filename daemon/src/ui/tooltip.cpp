#include "ui/tooltip.hpp"

void Tooltip::paintEvent(QPaintEvent *event) {
  int borderRadius = 10;
  QColor borderColor("#444444");

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor backgroundColor("#171615");

  painter.fillPath(path, backgroundColor);

  // Draw the border
  QPen pen(borderColor, 1); // Border with a thickness of 2
  painter.setPen(pen);
  painter.drawPath(path);
}

void Tooltip::setText(const QString &s) { label->setText(s); }

QString Tooltip::text() { return label->text(); }

Tooltip::Tooltip(QWidget *parent) : QWidget(parent), label(new QLabel) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
  setAttribute(Qt::WA_TranslucentBackground);

  auto layout = new QVBoxLayout;

  layout->addWidget(label);
  setLayout(layout);
}
