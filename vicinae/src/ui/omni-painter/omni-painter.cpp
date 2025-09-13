#include "ui/omni-painter/omni-painter.hpp"
#include "theme.hpp"
#include <qgraphicseffect.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qsvgrenderer.h>

OmniPainter::ImageMaskType OmniPainter::maskForName(const QString &name) {
  if (name == "circle") {
    return CircleMask;
  } else if (name == "roundedRectangle") {
    return RoundedRectangleMask;
  }

  return NoMask;
}

void OmniPainter::fillRect(QRect rect, const QColor &color, int radius, float alpha) {
  QColor final(color);

  final.setAlphaF(alpha);
  setBrush(final);
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

QColor OmniPainter::resolveColor(SemanticColor color) const {
  return ThemeService::instance().theme().resolveTint(color);
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

QBrush OmniPainter::colorBrush(const ColorLike &colorLike) const {
  if (auto color = std::get_if<QColor>(&colorLike)) {
    return *color;
  } else if (auto lgrad = std::get_if<ThemeLinearGradient>(&colorLike)) {
    QLinearGradient gradient;

    for (int i = 0; i != lgrad->points.size(); ++i) {
      QColor finalPoint = lgrad->points[i];

      finalPoint.setAlphaF(1);
      gradient.setColorAt(i, finalPoint);
    }

    return gradient;
  } else if (auto rgrad = std::get_if<ThemeRadialGradient>(&colorLike)) {
    QRadialGradient gradient;

    gradient.setSpread(QGradient::PadSpread);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

    for (int i = 0; i != rgrad->points.size(); ++i) {
      QColor finalPoint = rgrad->points[i];

      finalPoint.setAlphaF(1);
      gradient.setColorAt(i, finalPoint);
    }

    return gradient;
  } else if (auto tint = std::get_if<SemanticColor>(&colorLike)) {
    auto color = ThemeService::instance().getTintColor(*tint);

    if (std::get_if<SemanticColor>(&color)) {
      qWarning() << "Theme color set to color tint, not allowed! No color will be set to avoid loop";
      return {};
    }

    return colorBrush(color);
  }

  return {};
}

void OmniPainter::fillRect(QRect rect, const ColorLike &colorLike, int radius, float alpha) {
  if (auto color = std::get_if<QColor>(&colorLike)) {
    fillRect(rect, *color, radius, alpha);
  } else if (auto lgrad = std::get_if<ThemeLinearGradient>(&colorLike)) {
    fillRect(rect, *lgrad, radius, alpha);
  } else if (auto rgrad = std::get_if<ThemeRadialGradient>(&colorLike)) {
    fillRect(rect, *rgrad, radius, alpha);
  } else if (auto tint = std::get_if<SemanticColor>(&colorLike)) {
    auto color = ThemeService::instance().getTintColor(*tint);

    if (std::get_if<SemanticColor>(&color)) {
      qWarning() << "Theme color set to color tint, not allowed! No color will be set to avoid loop";
      return;
    }

    fillRect(rect, color, radius, alpha);
  }
}

void OmniPainter::setThemePen(const ColorLike &color, int width) {
  QPainter::setPen(QPen(colorBrush(color), width));
}

void OmniPainter::setThemeBrush(const ColorLike &color) { QPainter::setBrush(colorBrush(color)); }

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

void OmniPainter::drawPixmap(const QRect &rect, const QPixmap &pixmap, ImageMaskType mask) {
  QPainterPath path;

  setRenderHint(QPainter::Antialiasing);

  if (mask == ImageMaskType::CircleMask) {
    path.addEllipse(rect);
    setClipPath(path);
  } else if (mask == ImageMaskType::RoundedRectangleMask) {
    path.addRoundedRect(rect, 6, 6);
    setClipPath(path);
  }

  QPainter::drawPixmap(rect, pixmap);
}
