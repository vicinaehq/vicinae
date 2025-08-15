#include "emoji-image-loader.hpp"
#include "service-registry.hpp"
#include "font-service.hpp"

void EmojiImageLoader::render(const RenderConfig &config) {
  auto font = ServiceRegistry::instance()->fontService()->emojiFont();
  QPixmap canva(config.size * config.devicePixelRatio);

  font.setStyleStrategy(QFont::StyleStrategy::NoFontMerging);

  canva.fill(Qt::transparent);
  font.setPixelSize(canva.height() * 0.8);

  QPainter painter(&canva);

  painter.setFont(font);
  painter.drawText(canva.rect(), Qt::AlignCenter, m_emoji);
  canva.setDevicePixelRatio(config.devicePixelRatio);

  emit dataUpdated(canva);
}

EmojiImageLoader::EmojiImageLoader(const QString &emoji) : m_emoji(emoji) {}
