#pragma once
#include <qfont.h>
#include <qfontdatabase.h>
#include <qfontinfo.h>
#include <qhash.h>
#include <qstring.h>
#include <qlist.h>
#include <qdebug.h>
#include <cstdint>
#include <vector>

enum class FontCategory : std::uint8_t {
  Latin,
  Cyrillic,
  Greek,
  Monospace,
  NerdFonts,
  Emoji,
  CJK,
  Japanese,
  Korean,
  SimplifiedChinese,
  TraditionalChinese,
  Arabic,
  Hebrew,
  Thai,
  Lao,
  Devanagari,
  Bengali,
  Gurmukhi,
  Gujarati,
  Tamil,
  Telugu,
  Kannada,
  Malayalam,
  Sinhala,
  Armenian,
  Georgian,
  Thaana,
  Tibetan,
  Myanmar,
  Khmer,
  Syriac,
  Ogham,
  Runic,
  Nko,
  Symbols,
};

using FontCategoryMask = std::uint64_t;
constexpr FontCategoryMask categoryBit(FontCategory category) {
  return FontCategoryMask{1} << static_cast<int>(category);
}

struct FontFamily {
  QString name;
  QString family;
  FontCategory primary;
  FontCategoryMask categories;
  QString glyph;
  bool color;

  bool has(FontCategory category) const { return (categories & categoryBit(category)) != 0; }
};

class FontService {
  QFont m_emojiFont;
  QString m_builtinFamily;
  QString m_builtinMonoFamily;
  QStringList m_symbolFamilies;
  mutable std::vector<FontFamily> m_fontFamilies;
  mutable bool m_fontFamiliesBuilt = false;
  QFont findEmojiFont();

public:
  const QFont &emojiFont() const { return m_emojiFont; }
  const QString &builtinFontFamily() const { return m_builtinFamily; }
  const QString &builtinMonoFontFamily() const { return m_builtinMonoFamily; }
  const QStringList &symbolFontFamilies() const { return m_symbolFamilies; }
  QStringList families() const { return QFontDatabase::families(); }

  const std::vector<FontFamily> &fontFamilies() const;

  static QString categoryName(FontCategory category);
  static const std::vector<FontCategory> &orderedCategories();
  static QString specimenMarkdown(const QString &family, FontCategory category);

  FontService();
};
