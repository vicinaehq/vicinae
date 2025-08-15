#include "font-service.hpp"
#include <qfontdatabase.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qprocess.h>

static const std::vector<QString> UNIX_EMOJI_FONT_CANDIDATES = {
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

  auto environ = QProcessEnvironment::systemEnvironment();

  if (auto emojiFont = environ.value("EMOJI_FONT"); !emojiFont.isEmpty()) {
    if (!QFontDatabase::hasFamily(emojiFont)) {
      qWarning() << "EMOJI_FONT environment variable was set to" << emojiFont
                 << "but there is no such family installed.";
    }

    return QFont(emojiFont);
  }

  auto it = std::ranges::find_if(UNIX_EMOJI_FONT_CANDIDATES, QFontDatabase::hasFamily);

  if (it != UNIX_EMOJI_FONT_CANDIDATES.end()) return *it;

  for (const auto &font : QFontDatabase::families()) {
    if (font.contains("emoji", Qt::CaseInsensitive)) return font;
  }

  return {};
}

FontService::FontService() { m_emojiFont = findEmojiFont(); }
