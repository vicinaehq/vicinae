#include "builtin-icon-loader.hpp"
#include "contrast-helper.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/image/image.hpp"
#include "svg-image-loader.hpp"
#include <qcolor.h>

void BuiltinIconLoader::render(const RenderConfig &config) { emit dataUpdated(renderSync(config)); }

QPixmap BuiltinIconLoader::renderSync(const RenderConfig &config) {
  QPixmap canva(config.size * config.devicePixelRatio);
  int margin = 0;

  canva.fill(Qt::transparent);

  if (m_backgroundColor) {
    OmniPainter painter(&canva);
    int side = qMin(config.size.width(), config.size.height());
    qreal radius = side * 0.25 * config.devicePixelRatio;
    margin = qRound(side * 0.15 * config.devicePixelRatio);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(painter.colorBrush(*m_backgroundColor));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(canva.rect(), radius, radius);
  }

  QMargins margins{margin, margin, margin, margin};
  QRect iconRect = canva.rect().marginsRemoved(margins);
  SvgImageLoader loader(m_iconName);

  if (m_backgroundColor) {
    QColor color = OmniPainter::resolveColor(*m_backgroundColor);
    loader.render(canva, iconRect, ContrastHelper::getTonalContrastColor(color, 3));
  } else {
    loader.render(canva, iconRect, config.fill.value_or(SemanticColor::Foreground));
  }

  canva.setDevicePixelRatio(config.devicePixelRatio);

  return canva;
}

void BuiltinIconLoader::setFillColor(const std::optional<ColorLike> &color) { m_fillColor = color; }
void BuiltinIconLoader::setBackgroundColor(const std::optional<ColorLike> &color) {
  m_backgroundColor = color;
}

BuiltinIconLoader::BuiltinIconLoader(const QString &iconName)
    : m_iconName(iconName), m_fillColor(SemanticColor::Foreground) {}
