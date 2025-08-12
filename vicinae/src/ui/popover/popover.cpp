#include "ui/popover/popover.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpixmap.h>

void Popover::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;
  OmniPainter painter(this);
  QPainterPath path;
  QPen pen(theme.colors.border, 1);

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor finalColor = painter.resolveColor(SemanticColor::SecondaryBackground);

  finalColor.setAlphaF(0.98);
  painter.setPen(pen);
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);
}

void Popover::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }
