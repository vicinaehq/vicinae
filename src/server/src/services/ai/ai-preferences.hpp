#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <string_view>
#include <glaze/json/generic.hpp>
#include <QCoreApplication>
#include "command/preference-schema.hpp"
#include "services/ai/ai-provider.hpp"

namespace config {
class Manager;
}

namespace AI {

constexpr auto EXTENSION_ID = std::string_view("ai");
constexpr auto PROVIDERS_PREFERENCE = std::string_view("providers");
constexpr auto DEFAULT_MODEL_PREFERENCE = std::string_view("defaultModel");
constexpr auto NO_MODEL = std::string_view("none");

struct ProviderInstance {
  std::string type;
  glz::generic::object_t fields;
};

/**
 * The value the user set for a preference of the AI extension, if any.
 */
const glz::generic *findPreference(const config::Manager &config, std::string_view key);

/**
 * The model new conversations start with in AI Chat and Quick AI, which is also what extensions get when
 * they don't ask for one. Whether it can currently be used is up to the caller to check.
 */
std::optional<ModelRef> defaultModel(const config::Manager &config);
void setDefaultModel(config::Manager &config, const ModelRef &model);
std::vector<::Preference::DropdownData::Section> defaultModelSections();

} // namespace AI

struct AiPreferences {
  std::map<std::string, AI::ProviderInstance> providers;
  std::string defaultModel = std::string(AI::NO_MODEL);
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
  PreferenceMeta defaultModel{
      .title = tr("Default model"),
      .description = tr("The model new conversations start with in AI Chat and Quick AI, also used by "
                        "extensions that don't ask for a specific one."),
      .required = false,
      .sections = AI::defaultModelSections,
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
