#pragma once
#include <cstdint>

namespace AI {

/**
 * What a model is capable of. Some models can do many things at once.
 */
enum Capability : std::uint8_t {
  Completion = 1 << 0,
  Vision = 1 << 1,
  Thinking = 1 << 2,
  ToolCalling = 1 << 3,
  Embedding = 1 << 4,
  Transcription = 1 << 5,
  OCR = 1 << 6
};

using Capabilities = std::uint32_t;

} // namespace AI
