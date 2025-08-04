#pragma once
#include "ui/image/image.hpp"

class EmojiImageLoader : public AbstractImageLoader {
  QString m_emoji;

  void render(const RenderConfig &config) override;

public:
  EmojiImageLoader(const QString &emoji);
};
