#pragma once
#include <map>
#include <string>
#include <string_view>
#include <glaze/json/generic.hpp>
#include <QCoreApplication>
#include "command/preference-schema.hpp"

namespace config {
class Manager;
}

namespace AI {

constexpr auto EXTENSION_ID = std::string_view("ai");
constexpr auto PROVIDERS_PREFERENCE = std::string_view("providers");

struct ProviderInstance {
  std::string type;
  glz::generic::object_t fields;
};

/**
 * The value the user set for a preference of the AI extension, if any.
 */
const glz::generic *findPreference(const config::Manager &config, std::string_view key);

} // namespace AI

struct AiPreferences {
  std::map<std::string, AI::ProviderInstance> providers;
  std::string instructions;
  bool enableTools = true;
};

template <> struct PreferenceSchema<AiPreferences> {
  PreferenceMeta providers{
      .title = tr("Providers"),
      .description = tr("Local and cloud AI providers. Their models power dictation and Quick AI."),
      .component = QStringLiteral("AISettingsPage"),
      .kind = PreferenceMeta::Kind::Custom,
  };
  PreferenceMeta instructions{
      .title = tr("Custom instructions"),
      .description = tr("Tell the AI how to respond in Quick AI and AI Chat: your language, tone, how long "
                        "answers should be, or anything it should know about you."),
      .placeholder = tr("Answer in French. Keep answers short unless I ask for details."),
      .kind = PreferenceMeta::Kind::LongText,
      .required = false,
  };
  PreferenceMeta enableTools{
      .title = tr("Enable tools"),
      .description = tr("Allow AI models to use tools from enabled extensions."),
  };
  Q_DECLARE_TR_FUNCTIONS(AiPreferences)
};
