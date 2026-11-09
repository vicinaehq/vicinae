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

struct AgentFields {
  std::string command;
};

struct CustomAgentFields {
  std::string name;
  std::string command;
};

struct LlamaCppFields {
  std::string name;
  std::string url = "http://localhost:8080";
};

struct LemonadeFields {
  std::string name;
  std::string url = "http://localhost:13305";
  std::string apiKey;
};

struct AudioCppFields {
  std::string name;
  std::string url = "http://localhost:8080";
};

enum class KeepLoaded { Immediately, OneMinute, FiveMinutes, FifteenMinutes, Always };
enum class Device { Gpu, Cpu };

struct LocalFields {
  KeepLoaded keepLoaded = KeepLoaded::OneMinute;
  Device device = Device::Gpu;
};

enum class Category : std::uint8_t { Builtin, Cloud, SelfHosted, Agent };

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

namespace AI {
/**
 * The settings of an agent with a preset, showing the command that runs by default.
 */
std::vector<::Preference> agentFields(std::string_view type);
} // namespace AI

template <> struct PreferenceSchema<AI::AgentFields> {
  PreferenceMeta command{
      .title = tr("Command"),
      .description = tr("Command that starts the agent in ACP mode. Leave empty to find it automatically."),
      .required = false,
  };
  Q_DECLARE_TR_FUNCTIONS(AgentFields)
};

template <> struct PreferenceSchema<AI::CustomAgentFields> {
  PreferenceMeta name{
      .title = tr("Name"),
      .description = tr("How this agent is shown in model lists."),
      .placeholder = "My agent",
  };
  PreferenceMeta command{
      .title = tr("Command"),
      .description = tr("Command that starts the agent in ACP mode, with its arguments."),
      .placeholder = "my-agent --acp",
  };
  Q_DECLARE_TR_FUNCTIONS(CustomAgentFields)
};

template <> struct PreferenceSchema<AI::LlamaCppFields> {
  PreferenceMeta name{
      .title = tr("Name"),
      .description = tr("How this server is shown in model lists."),
      .placeholder = "llama.cpp",
  };
  PreferenceMeta url{
      .title = tr("Server URL"),
      .description = tr("The address of your llama-server instance."),
      .placeholder = "http://localhost:8080",
  };
  Q_DECLARE_TR_FUNCTIONS(LlamaCppFields)
};

template <> struct PreferenceSchema<AI::LemonadeFields> {
  PreferenceMeta name{
      .title = tr("Name"),
      .description = tr("How this server is shown in model lists."),
      .placeholder = "Lemonade",
  };
  PreferenceMeta url{
      .title = tr("Server URL"),
      .description = tr("The address of your Lemonade server."),
      .placeholder = "http://localhost:13305",
  };
  PreferenceMeta apiKey{
      .title = tr("API Key"),
      .description = tr("Only needed if the server was started with an API key."),
      .placeholder = "",
      .kind = PreferenceMeta::Kind::Password,
      .required = false,
  };
  Q_DECLARE_TR_FUNCTIONS(LemonadeFields)
};

template <> struct PreferenceSchema<AI::AudioCppFields> {
  PreferenceMeta name{
      .title = tr("Name"),
      .description = tr("How this server is shown in model lists."),
      .placeholder = "audio.cpp",
  };
  PreferenceMeta url{
      .title = tr("Server URL"),
      .description = tr("The address of your audio.cpp server."),
      .placeholder = "http://localhost:8080",
  };
  Q_DECLARE_TR_FUNCTIONS(AudioCppFields)
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
        .description = "Anthropic's coding agent, through your Claude Code sign-in.",
        .cardinality = Cardinality::Single,
        .category = Category::Agent,
        .fields = +[] { return agentFields("claude-code"); },
    },
    {
        .type = "codex",
        .label = "Codex",
        .icon = BuiltinIcon::Codex,
        .description = "OpenAI's coding agent, through your Codex sign-in.",
        .cardinality = Cardinality::Single,
        .category = Category::Agent,
        .fields = +[] { return agentFields("codex"); },
    },
    {
        .type = "antigravity",
        .label = "Antigravity",
        .icon = BuiltinIcon::Antigravity,
        .description = "Google's coding agent, through your Antigravity sign-in.",
        .cardinality = Cardinality::Single,
        .category = Category::Agent,
        .fields = +[] { return agentFields("antigravity"); },
    },
    {
        .type = "opencode",
        .label = "OpenCode",
        .icon = BuiltinIcon::Opencode,
        .description = "The open source coding agent.",
        .cardinality = Cardinality::Single,
        .category = Category::Agent,
        .fields = +[] { return agentFields("opencode"); },
    },
    {
        .type = "qwen-code",
        .label = "Qwen Code",
        .icon = BuiltinIcon::Qwen,
        .description = "Alibaba's coding agent, through your Qwen Code sign-in.",
        .cardinality = Cardinality::Single,
        .category = Category::Agent,
        .fields = +[] { return agentFields("qwen-code"); },
    },
    {
        .type = "kilo",
        .label = "Kilo Code",
        .icon = BuiltinIcon::Kilo,
        .description = "Kilo's open source coding agent.",
        .cardinality = Cardinality::Single,
        .category = Category::Agent,
        .fields = +[] { return agentFields("kilo"); },
    },
    {
        .type = "acp",
        .label = "Custom agent",
        .icon = BuiltinIcon::Terminal,
        .description = "Any agent that supports the Agent Client Protocol (ACP).",
        .cardinality = Cardinality::Multiple,
        .category = Category::Agent,
        .fields = +[] { return describePreferences<CustomAgentFields>(); },
    },
    {
        .type = "llama-cpp",
        .label = "llama.cpp",
        .icon = BuiltinIcon::LlamaCpp,
        .description = "A llama-server instance, local or remote. Lists the models it serves.",
        .cardinality = Cardinality::Multiple,
        .category = Category::SelfHosted,
        .fields = +[] { return describePreferences<LlamaCppFields>(); },
    },
    {
        .type = "lemonade",
        .label = "Lemonade",
        .icon = BuiltinIcon::Lemonade,
        .description =
            "AMD's local server for language and speech models. Lists the models you have downloaded.",
        .cardinality = Cardinality::Multiple,
        .category = Category::SelfHosted,
        .fields = +[] { return describePreferences<LemonadeFields>(); },
    },
    {
        .type = "audio-cpp",
        .label = "audio.cpp",
        .icon = BuiltinIcon::Waveform,
        .description = "Local speech server built on ggml. Provides transcription models.",
        .cardinality = Cardinality::Multiple,
        .category = Category::SelfHosted,
        .fields = +[] { return describePreferences<AudioCppFields>(); },
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
