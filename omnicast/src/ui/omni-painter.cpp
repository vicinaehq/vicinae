#include "ui/omni-painter.hpp"
#include <qgraphicseffect.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qscopedpointer.h>

void OmniPainter::fillRect(QRect rect, const QColor &color, int radius, float alpha) {
  setBrush(color);
  drawRoundedRect(rect, radius, radius);
}

void OmniPainter::fillRect(QRect rect, const ThemeLinearGradient &lgrad, int radius, float alpha) {
  QLinearGradient gradient;

  for (int i = 0; i != lgrad.points.size(); ++i) {
    QColor finalPoint = lgrad.points[i];

    finalPoint.setAlphaF(alpha);
    gradient.setColorAt(i, finalPoint);
  }

  setBrush(gradient);
  drawRoundedRect(rect, radius, radius);
}

void OmniPainter::fillRect(QRect rect, const ThemeRadialGradient &rgrad, int radius, float alpha) {
  QRadialGradient gradient(rect.center(), rect.width() / 2.0);

  gradient.setSpread(QGradient::PadSpread);
  gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

  for (int i = 0; i != rgrad.points.size(); ++i) {
    QColor finalPoint = rgrad.points[i];

    finalPoint.setAlphaF(alpha);
    gradient.setColorAt(i, finalPoint);
  }

  setBrush(gradient);
  drawRoundedRect(rect, radius, radius);
}

void OmniPainter::fillRect(QRect rect, const ColorLike &colorLike, int radius, float alpha) {
  if (auto color = std::get_if<QColor>(&colorLike)) {
    fillRect(rect, *color, radius, alpha);
  } else if (auto lgrad = std::get_if<ThemeLinearGradient>(&colorLike)) {
    fillRect(rect, *lgrad, radius, alpha);
  } else if (auto rgrad = std::get_if<ThemeRadialGradient>(&colorLike)) {
    fillRect(rect, *rgrad, radius, alpha);
  } else if (auto tint = std::get_if<ColorTint>(&colorLike)) {
    auto color = ThemeService::instance().getTintColor(*tint);

    if (std::get_if<ColorTint>(&color)) {
      qWarning() << "Theme color set to color tint, not allowed! No color will be set to avoid loop";
      return;
    }

    fillRect(rect, color, radius, alpha);
  }
}

void OmniPainter::drawBlurredPixmap(const QPixmap &pixmap, int blurRadius) {
  auto blur = new QGraphicsBlurEffect;

  blur->setBlurRadius(blurRadius); // Adjust radius as needed
  blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);

  // Apply the blur using QGraphicsScene
  QGraphicsScene scene;
  QGraphicsPixmapItem item;
  item.setPixmap(pixmap);
  item.setGraphicsEffect(blur);
  scene.addItem(&item);
  scene.render(this);
  delete blur;
}
