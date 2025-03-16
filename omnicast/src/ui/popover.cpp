#include "ui/popover.hpp"
#include "theme.hpp"
#include <qpainter.h>
#include <qpainterpath.h>

void Popover::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;
  QPainter painter(this);
  QPainterPath path;
  QPen pen(theme.colors.statusBackgroundBorder, 1);

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  painter.fillPath(path, theme.colors.statusBackground);
  painter.setPen(pen);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);
}
