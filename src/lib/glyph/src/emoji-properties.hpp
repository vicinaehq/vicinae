#pragma once
#include <cstdint>

namespace emoji {

// Bit values must match EMOJI_PROP in the unicode generator's emoji-properties.ts.
enum EmojiProp : std::uint8_t {
  Emoji = 1,
  EmojiPresentation = 2,
  EmojiModifier = 4,
  EmojiModifierBase = 8,
};

std::uint8_t lookupProperties(char32_t cp);

} // namespace emoji
