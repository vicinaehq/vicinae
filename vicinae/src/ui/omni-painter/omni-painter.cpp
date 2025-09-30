#include "ui/omni-painter/omni-painter.hpp"
#include "theme.hpp"
#include <qgraphicseffect.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qsvgrenderer.h>

class ColorResolver {
public:
  static QColor resolve(const ColorLike &color, const ThemeInfo &theme) {
    ColorResolver resolver(theme);
    auto result = std::visit(resolver, color);

    return result;
  }

  ColorResolver(const ThemeInfo &theme) : m_theme(theme) {}

  QColor operator()(const DynamicColor &dynamic) const {
    // for now, we ignore `adjustContrast`
    if (m_theme.appearance == "light") return dynamic.light;
    return dynamic.dark;
  }
  QColor operator()(const QColor &color) const { return color; }
  QColor operator()(const SemanticColor &color) const { return m_theme.resolveTint(color); }
  QColor operator()(const QString &text) const { return QColor(text); }

private:
  const ThemeInfo &m_theme;
};

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

QBrush OmniPainter::colorBrush(const ColorLike &colorLike) const { return QBrush(resolveColor(colorLike)); }

QColor OmniPainter::resolveColor(const ColorLike &colorLike) const {
  return ColorResolver::resolve(colorLike, ThemeService::instance().theme());
}

void OmniPainter::fillRect(QRect rect, const ColorLike &colorLike, int radius, float alpha) {
  auto color = resolveColor(colorLike);
  fillRect(rect, color, radius, alpha);
}

void OmniPainter::setThemePen(const ColorLike &color, int width) {
  QPainter::setPen(QPen(colorBrush(color), width));
}

void OmniPainter::setThemeBrush(const ColorLike &color) { QPainter::setBrush(colorBrush(color)); }

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
