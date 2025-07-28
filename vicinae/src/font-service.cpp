#include "font-service.hpp"

static const std::vector<QString> unixEmojiFontCandidates = {
    "Twemoji",
    "Noto Color Emoji",
    "JoyPixels",
    "Emoji One",
};

QFont FontService::findEmojiFont() {
#ifdef Q_OS_WIN
  return QFont("Segoe UI Emoji");
#endif
#ifdef Q_OS_MACOS
  return QFont("Apple Color Emoji");
#endif

  auto it = std::ranges::find_if(unixEmojiFontCandidates, QFontDatabase::hasFamily);

  if (it != unixEmojiFontCandidates.end()) return *it;

  return {};
}

FontService::FontService() { m_emojiFont = findEmojiFont(); }
