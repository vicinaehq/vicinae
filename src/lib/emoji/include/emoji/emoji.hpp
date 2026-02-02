#pragma once
#include <cstdint>
#include <string_view>
#include <span>
#include "generated/db.hpp"

namespace emoji {

enum class SkinTone : std::uint8_t { Light, MediumLight, Medium, MediumDark, Dark };

struct SkinToneInfo {
  SkinTone tone;
  std::string_view id;
  std::string_view displayName;
  std::string_view utf8;
};

constexpr std::uint8_t skinToneCount = 5;

std::span<const SkinToneInfo> skinTones();
SkinToneInfo skinToneInfo(SkinTone tone);
std::string applySkinTone(std::string_view emoji, SkinTone tone);

const EmojiData *findStaticEmoji(std::string_view emoji);
std::span<const EmojiData> emojis();

bool isUtf8EncodedEmoji(std::string_view str);

} // namespace emoji
