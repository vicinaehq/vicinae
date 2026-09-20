#include "openai-provider.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <ranges>

namespace AI {

namespace {

constexpr auto UNSUPPORTED_MARKERS = std::to_array<std::string_view>({
    "tts",
    "dall-e",
    "gpt-image",
    "moderation",
    "realtime",
    "audio",
    "search-preview",
    "computer-use",
    "codex",
    "davinci",
    "babbage",
    "sora",
    "instruct",
});

constexpr auto REASONING_PREFIXES = std::to_array<std::string_view>({"o1", "o3", "o4", "gpt-5"});
constexpr auto TEXT_ONLY_PREFIXES = std::to_array<std::string_view>({"gpt-3.5", "o1-mini", "o3-mini"});

bool allDigits(std::string_view s) {
  return !s.empty() && std::ranges::all_of(s, [](unsigned char c) { return std::isdigit(c); });
}

bool isSnapshot(std::string_view id) {
  if (id.size() > 11) {
    const auto tail = id.substr(id.size() - 11);
    if (tail[0] == '-' && tail[5] == '-' && tail[8] == '-' && allDigits(tail.substr(1, 4)) &&
        allDigits(tail.substr(6, 2)) && allDigits(tail.substr(9, 2))) {
      return true;
    }
  }

  const auto pos = id.rfind('-');
  return pos != std::string_view::npos && id.size() - pos == 5 && allDigits(id.substr(pos + 1));
}

std::string_view reasoningEffort(ThinkingMode mode) {
  switch (mode) {
  case ThinkingMode::None:
  case ThinkingMode::Low:
    return "low";
  case ThinkingMode::Medium:
    return "medium";
  case ThinkingMode::High:
    return "high";
  }
  return "medium";
}

} // namespace

OpenAIProvider::OpenAIProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "openai", "https://api.openai.com/v1") {}

Capabilities OpenAIProvider::capabilitiesFor(std::string_view id) const {
  if (isSnapshot(id)) return 0;
  if (id.starts_with("whisper") || id.contains("transcribe")) return Capability::Transcription;
  if (id.starts_with("text-embedding")) return Capability::Embedding;
  if (std::ranges::any_of(UNSUPPORTED_MARKERS, [&](auto marker) { return id.contains(marker); })) return 0;

  Capabilities caps = Capability::Completion | Capability::ToolCalling;
  if (std::ranges::any_of(REASONING_PREFIXES, [&](auto p) { return id.starts_with(p); })) {
    caps |= Capability::Thinking;
  }
  if (std::ranges::none_of(TEXT_ONLY_PREFIXES, [&](auto p) { return id.starts_with(p); })) {
    caps |= Capability::Vision;
  }
  return caps;
}

openai::ChatRequest OpenAIProvider::makeRequest(std::string_view modelId, const Model &model,
                                                const ChatCompletionPayload &payload) const {
  auto request = openai::ChatRequest::make(modelId, payload);

  if (model.caps & Capability::Thinking) {
    request.temperature.reset();
    request.reasoning_effort = std::string(reasoningEffort(payload.thinking));
  }

  return request;
}

} // namespace AI
