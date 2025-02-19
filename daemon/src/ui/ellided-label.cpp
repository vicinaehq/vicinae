#include "ui/ellided-label.hpp"
#include <qevent.h>

EllidedLabel::EllidedLabel() { setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); }

void EllidedLabel::paintEvent(QPaintEvent *event) {
  QFrame::paintEvent(event);
  QPainter painter(this);
  auto metrics = fontMetrics();
  auto elided = metrics.elidedText(text(), Qt::ElideRight, width());

  painter.drawText(QPoint(0, metrics.ascent()), elided);
}
