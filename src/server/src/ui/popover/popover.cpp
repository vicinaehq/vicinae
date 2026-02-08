#include "popover.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qpainterpath.h>

void Popover::paintEvent(QPaintEvent *event) {
  const auto &config = ServiceRegistry::instance()->config()->value();
  OmniPainter painter(this);
  int borderRadius = 10;
  QPainterPath path;

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  QColor finalColor = OmniPainter::resolveColor(SemanticColor::PopoverBackground);

  finalColor.setAlphaF(config.launcherWindow.opacity);
  painter.setThemePen(SemanticColor::PopoverBorder, 3);
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);
}
