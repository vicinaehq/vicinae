#include "groq-provider.hpp"
#include <algorithm>
#include <array>
#include <ranges>
#include <span>
#include <string_view>

namespace AI {

namespace {

constexpr auto UNSUPPORTED_MARKERS = std::to_array<std::string_view>({"tts", "guard", "embedding"});
constexpr auto VISION_MARKERS = std::to_array<std::string_view>({"vision", "llama-4"});
constexpr auto REASONING_MARKERS =
    std::to_array<std::string_view>({"gpt-oss", "qwen3", "deepseek-r1", "qwq"});

bool containsAny(std::string_view id, std::span<const std::string_view> markers) {
  return std::ranges::any_of(markers, [&](auto marker) { return id.contains(marker); });
}

} // namespace

GroqProvider::GroqProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "groq", "https://api.groq.com/openai/v1") {}

Capabilities GroqProvider::capabilitiesFor(std::string_view id) const {
  if (id.contains("whisper")) return Capability::Transcription;
  if (containsAny(id, UNSUPPORTED_MARKERS)) return 0;
  if (id.contains("compound")) return Capability::Completion;

  Capabilities caps = Capability::Completion | Capability::ToolCalling;
  if (containsAny(id, VISION_MARKERS)) caps |= Capability::Vision;
  if (containsAny(id, REASONING_MARKERS)) caps |= Capability::Thinking;
  return caps;
}

} // namespace AI
