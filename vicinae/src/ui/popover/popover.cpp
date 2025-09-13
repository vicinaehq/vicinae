#include "popover.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qpen.h>
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
  QPen pen(theme.resolveTint(SemanticColor::Border), 1);

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor finalColor = painter.resolveColor(SemanticColor::SecondaryBackground);
  painter.setPen(pen);
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
}

void Popover::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }
