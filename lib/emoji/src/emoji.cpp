#include "emoji/emoji.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <unicode/uchar.h>
#include <unicode/utf8.h>

typedef const char *emoji_text_iter_t;

#include "segmenter/emoji-presentation-scanner.c"

namespace emoji {

static constexpr std::array<SkinToneInfo, skinToneCount> skinToneInfos = {
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

std::string applySkinTone(std::string_view emoji, SkinTone tone) {
  const auto info = skinToneInfo(tone);
  int32_t i = 0;
  int32_t const length = static_cast<int32_t>(emoji.size());
  UChar32 c;

  U8_NEXT(emoji, i, length, c);

  if (c < 0) { return std::string(emoji); }

  std::string result;

  result.reserve(emoji.size() + info.utf8.size());
  result.append(emoji.substr(0, i));
  result.append(info.utf8);
  // variation selectors should not appear if we add skin tone, because the skin tone makes it clear
  // it needs to be treated as a colored emoji.
  result.append(stripVariationSelectors(emoji.substr(i)));

  return result;
}

const EmojiData *findStaticEmoji(std::string_view emoji) {
  auto &map = StaticEmojiDatabase::mapping();
  if (auto it = map.find(emoji); it != map.end()) { return it->second; }
  return nullptr;
}

std::span<const EmojiData> emojis() { return StaticEmojiDatabase::orderedList(); }

static constexpr char32_t kCombiningEnclosingKeycapCharacter = 0x20E3;
static constexpr char32_t kCombiningEnclosingCircleBackslashCharacter = 0x20E0;
static constexpr char32_t kZeroWidthJoinerCharacter = 0x200D;
static constexpr char32_t kVariationSelector15Character = 0xFE0E;
static constexpr char32_t kVariationSelector16Character = 0xFE0F;
static constexpr char kMaxEmojiScannerCategory = 16;

namespace Character {
static inline bool isEmojiModifierBase(char32_t cp) {
  return u_hasBinaryProperty(cp, UCHAR_EMOJI_MODIFIER_BASE);
}

static inline bool isModifier(char32_t cp) { return u_hasBinaryProperty(cp, UCHAR_EMOJI_MODIFIER); }

static inline bool isRegionalIndicator(char32_t cp) { return cp >= 0x1F1E6 && cp <= 0x1F1FF; }

static inline bool isEmojiKeycapBase(char32_t cp) {
  return (cp >= 0x0023 && cp <= 0x0023) || (cp >= 0x002A && cp <= 0x002A) || (cp >= 0x0030 && cp <= 0x0039);
}

static inline bool isEmojiEmojiDefault(char32_t cp) {
  return u_hasBinaryProperty(cp, UCHAR_EMOJI_PRESENTATION);
}

static inline bool isEmojiTextDefault(char32_t cp) {
  return u_hasBinaryProperty(cp, UCHAR_EMOJI) && !u_hasBinaryProperty(cp, UCHAR_EMOJI_PRESENTATION);
}

static inline bool isEmoji(char32_t cp) { return u_hasBinaryProperty(cp, UCHAR_EMOJI); }
} // namespace Character

enum EmojiCategory : std::uint8_t {
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
  if (codepoint == kCombiningEnclosingKeycapCharacter) return CombiningEnclosingKeycap;
  if (codepoint == kCombiningEnclosingCircleBackslashCharacter) return CombiningEnclosingCircleBackslash;
  if (codepoint == kZeroWidthJoinerCharacter) return ZWJ;
  if (codepoint == kVariationSelector15Character) return VS15;
  if (codepoint == kVariationSelector16Character) return VS16;
  if (codepoint == 0x1F3F4) return TagBase;
  if ((codepoint >= 0xE0030 && codepoint <= 0xE0039) || (codepoint >= 0xE0061 && codepoint <= 0xE007A))
    return TagSequence;
  if (codepoint == 0xE007F) return TagTerm;
  if (Character::isEmojiModifierBase(codepoint)) return EmojiModifierBase;
  if (Character::isModifier(codepoint)) return EmojiModifier;
  if (Character::isRegionalIndicator(codepoint)) return RegionalIndicator;
  if (Character::isEmojiKeycapBase(codepoint)) return KeycapBase;

  if (Character::isEmojiEmojiDefault(codepoint)) return EmojiEmojiPresentation;
  if (Character::isEmojiTextDefault(codepoint)) return EmojiTextPresentation;
  if (Character::isEmoji(codepoint)) return Emoji;

  // Ragel state machine will interpret unknown category as "any".
  return kMaxEmojiScannerCategory;
}

static std::string segment(std::string_view str) {
  std::string result;
  result.reserve(str.size());

  int32_t i = 0;
  int32_t const length = static_cast<int32_t>(str.size());

  while (i < length) {
    UChar32 c;
    U8_NEXT(str, i, length, c);

    if (c < 0) { continue; }

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
