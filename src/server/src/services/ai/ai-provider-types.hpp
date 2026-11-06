#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <QCoreApplication>
#include "command/preference-schema.hpp"
#include "services/builtin-icon/builtin-icon.hpp"

namespace AI {

// Builtin providers are constructed by the application, never added or removed through config.
enum class Cardinality : std::uint8_t { Builtin, Single, Multiple };

struct OllamaFields {
  std::string name;
  std::string url = "http://localhost:11434";
};

struct OpenAIFields {
  std::string apiKey;
};

struct GroqFields {
  std::string apiKey;
};

struct OpenRouterFields {
  std::string apiKey;
};

struct MistralFields {
  std::string apiKey;
};

struct OpenAICompatibleFields {
  std::string name;
  std::string url;
  std::string apiKey;
};

struct ClaudeCodeFields {
  std::string executable;
};

struct CodexFields {
  std::string executable;
};

enum class KeepLoaded { Immediately, OneMinute, FiveMinutes, FifteenMinutes, Always };
enum class Device { Gpu, Cpu };

struct LocalFields {
  KeepLoaded keepLoaded = KeepLoaded::OneMinute;
  Device device = Device::Gpu;
};

enum class Category : std::uint8_t { Builtin, Cloud, SelfHosted, Cli };

struct ProviderTypeInfo {
  std::string_view type;
  std::string_view label;
  BuiltinIcon icon;
  std::string_view description;
  Cardinality cardinality;
  Category category;
  std::vector<::Preference> (*fields)();
};

} // namespace AI

template <> struct glz::meta<AI::KeepLoaded> {
  using enum AI::KeepLoaded;
  static constexpr auto value = glz::enumerate("0", Immediately, "60", OneMinute, "300", FiveMinutes, "900",
                                               FifteenMinutes, "always", Always);
};

template <> struct glz::meta<AI::Device> {
  using enum AI::Device;
  static constexpr auto value = glz::enumerate("gpu", Gpu, "cpu", Cpu);
};

template <> struct PreferenceSchema<AI::OllamaFields> {
  PreferenceMeta name{
      .title = tr("Name"),
      .description = tr("How this instance is shown in model lists."),
      .placeholder = "Ollama",
  };
  PreferenceMeta url{
      .title = tr("Server URL"),
      .description = tr("The address of your Ollama instance."),
      .placeholder = "http://localhost:11434",
  };
  Q_DECLARE_TR_FUNCTIONS(OllamaFields)
};

template <> struct PreferenceSchema<AI::OpenAIFields> {
  PreferenceMeta apiKey{
      .title = tr("API Key"),
      .description = tr("Your OpenAI API key. You can create one in the OpenAI platform dashboard."),
      .placeholder = "sk-...",
      .kind = PreferenceMeta::Kind::Password,
  };
  Q_DECLARE_TR_FUNCTIONS(OpenAIFields)
};

template <> struct PreferenceSchema<AI::GroqFields> {
  PreferenceMeta apiKey{
      .title = tr("API Key"),
      .description = tr("Your Groq API key. You can create one in the Groq console."),
      .placeholder = "gsk_...",
      .kind = PreferenceMeta::Kind::Password,
  };
  Q_DECLARE_TR_FUNCTIONS(GroqFields)
};

template <> struct PreferenceSchema<AI::OpenRouterFields> {
  PreferenceMeta apiKey{
      .title = tr("API Key"),
      .description = tr("Your OpenRouter API key. You can create one in your OpenRouter settings."),
      .placeholder = "sk-or-...",
      .kind = PreferenceMeta::Kind::Password,
  };
  Q_DECLARE_TR_FUNCTIONS(OpenRouterFields)
};

template <> struct PreferenceSchema<AI::MistralFields> {
  PreferenceMeta apiKey{
      .title = tr("API Key"),
      .description = tr("Your Mistral AI API key. You can find it in your Mistral dashboard."),
      .placeholder = "sk-...",
      .kind = PreferenceMeta::Kind::Password,
  };
  Q_DECLARE_TR_FUNCTIONS(MistralFields)
};

template <> struct PreferenceSchema<AI::OpenAICompatibleFields> {
  PreferenceMeta name{
      .title = tr("Name"),
      .description = tr("How this server is shown in model lists."),
      .placeholder = "My server",
  };
  PreferenceMeta url{
      .title = tr("Server URL"),
      .description =
          tr("Base URL of the OpenAI-compatible API, including the version prefix if the server has one."),
      .placeholder = "http://localhost:8080/v1",
  };
  PreferenceMeta apiKey{
      .title = tr("API Key"),
      .description = tr("Leave empty if the server does not require one."),
      .placeholder = "sk-...",
      .kind = PreferenceMeta::Kind::Password,
      .required = false,
  };
  Q_DECLARE_TR_FUNCTIONS(OpenAICompatibleFields)
};

template <> struct PreferenceSchema<AI::ClaudeCodeFields> {
  PreferenceMeta executable{
      .title = tr("Executable"),
      .description = tr("Path to the claude executable. Leave empty to find it automatically."),
      .placeholder = "claude",
      .required = false,
  };
  Q_DECLARE_TR_FUNCTIONS(ClaudeCodeFields)
};

template <> struct PreferenceSchema<AI::CodexFields> {
  PreferenceMeta executable{
      .title = tr("Executable"),
      .description = tr("Path to the codex executable. Leave empty to find it automatically."),
      .placeholder = "codex",
      .required = false,
  };
  Q_DECLARE_TR_FUNCTIONS(CodexFields)
};

template <> struct PreferenceSchema<AI::LocalFields> {
  PreferenceMeta keepLoaded{
      .title = tr("Keep models loaded"),
      .description = tr("How long a model stays in memory after its last use."),
      .options =
          [] {
            using AI::KeepLoaded;
            return std::vector<Preference::DropdownData::Option>{
                option(KeepLoaded::Immediately, tr("Unload immediately")),
                option(KeepLoaded::OneMinute, tr("1 minute")),
                option(KeepLoaded::FiveMinutes, tr("5 minutes")),
                option(KeepLoaded::FifteenMinutes, tr("15 minutes")),
                option(KeepLoaded::Always, tr("Until Vicinae quits")),
            };
          },
  };
  PreferenceMeta device{
      .title = tr("Run models on"),
      .description = tr("Falls back to the CPU when no compatible GPU is available."),
      .options =
          [] {
            using AI::Device;
            return std::vector<Preference::DropdownData::Option>{
                option(Device::Gpu, tr("GPU")),
                option(Device::Cpu, tr("CPU")),
            };
          },
  };
  Q_DECLARE_TR_FUNCTIONS(LocalFields)
};

namespace AI {

inline constexpr auto PROVIDER_TYPES = std::to_array<ProviderTypeInfo>({
    {
        .type = "local",
        .label = "Built-in",
        .icon = BuiltinIcon::Vicinae,
        .description = "Vicinae-managed models, mostly for dictation purposes.",
        .cardinality = Cardinality::Builtin,
        .category = Category::Builtin,
        .fields = +[] { return describePreferences<LocalFields>(); },
    },
    {
        .type = "ollama",
        .label = "Ollama",
        .icon = BuiltinIcon::Ollama,
        .description = "Connect to a local or remote Ollama instance.",
        .cardinality = Cardinality::Multiple,
        .category = Category::SelfHosted,
        .fields = +[] { return describePreferences<OllamaFields>(); },
    },
    {
        .type = "openai",
        .label = "OpenAI",
        .icon = BuiltinIcon::Openai,
        .description = "OpenAI cloud API. Provides language and transcription models.",
        .cardinality = Cardinality::Single,
        .category = Category::Cloud,
        .fields = +[] { return describePreferences<OpenAIFields>(); },
    },
    {
        .type = "groq",
        .label = "Groq",
        .icon = BuiltinIcon::Groq,
        .description = "Groq cloud API. Fast open-weight language models and Whisper transcription.",
        .cardinality = Cardinality::Single,
        .category = Category::Cloud,
        .fields = +[] { return describePreferences<GroqFields>(); },
    },
    {
        .type = "openrouter",
        .label = "OpenRouter",
        .icon = BuiltinIcon::Openrouter,
        .description = "OpenRouter cloud API. One key for models from every major vendor.",
        .cardinality = Cardinality::Single,
        .category = Category::Cloud,
        .fields = +[] { return describePreferences<OpenRouterFields>(); },
    },
    {
        .type = "mistral",
        .label = "Mistral",
        .icon = BuiltinIcon::Mistral,
        .description = "Mistral AI cloud API. Provides transcription and language models.",
        .cardinality = Cardinality::Single,
        .category = Category::Cloud,
        .fields = +[] { return describePreferences<MistralFields>(); },
    },
    {
        .type = "claude-code",
        .label = "Claude Code",
        .icon = BuiltinIcon::ClaudeCode,
        .description = "Chat with Claude through your Claude Code sign-in. Requires the claude CLI.",
        .cardinality = Cardinality::Single,
        .category = Category::Cli,
        .fields = +[] { return describePreferences<ClaudeCodeFields>(); },
    },
    {
        .type = "codex",
        .label = "Codex",
        .icon = BuiltinIcon::Codex,
        .description = "Chat with OpenAI models through your Codex sign-in. Requires the codex CLI.",
        .cardinality = Cardinality::Single,
        .category = Category::Cli,
        .fields = +[] { return describePreferences<CodexFields>(); },
    },
    {
        .type = "openai-compatible",
        .label = "OpenAI-compatible",
        .icon = BuiltinIcon::Network,
        .description =
            "Any server exposing the OpenAI chat completions API, such as LM Studio, vLLM or llama.cpp.",
        .cardinality = Cardinality::Multiple,
        .category = Category::SelfHosted,
        .fields = +[] { return describePreferences<OpenAICompatibleFields>(); },
    },
});

inline const ProviderTypeInfo *findProviderType(std::string_view type) {
  for (const auto &info : PROVIDER_TYPES) {
    if (info.type == type) return &info;
  }
  return nullptr;
}

} // namespace AI
