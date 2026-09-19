#pragma once
#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include <qcoreapplication.h>
#include "services/builtin-icon/builtin-icon.hpp"

namespace AI {

enum class FieldKind : std::uint8_t { Text, Secret, Select, Toggle };

// Builtin providers are constructed by the application, never added or removed through config.
enum class Cardinality : std::uint8_t { Builtin, Single, Multiple };

struct FieldOption {
  std::string_view value;
  const char *label;
};

struct ProviderField {
  std::string_view key;
  const char *label;
  const char *description;
  FieldKind kind = FieldKind::Text;
  std::string_view placeholder;
  std::string_view defaultValue;
  bool defaultChecked = false;
  std::span<const FieldOption> options;
};

struct ProviderTypeInfo {
  std::string_view type;
  std::string_view label;
  BuiltinIcon icon;
  std::string_view description;
  Cardinality cardinality;
  std::span<const ProviderField> fields;
};

constexpr auto PROVIDER_TR_CONTEXT = "AIProviderTypes";
#define AI_PROVIDER_TR(text) QT_TRANSLATE_NOOP("AIProviderTypes", text)

inline constexpr auto OLLAMA_FIELDS = std::to_array<ProviderField>({
    {.key = "url",
     .label = AI_PROVIDER_TR("Server URL"),
     .description = AI_PROVIDER_TR("The address of your Ollama instance."),
     .placeholder = "http://localhost:11434",
     .defaultValue = "http://localhost:11434"},
});

inline constexpr auto MISTRAL_FIELDS = std::to_array<ProviderField>({
    {.key = "apiKey",
     .label = AI_PROVIDER_TR("API Key"),
     .description = AI_PROVIDER_TR("Your Mistral AI API key. You can find it in your Mistral dashboard."),
     .kind = FieldKind::Secret,
     .placeholder = "sk-..."},
});

inline constexpr auto LOCAL_KEEP_LOADED_OPTIONS = std::to_array<FieldOption>({
    {.value = "0", .label = AI_PROVIDER_TR("Unload immediately")},
    {.value = "60", .label = AI_PROVIDER_TR("1 minute")},
    {.value = "300", .label = AI_PROVIDER_TR("5 minutes")},
    {.value = "900", .label = AI_PROVIDER_TR("15 minutes")},
    {.value = "always", .label = AI_PROVIDER_TR("Until Vicinae quits")},
});

inline constexpr auto LOCAL_FIELDS = std::to_array<ProviderField>({
    {.key = "keepLoaded",
     .label = AI_PROVIDER_TR("Keep models loaded"),
     .description = AI_PROVIDER_TR("How long a model stays in memory after its last use."),
     .kind = FieldKind::Select,
     .defaultValue = "60",
     .options = LOCAL_KEEP_LOADED_OPTIONS},
});

inline constexpr auto PROVIDER_TYPES = std::to_array<ProviderTypeInfo>({
    {
        .type = "local",
        .label = "Built-in",
        .icon = BuiltinIcon::Vicinae,
        .description = "Vicinae-managed models, mostly for dictation purposes.",
        .cardinality = Cardinality::Builtin,
        .fields = LOCAL_FIELDS,
    },
    {
        .type = "ollama",
        .label = "Ollama",
        .icon = BuiltinIcon::Ollama,
        .description = "Connect to a local or remote Ollama instance.",
        .cardinality = Cardinality::Multiple,
        .fields = OLLAMA_FIELDS,
    },
    {
        .type = "mistral",
        .label = "Mistral",
        .icon = BuiltinIcon::Mistral,
        .description = "Mistral AI cloud API. Provides transcription and language models.",
        .cardinality = Cardinality::Single,
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
