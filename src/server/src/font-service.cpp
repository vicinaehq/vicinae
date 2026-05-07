#include "font-service.hpp"
#include <qfontdatabase.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <QProcessEnvironment>

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

  if (auto emojiFont = qEnvironmentVariable("EMOJI_FONT"); !emojiFont.isEmpty()) {
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

FontService::FontService() {
  m_emojiFont = findEmojiFont();

  int const bodyId = QFontDatabase::addApplicationFont(":/fonts/Outfit-Variable.ttf");
  if (bodyId != -1) {
    auto families = QFontDatabase::applicationFontFamilies(bodyId);
    if (!families.isEmpty()) m_builtinFamily = families.first();
  } else {
    qWarning() << "Failed to load bundled Outfit font";
  }

  int const monoId = QFontDatabase::addApplicationFont(":/fonts/GeistMono-Variable.ttf");
  if (monoId != -1) {
    auto families = QFontDatabase::applicationFontFamilies(monoId);
    if (!families.isEmpty()) m_builtinMonoFamily = families.first();
  } else {
    qWarning() << "Failed to load bundled Geist Mono font";
  }
}
