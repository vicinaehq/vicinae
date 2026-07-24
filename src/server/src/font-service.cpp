#include "font-service.hpp"
#include <algorithm>
#include <QCoreApplication>
#include <optional>
#include <qfontdatabase.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <QProcessEnvironment>
#include <unordered_map>
#include <unordered_set>

static const auto UNIX_EMOJI_FONT_CANDIDATES = {
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

  if (it != UNIX_EMOJI_FONT_CANDIDATES.end()) return QFont(*it);

  for (const auto &font : QFontDatabase::families()) {
    if (font.contains("emoji", Qt::CaseInsensitive)) return font;
  }

  return {};
}

FontService::FontService() {
  m_emojiFont = findEmojiFont();

#ifdef Q_OS_LINUX
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
#endif

  if (m_builtinMonoFamily.isEmpty()) {
    m_builtinMonoFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
  }

  for (const auto *path : {":/fonts/NotoSansMath-Regular.ttf", ":/fonts/NotoSansSymbols2-Regular.ttf",
                           ":/fonts/NotoSansSymbols-Regular.ttf"}) {
    int const id = QFontDatabase::addApplicationFont(path);
    if (id == -1) {
      qWarning() << "Failed to load bundled symbol font" << path;
      continue;
    }
    auto families = QFontDatabase::applicationFontFamilies(id);
    if (!families.isEmpty()) m_symbolFamilies << families.first();
  }
  if (!m_builtinFamily.isEmpty()) m_symbolFamilies << m_builtinFamily;
}

namespace {

// Width modifiers (Condensed, Narrow, ...) are intentionally absent: distinct designs.
const std::unordered_set<QString> &styleTokens() {
  static const std::unordered_set<QString> tokens = {
      "thin",      "hairline",  "extralight", "ultralight", "light", "regular", "normal",
      "book",      "medium",    "semibold",   "demibold",   "demi",  "semi",    "bold",
      "extrabold", "ultrabold", "black",      "heavy",      "fat",   "poster",  "extra",
      "ultra",     "italic",    "oblique",    "inclined",
  };
  return tokens;
}

// Qt appends " [foundry]" to disambiguate a family registered by multiple foundries.
QString stripFoundry(const QString &family) {
  const qsizetype i = family.lastIndexOf(QLatin1String(" ["));
  if (i > 0 && family.endsWith(QLatin1Char(']'))) return family.left(i);
  return family;
}

QString baseFamily(const QString &family) {
  QStringList tokens = stripFoundry(family).split(' ', Qt::SkipEmptyParts);
  while (tokens.size() > 1 && styleTokens().contains(tokens.last().toLower())) {
    tokens.removeLast();
  }
  return tokens.join(' ');
}

QString representativeFamily(const QString &base, const std::vector<QString> &members) {
  if (std::ranges::find(members, base) != members.end()) return base;
  return *std::ranges::min_element(members, {}, &QString::size);
}

struct CategoryInfo {
  FontCategory category;
  QString name;
  QString glyph;
};

const std::vector<CategoryInfo> &categories() {
  static const std::vector<CategoryInfo> v = {
      {FontCategory::Latin, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Latin")),
       QStringLiteral("Aa")},
      {FontCategory::Cyrillic, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Cyrillic")),
       QStringLiteral("Аб")},
      {FontCategory::Greek, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Greek")),
       QStringLiteral("Αα")},
      {FontCategory::Monospace, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Monospace")),
       QStringLiteral("Aa")},
      {FontCategory::NerdFonts, QStringLiteral("Nerd Fonts"), QStringLiteral("Aa")},
      {FontCategory::Emoji, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Emoji")),
       QStringLiteral("😀")},
      {FontCategory::CJK, QStringLiteral("CJK"), QStringLiteral("永")},
      {FontCategory::Japanese, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Japanese")),
       QStringLiteral("あ")},
      {FontCategory::Korean, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Korean")),
       QStringLiteral("한")},
      {FontCategory::SimplifiedChinese,
       QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Simplified Chinese")), QStringLiteral("汉")},
      {FontCategory::TraditionalChinese,
       QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Traditional Chinese")), QStringLiteral("漢")},
      {FontCategory::Arabic, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Arabic")),
       QStringLiteral("أب")},
      {FontCategory::Hebrew, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Hebrew")),
       QStringLiteral("אב")},
      {FontCategory::Thai, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Thai")), QStringLiteral("ก")},
      {FontCategory::Lao, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Lao")), QStringLiteral("ກ")},
      {FontCategory::Devanagari, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Devanagari")),
       QStringLiteral("अ")},
      {FontCategory::Bengali, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Bengali")),
       QStringLiteral("অ")},
      {FontCategory::Gurmukhi, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Gurmukhi")),
       QStringLiteral("ਅ")},
      {FontCategory::Gujarati, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Gujarati")),
       QStringLiteral("અ")},
      {FontCategory::Tamil, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Tamil")),
       QStringLiteral("அ")},
      {FontCategory::Telugu, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Telugu")),
       QStringLiteral("అ")},
      {FontCategory::Kannada, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Kannada")),
       QStringLiteral("ಅ")},
      {FontCategory::Malayalam, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Malayalam")),
       QStringLiteral("അ")},
      {FontCategory::Sinhala, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Sinhala")),
       QStringLiteral("අ")},
      {FontCategory::Armenian, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Armenian")),
       QStringLiteral("Աա")},
      {FontCategory::Georgian, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Georgian")),
       QStringLiteral("ა")},
      {FontCategory::Thaana, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Thaana")),
       QStringLiteral("ތ")},
      {FontCategory::Tibetan, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Tibetan")),
       QStringLiteral("ཀ")},
      {FontCategory::Myanmar, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Myanmar")),
       QStringLiteral("က")},
      {FontCategory::Khmer, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Khmer")),
       QStringLiteral("ក")},
      {FontCategory::Syriac, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Syriac")), QString()},
      {FontCategory::Ogham, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Ogham")), QString()},
      {FontCategory::Runic, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Runic")), QString()},
      {FontCategory::Nko, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "N'Ko")), QString()},
      {FontCategory::Symbols, QStringLiteral(QT_TRANSLATE_NOOP("font-categories", "Symbols")), QString()},
  };
  return v;
}

const CategoryInfo *categoryInfo(FontCategory category) {
  for (const auto &info : categories()) {
    if (info.category == category) return &info;
  }
  return nullptr;
}

const std::vector<std::pair<QFontDatabase::WritingSystem, FontCategory>> &distinctiveScripts() {
  static const std::vector<std::pair<QFontDatabase::WritingSystem, FontCategory>> v = {
      {QFontDatabase::Arabic, FontCategory::Arabic},
      {QFontDatabase::Hebrew, FontCategory::Hebrew},
      {QFontDatabase::Thai, FontCategory::Thai},
      {QFontDatabase::Lao, FontCategory::Lao},
      {QFontDatabase::Devanagari, FontCategory::Devanagari},
      {QFontDatabase::Bengali, FontCategory::Bengali},
      {QFontDatabase::Gurmukhi, FontCategory::Gurmukhi},
      {QFontDatabase::Gujarati, FontCategory::Gujarati},
      {QFontDatabase::Tamil, FontCategory::Tamil},
      {QFontDatabase::Telugu, FontCategory::Telugu},
      {QFontDatabase::Kannada, FontCategory::Kannada},
      {QFontDatabase::Malayalam, FontCategory::Malayalam},
      {QFontDatabase::Sinhala, FontCategory::Sinhala},
      {QFontDatabase::Thaana, FontCategory::Thaana},
      {QFontDatabase::Tibetan, FontCategory::Tibetan},
      {QFontDatabase::Myanmar, FontCategory::Myanmar},
      {QFontDatabase::Khmer, FontCategory::Khmer},
      {QFontDatabase::Armenian, FontCategory::Armenian},
      {QFontDatabase::Georgian, FontCategory::Georgian},
      {QFontDatabase::Syriac, FontCategory::Syriac},
      {QFontDatabase::Ogham, FontCategory::Ogham},
      {QFontDatabase::Runic, FontCategory::Runic},
      {QFontDatabase::Nko, FontCategory::Nko},
  };
  return v;
}

using WritingSystems = QList<QFontDatabase::WritingSystem>;

QString pangramFor(QFontDatabase::WritingSystem ws) {
  static const std::vector<std::pair<QFontDatabase::WritingSystem, QString>> table = {
      {QFontDatabase::Latin, QStringLiteral("The quick brown fox jumps over the lazy dog")},
      {QFontDatabase::Cyrillic, QStringLiteral("Съешь же ещё этих мягких французских булок да выпей чаю")},
      {QFontDatabase::Greek, QStringLiteral("Ταχίστη αλώπηξ βαφής ψημένη γη, δρασκελίζει υπέρ νωθρού κυνός")},
      {QFontDatabase::Japanese, QStringLiteral("いろはにほへと ちりぬるを わかよたれそ つねならむ "
                                               "うゐのおくやま けふこえて あさきゆめみし ゑひもせす")},
      {QFontDatabase::Korean,
       QStringLiteral("키스의 고유조건은 입술끼리 만나야 하고 특별한 기술은 필요치 않다")},
      {QFontDatabase::SimplifiedChinese, QStringLiteral("视野无限广，窗外有蓝天")},
      {QFontDatabase::TraditionalChinese, QStringLiteral("視野無限廣，窗外有藍天")},
      {QFontDatabase::Arabic, QStringLiteral("صِف خَلقَ خَودِ كَمِثلِ الشَمسِ إِذ بَزَغَت — يَحظى الضَجيعُ بِها نَجلاءَ مِعطارِ")},
      {QFontDatabase::Hebrew, QStringLiteral("דג סקרן שט בים מאוכזב ולפתע מצא חברה")},
      {QFontDatabase::Thai, QStringLiteral("เป็นมนุษย์สุดประเสริฐเลิศคุณค่า กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ")},
      {QFontDatabase::Devanagari,
       QStringLiteral("ऋषियों को सताने वाले दुष्ट राक्षसों के राजा रावण का सर्वनाश करने वाले भगवान श्रीराम")},
  };
  for (const auto &[w, text] : table) {
    if (w == ws) return text;
  }
  return QFontDatabase::writingSystemSample(ws);
}

// Secondary-script order for the demo; the font's primary leads (see specimenMarkdown).
const std::vector<QFontDatabase::WritingSystem> &demoScripts() {
  static const std::vector<QFontDatabase::WritingSystem> v = {
      QFontDatabase::Japanese,
      QFontDatabase::Korean,
      QFontDatabase::SimplifiedChinese,
      QFontDatabase::TraditionalChinese,
      QFontDatabase::Latin,
      QFontDatabase::Cyrillic,
      QFontDatabase::Greek,
      QFontDatabase::Arabic,
      QFontDatabase::Hebrew,
      QFontDatabase::Thai,
      QFontDatabase::Lao,
      QFontDatabase::Devanagari,
      QFontDatabase::Bengali,
      QFontDatabase::Gurmukhi,
      QFontDatabase::Gujarati,
      QFontDatabase::Tamil,
      QFontDatabase::Telugu,
      QFontDatabase::Kannada,
      QFontDatabase::Malayalam,
      QFontDatabase::Sinhala,
      QFontDatabase::Thaana,
      QFontDatabase::Tibetan,
      QFontDatabase::Myanmar,
      QFontDatabase::Khmer,
      QFontDatabase::Syriac,
      QFontDatabase::Ogham,
      QFontDatabase::Runic,
      QFontDatabase::Nko,
  };
  return v;
}

std::optional<QFontDatabase::WritingSystem> primaryWritingSystem(FontCategory category) {
  switch (category) {
  case FontCategory::Latin:
  case FontCategory::Monospace:
  case FontCategory::NerdFonts:
    return QFontDatabase::Latin;
  case FontCategory::CJK:
  case FontCategory::Japanese:
    return QFontDatabase::Japanese;
  case FontCategory::Korean:
    return QFontDatabase::Korean;
  case FontCategory::SimplifiedChinese:
    return QFontDatabase::SimplifiedChinese;
  case FontCategory::TraditionalChinese:
    return QFontDatabase::TraditionalChinese;
  default:
    break;
  }
  for (const auto &[ws, cat] : distinctiveScripts()) {
    if (cat == category) return ws;
  }
  return std::nullopt;
}

FontCategory scriptCategory(const WritingSystems &systems) {
  if (systems.isEmpty()) return FontCategory::Symbols;

  auto has = [&](QFontDatabase::WritingSystem ws) { return std::ranges::find(systems, ws) != systems.end(); };
  auto hasEuropean = [&] {
    return has(QFontDatabase::Latin) || has(QFontDatabase::Cyrillic) || has(QFontDatabase::Greek);
  };

  const bool japanese = has(QFontDatabase::Japanese);
  const bool korean = has(QFontDatabase::Korean);
  if (japanese && korean) return FontCategory::CJK;
  if (japanese) return FontCategory::Japanese;
  if (korean) return FontCategory::Korean;
  if (has(QFontDatabase::SimplifiedChinese)) return FontCategory::SimplifiedChinese;
  if (has(QFontDatabase::TraditionalChinese)) return FontCategory::TraditionalChinese;

  std::vector<FontCategory> distinctive;
  for (const auto &[ws, category] : distinctiveScripts()) {
    if (has(ws)) distinctive.push_back(category);
  }

  if (distinctive.size() == 1) return distinctive.front();
  if (!distinctive.empty()) return hasEuropean() ? FontCategory::Latin : distinctive.front();

  if (hasEuropean()) return FontCategory::Latin;
  return FontCategory::Symbols;
}

bool nameLooksEmoji(const QString &family) {
  const QString lower = family.toLower();
  for (const auto *needle : {"emoji", "openmoji", "joypixels", "blobmoji"}) {
    if (lower.contains(QLatin1String(needle))) return true;
  }
  return false;
}

bool nameLooksNerdFont(const QString &family) {
  if (family.contains(QLatin1String("Nerd Font"), Qt::CaseInsensitive)) return true;
  for (const auto &token : family.split(' ', Qt::SkipEmptyParts)) {
    for (const auto *abbr : {"NF", "NFM", "NFP"}) {
      if (token.compare(QLatin1String(abbr), Qt::CaseInsensitive) == 0) return true;
    }
  }
  return false;
}

// URW PostScript symbol fonts map their symbols onto plain ASCII, so the name is the only tell.
bool isKnownSymbolFont(const QString &family) {
  static const std::unordered_set<QString> known = {"d050000l", "standard symbols ps"};
  return known.contains(family.toLower());
}

bool emojiIsColor(const QString &family) {
  const QString lower = family.toLower();
  if (lower.contains(QLatin1String("color"))) return true;
  if (lower.contains(QLatin1String("mono")) || lower.contains(QLatin1String("black"))) return false;
  if (lower.contains(QLatin1String("noto")) && lower.contains(QLatin1String("emoji"))) return false;
  return true;
}

QString glyphFor(FontCategory category, const WritingSystems &systems) {
  if (category == FontCategory::Latin || category == FontCategory::Monospace ||
      category == FontCategory::NerdFonts) {
    auto has = [&](QFontDatabase::WritingSystem ws) {
      return std::ranges::find(systems, ws) != systems.end();
    };
    if (has(QFontDatabase::Latin)) return QStringLiteral("Aa");
    if (has(QFontDatabase::Cyrillic)) return QStringLiteral("Аб");
    if (has(QFontDatabase::Greek)) return QStringLiteral("Αα");
    return {};
  }
  const CategoryInfo *info = categoryInfo(category);
  return info ? info->glyph : QString();
}

struct Classified {
  FontCategory primary;
  FontCategoryMask categories;
  QString glyph;
  bool color;
};

Classified categorize(const QString &family) {
  const WritingSystems systems = QFontDatabase::writingSystems(family);

  if (nameLooksEmoji(family))
    return {FontCategory::Emoji, categoryBit(FontCategory::Emoji), glyphFor(FontCategory::Emoji, systems),
            emojiIsColor(family)};

  FontCategory script = scriptCategory(systems);
  if (script == FontCategory::Latin && isKnownSymbolFont(family)) script = FontCategory::Symbols;

  const bool nerd = nameLooksNerdFont(family);
  const bool mono = QFontDatabase::isFixedPitch(family);

  const FontCategory primary = nerd                                      ? FontCategory::NerdFonts
                               : (mono && script == FontCategory::Latin) ? FontCategory::Monospace
                                                                         : script;

  FontCategoryMask mask = categoryBit(primary) | categoryBit(script);
  if (nerd) mask |= categoryBit(FontCategory::NerdFonts);
  if (mono) mask |= categoryBit(FontCategory::Monospace);

  // Filter-only facets: never a primary section, but tagged on any font that covers them.
  auto has = [&](QFontDatabase::WritingSystem ws) { return std::ranges::find(systems, ws) != systems.end(); };
  if (has(QFontDatabase::Cyrillic)) mask |= categoryBit(FontCategory::Cyrillic);
  if (has(QFontDatabase::Greek)) mask |= categoryBit(FontCategory::Greek);

  return {primary, mask, glyphFor(primary, systems), false};
}

} // namespace

const std::vector<FontFamily> &FontService::fontFamilies() const {
  if (m_fontFamiliesBuilt) return m_fontFamilies;

  const QStringList all = QFontDatabase::families();
  std::vector<QString> order;
  std::unordered_map<QString, std::vector<QString>> groups;
  for (const auto &rawFamily : all) {
    // Bitmap fonts render badly (and can be pathologically slow) scaled up to a preview;
    // skip them, keeping color-emoji fonts which are bitmap by nature.
    if (!nameLooksEmoji(rawFamily) && !QFontDatabase::isSmoothlyScalable(rawFamily)) continue;
    const QString family = stripFoundry(rawFamily);
    const QString base = baseFamily(family);
    auto [it, inserted] = groups.try_emplace(base);
    if (inserted) order.push_back(base);
    it->second.push_back(family);
  }

  m_fontFamilies.reserve(order.size());
  for (const auto &base : order) {
    const QString rep = representativeFamily(base, groups.at(base));
    const auto [primary, categories, glyph, color] = categorize(rep);
    if (primary == FontCategory::Symbols) continue;
    m_fontFamilies.push_back({.name = base,
                              .family = rep,
                              .primary = primary,
                              .categories = categories,
                              .glyph = glyph,
                              .color = color});
  }
  m_fontFamiliesBuilt = true;
  return m_fontFamilies;
}

QString FontService::categoryName(FontCategory category) {
  const CategoryInfo *info = categoryInfo(category);
  return info ? info->name : QString();
}

QString FontService::specimenMarkdown(const QString &family, FontCategory category) {
  auto block = [](QStringView sample) {
    return QStringLiteral("# %1\n\n%1\n\n**%1**\n\n*%1*\n\n").arg(sample);
  };

  if (category == FontCategory::Emoji)
    return block(
        QStringLiteral("😀 😂 😍 😎 🥳 😭 👍 🙏 🎉 ❤️ 🔥 ⭐ 🌈 ☀️ 🐶 🐱 🦊 🐧 🍕 🍩 ☕ 🚀 ✈️ ⚽ 🎸 📷 🇫🇷 🇯🇵"));

  const auto systems = QFontDatabase::writingSystems(family);
  auto has = [&](QFontDatabase::WritingSystem ws) { return std::ranges::find(systems, ws) != systems.end(); };

  std::vector<QFontDatabase::WritingSystem> order;
  const auto primary = primaryWritingSystem(category);
  if (primary && has(*primary)) order.push_back(*primary);
  for (const auto ws : demoScripts()) {
    if (has(ws) && primary != ws) order.push_back(ws);
  }

  QString md;
  bool chineseShown = false;
  for (const auto ws : order) {
    const bool chinese = ws == QFontDatabase::SimplifiedChinese || ws == QFontDatabase::TraditionalChinese;
    if (chinese && chineseShown) continue;
    chineseShown = chineseShown || chinese;
    if (!md.isEmpty()) md += QStringLiteral("---\n\n");
    md += block(pangramFor(ws));
  }
  if (md.isEmpty()) md = block(pangramFor(QFontDatabase::Latin));
  return md;
}

const std::vector<FontCategory> &FontService::orderedCategories() {
  static const std::vector<FontCategory> order = [] {
    std::vector<FontCategory> v;
    v.reserve(categories().size());
    for (const auto &info : categories()) {
      v.push_back(info.category);
    }
    return v;
  }();
  return order;
}
