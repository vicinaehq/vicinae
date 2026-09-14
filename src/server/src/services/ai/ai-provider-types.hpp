#pragma once
#include <array>
#include <span>
#include <string_view>
#include <qcoreapplication.h>
#include "services/builtin-icon/builtin-icon.hpp"

namespace AI {

struct ProviderField {
  std::string_view key;
  const char *label;
  const char *description;
  std::string_view placeholder;
  bool secret = false;
};

struct ProviderTypeInfo {
  std::string_view type;
  std::string_view label;
  BuiltinIcon icon;
  std::string_view description;
  bool allowMultiple;
  std::span<const ProviderField> fields;
};

constexpr auto PROVIDER_TR_CONTEXT = "AIProviderTypes";
#define AI_PROVIDER_TR(text) QT_TRANSLATE_NOOP("AIProviderTypes", text)

inline constexpr auto OLLAMA_FIELDS = std::to_array<ProviderField>({
    {.key = "url",
     .label = AI_PROVIDER_TR("Server URL"),
     .description = AI_PROVIDER_TR("The address of your Ollama instance."),
     .placeholder = "http://localhost:11434"},
});

inline constexpr auto MISTRAL_FIELDS = std::to_array<ProviderField>({
    {.key = "apiKey",
     .label = AI_PROVIDER_TR("API Key"),
     .description = AI_PROVIDER_TR("Your Mistral AI API key. You can find it in your Mistral dashboard."),
     .placeholder = "sk-...",
     .secret = true},
});

inline constexpr auto PROVIDER_TYPES = std::to_array<ProviderTypeInfo>({
    {
        .type = "ollama",
        .label = "Ollama",
        .icon = BuiltinIcon::Ollama,
        .description = "Connect to a local or remote Ollama instance.",
        .allowMultiple = true,
        .fields = OLLAMA_FIELDS,
    },
    {
        .type = "mistral",
        .label = "Mistral",
        .icon = BuiltinIcon::Mistral,
        .description = "Mistral AI cloud API. Provides transcription and language models.",
        .allowMultiple = false,
        .fields = MISTRAL_FIELDS,
    },
});

inline const ProviderTypeInfo *findProviderType(std::string_view type) {
  for (const auto &info : PROVIDER_TYPES) {
    if (info.type == type) return &info;
  }
  return nullptr;
}

} // namespace AI
