#include "popover.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qpainterpath.h>

void Popover::paintEvent(QPaintEvent *event) {
  const auto &config = ServiceRegistry::instance()->config()->value();
  QPainter painter(this);
  QPainterPath path;
  QColor finalColor = OmniPainter::resolveColor(SemanticColor::PopoverBackground);
  QColor borderColor = OmniPainter::resolveColor(SemanticColor::PopoverBorder);
  int borderRadius = 10;

  // helps getting rid of weird opacity artifacting on first launch
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(rect(), Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);
  painter.setClipPath(path);
  finalColor.setAlphaF(config.launcherWindow.opacity);
  painter.setPen(QPen(borderColor, 2));
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);
}
