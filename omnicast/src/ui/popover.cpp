#include "ui/popover.hpp"
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpixmap.h>

void Popover::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;
  QPainter painter(this);
  QPainterPath path;
  QPen pen(theme.colors.statusBackgroundBorder, 1);

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor finalColor(theme.colors.mainBackground);

  finalColor.setAlphaF(0.98);
  painter.setPen(pen);
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

Popover::Popover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);
}

void Popover::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  // recomputeWallpaper();
}

void Popover::recomputeWallpaper() {
  QPixmap pix("/home/aurelle/Downloads/sequoia.jpg");
  QPixmap wp(size());
  {
    OmniPainter painter(&wp);

    wp.fill(Qt::transparent);
    painter.drawBlurredPixmap(pix.scaled(size(), {}, Qt::SmoothTransformation), 30);
  }
  _wallpaper = wp;
}
