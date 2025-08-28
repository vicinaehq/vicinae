#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <qcoreevent.h>

bool VerticalScrollArea::eventFilter(QObject *o, QEvent *e) {
  if (o == widget() && e->type() == QEvent::Resize) {
    widget()->setMaximumWidth(width());
    emit widgetResized();
  }

  return false;
}

void VerticalScrollArea::resizeEvent(QResizeEvent *event) {
  // Very cool trick that makes it so that the scrollbar doesn't shrink the space allocated
  // for the widget, positioning it on top of it instead.
  setViewportMargins(0, 0, -verticalScrollBar()->width(), 0);
  QScrollArea::resizeEvent(event);
}

VerticalScrollArea::VerticalScrollArea(QWidget *parent) : QScrollArea(parent) {
  setFocusPolicy(Qt::NoFocus);
  setWidgetResizable(true);
  setVerticalScrollBar(new OmniScrollBar);
  setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
}
