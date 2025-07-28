#include "ui/ellided-label.hpp"
#include <qevent.h>
#include <qlabel.h>
#include <qwidget.h>

EllidedLabel::EllidedLabel(const QString &text, QWidget *parent) : QLabel(text, parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void EllidedLabel::paintEvent(QPaintEvent *event) {
  QFrame::paintEvent(event);
  QPainter painter(this);
  auto metrics = fontMetrics();
  auto elided = metrics.elidedText(text(), Qt::ElideRight, width());

  painter.drawText(rect(), alignment(), elided);
}
