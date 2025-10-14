#include "ui/popover/popover.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpixmap.h>

void Popover::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  int borderRadius = 10;
  QPainterPath path;

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  QColor finalColor = painter.resolveColor(SemanticColor::PopoverBackground);
  // finalColor.setAlphaF(0.98);
  painter.setThemePen(SemanticColor::PopoverBorder, 3);
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
}
