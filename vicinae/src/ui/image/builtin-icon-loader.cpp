#include "builtin-icon-loader.hpp"
#include "theme.hpp"
#include "ui/image/image.hpp"
#include "svg-image-loader.hpp"

void BuiltinIconLoader::render(const RenderConfig &config) { emit dataUpdated(renderSync(config)); }

QPixmap BuiltinIconLoader::renderSync(const RenderConfig &config) {
  QPixmap canva(config.size * config.devicePixelRatio);
  int margin = 0;

  canva.fill(Qt::transparent);

  if (m_backgroundColor) {
    OmniPainter painter(&canva);
    qreal radius = qRound(4 * config.devicePixelRatio);

    painter.setRenderHint(QPainter::Antialiasing, true);
    margin = qRound(3 * config.devicePixelRatio);
    painter.setBrush(painter.colorBrush(*m_backgroundColor));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(canva.rect(), radius, radius);
  }

  QMargins margins{margin, margin, margin, margin};
  QRect iconRect = canva.rect().marginsRemoved(margins);
  SvgImageLoader loader(m_iconName);

  loader.setFillColor(m_fillColor);
  loader.render(canva, iconRect);
  canva.setDevicePixelRatio(config.devicePixelRatio);

  return canva;
}

void BuiltinIconLoader::setFillColor(const std::optional<ColorLike> &color) { m_fillColor = color; }
void BuiltinIconLoader::setBackgroundColor(const std::optional<ColorLike> &color) {
  m_backgroundColor = color;
}

BuiltinIconLoader::BuiltinIconLoader(const QString &iconName)
    : m_iconName(iconName), m_fillColor(SemanticColor::TextPrimary) {}
