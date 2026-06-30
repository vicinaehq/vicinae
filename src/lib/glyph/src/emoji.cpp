#include "glyph/emoji.hpp"
#include "emoji-properties.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

typedef const char *emoji_text_iter_t;

// NOLINTBEGIN(bugprone-suspicious-include)
#include "segmenter/emoji-presentation-scanner.c"
// NOLINTEND(bugprone-suspicious-include)

namespace emoji {

static constexpr std::array<SkinToneInfo, skinToneCount> skinToneInfos = {
    SkinToneInfo{.tone = SkinTone::Default, .id = "default", .displayName = "Default", .utf8 = ""},
    SkinToneInfo{.tone = SkinTone::Light, .id = "light", .displayName = "Light", .utf8 = "\xF0\x9F\x8F\xBB"},
    SkinToneInfo{.tone = SkinTone::MediumLight,
                 .id = "medium-light",
                 .displayName = "Medium Light",
                 .utf8 = "\xF0\x9F\x8F\xBC"},
    SkinToneInfo{
        .tone = SkinTone::Medium, .id = "medium", .displayName = "Medium", .utf8 = "\xF0\x9F\x8F\xBD"},
    SkinToneInfo{.tone = SkinTone::MediumDark,
                 .id = "medium-dark",
                 .displayName = "Medium Dark",
                 .utf8 = "\xF0\x9F\x8F\xBE"},
    SkinToneInfo{.tone = SkinTone::Dark, .id = "dark", .displayName = "Dark", .utf8 = "\xF0\x9F\x8F\xBF"},
};

std::span<const SkinToneInfo> skinTones() { return skinToneInfos; }

SkinToneInfo skinToneInfo(SkinTone tone) { return skinToneInfos.at(static_cast<std::uint8_t>(tone)); }

static constexpr std::string_view kVariationSelector16 = "\xEF\xB8\x8F";

static std::string stripVariationSelectors(std::string_view str) {
  std::string result;
  result.reserve(str.size());

  size_t pos = 0;
  while (pos < str.size()) {
    if (pos + kVariationSelector16.size() <= str.size() &&
        str.substr(pos, kVariationSelector16.size()) == kVariationSelector16) {
      pos += kVariationSelector16.size();
    } else {
      result.push_back(str[pos]);
      pos++;
    }
  }

  return result;
}

static constexpr char32_t kInvalidCodepoint = 0xFFFFFFFF;

static int decodeUtf8(std::string_view str, size_t pos, char32_t &cp) {
  if (pos >= str.size()) {
    cp = kInvalidCodepoint;
    return 0;
  }

  auto byte = [&](size_t i) { return static_cast<unsigned char>(str[i]); };
  unsigned char const lead = byte(pos);

  if (lead < 0x80) {
    cp = lead;
    return 1;
  }

  int len = 0;
  char32_t min = 0;
  if ((lead & 0xE0) == 0xC0) {
    cp = lead & 0x1F;
    len = 2;
    min = 0x80;
  } else if ((lead & 0xF0) == 0xE0) {
    cp = lead & 0x0F;
    len = 3;
    min = 0x800;
  } else if ((lead & 0xF8) == 0xF0) {
    cp = lead & 0x07;
    len = 4;
    min = 0x10000;
  } else {
    cp = kInvalidCodepoint;
    return 1;
  }

  if (pos + len > str.size()) {
    cp = kInvalidCodepoint;
    return 1;
  }

  for (int k = 1; k < len; ++k) {
    unsigned char const cont = byte(pos + k);
    if ((cont & 0xC0) != 0x80) {
      cp = kInvalidCodepoint;
      return 1;
    }
    cp = (cp << 6) | (cont & 0x3F);
  }

  if (cp < min || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) { cp = kInvalidCodepoint; }

  return len;
}

std::string applySkinTone(std::string_view emoji, SkinTone tone) {
  const auto info = skinToneInfo(tone);
  char32_t c = 0;
  size_t const i = decodeUtf8(emoji, 0, c);

  if (c == kInvalidCodepoint) { return std::string(emoji); }

  std::string result;

  result.reserve(emoji.size() + info.utf8.size());
  result.append(emoji.substr(0, i));
  result.append(info.utf8);
  // variation selectors should not appear if we add skin tone, because the skin tone makes it clear
  // it needs to be treated as a colored emoji.
  result.append(stripVariationSelectors(emoji.substr(i)));

  return result;
}

static constexpr char32_t kCombiningEnclosingKeycapCharacter = 0x20E3;
static constexpr char32_t kCombiningEnclosingCircleBackslashCharacter = 0x20E0;
static constexpr char32_t kZeroWidthJoinerCharacter = 0x200D;
static constexpr char32_t kVariationSelector15Character = 0xFE0E;
static constexpr char32_t kVariationSelector16Character = 0xFE0F;
static constexpr char kMaxEmojiScannerCategory = 16;

namespace Character {
static inline bool isEmojiModifierBase(char32_t cp) {
  return (lookupProperties(cp) & EmojiModifierBase) != 0;
}

static inline bool isModifier(char32_t cp) { return (lookupProperties(cp) & EmojiModifier) != 0; }

static inline bool isRegionalIndicator(char32_t cp) { return cp >= 0x1F1E6 && cp <= 0x1F1FF; }

static inline bool isEmojiKeycapBase(char32_t cp) {
  return (cp >= 0x0023 && cp <= 0x0023) || (cp >= 0x002A && cp <= 0x002A) || (cp >= 0x0030 && cp <= 0x0039);
}

static inline bool isEmojiEmojiDefault(char32_t cp) {
  return (lookupProperties(cp) & EmojiPresentation) != 0;
}

static inline bool isEmojiTextDefault(char32_t cp) {
  std::uint8_t const p = lookupProperties(cp);
  return (p & Emoji) && !(p & EmojiPresentation);
}

static inline bool isEmoji(char32_t cp) { return (lookupProperties(cp) & Emoji) != 0; }
} // namespace Character

enum class EmojiCategory : std::uint8_t {
  Emoji = 0,
  EmojiTextPresentation = 1,
  EmojiEmojiPresentation = 2,
  EmojiModifierBase = 3,
  EmojiModifier = 4,
  EmojiVsBase = 5,
  RegionalIndicator = 6,
  KeycapBase = 7,
  CombiningEnclosingKeycap = 8,
  CombiningEnclosingCircleBackslash = 9,
  ZWJ = 10,
  VS15 = 11,
  VS16 = 12,
  TagBase = 13,
  TagSequence = 14,
  TagTerm = 15,
};

static char emojiSegmentationCategory(char32_t codepoint) {
  using enum EmojiCategory;
  auto cat = [](EmojiCategory c) { return static_cast<char>(c); };
  if (codepoint == kCombiningEnclosingKeycapCharacter) return cat(CombiningEnclosingKeycap);
  if (codepoint == kCombiningEnclosingCircleBackslashCharacter) return cat(CombiningEnclosingCircleBackslash);
  if (codepoint == kZeroWidthJoinerCharacter) return cat(ZWJ);
  if (codepoint == kVariationSelector15Character) return cat(VS15);
  if (codepoint == kVariationSelector16Character) return cat(VS16);
  if (codepoint == 0x1F3F4) return cat(TagBase);
  if ((codepoint >= 0xE0030 && codepoint <= 0xE0039) || (codepoint >= 0xE0061 && codepoint <= 0xE007A))
    return cat(TagSequence);
  if (codepoint == 0xE007F) return cat(TagTerm);
  if (Character::isEmojiModifierBase(codepoint)) return cat(EmojiModifierBase);
  if (Character::isModifier(codepoint)) return cat(EmojiModifier);
  if (Character::isRegionalIndicator(codepoint)) return cat(RegionalIndicator);
  if (Character::isEmojiKeycapBase(codepoint)) return cat(KeycapBase);

  if (Character::isEmojiEmojiDefault(codepoint)) return cat(EmojiEmojiPresentation);
  if (Character::isEmojiTextDefault(codepoint)) return cat(EmojiTextPresentation);
  if (Character::isEmoji(codepoint)) return cat(Emoji);

  // Ragel state machine will interpret unknown category as "any".
  return kMaxEmojiScannerCategory;
}

static std::string segment(std::string_view str) {
  std::string result;
  result.reserve(str.size());

  size_t i = 0;
  while (i < str.size()) {
    char32_t c = 0;
    i += decodeUtf8(str, i, c);

    if (c == kInvalidCodepoint) { continue; }

    result.push_back(emojiSegmentationCategory(c));
  }

  return result;
}

bool isUtf8EncodedEmoji(std::string_view str) {
  bool is_emoji = false;
  [[maybe_unused]] bool has_vs = false;
  std::string segmented = segment(str);
  scan_emoji_presentation(segmented.data(), segmented.data() + segmented.size(), &is_emoji, &has_vs);
  return is_emoji;
}

} // namespace emoji
