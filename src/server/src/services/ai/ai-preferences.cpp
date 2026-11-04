#include "ai-preferences.hpp"
#include "builtins/ai/ai-model-selector-utils.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"

namespace AI {

const glz::generic *findPreference(const config::Manager &config, std::string_view key) {
  const auto &providers = config.value().providers;
  const auto it = providers.find(std::string(EXTENSION_ID));
  if (it == providers.end() || !it->second.preferences) return nullptr;
  return preferences::find(*it->second.preferences, key);
}

std::optional<ModelRef> defaultModel(const config::Manager &config) {
  const auto *value = findPreference(config, DEFAULT_MODEL_PREFERENCE);
  if (!value || !value->is_string()) return std::nullopt;
  auto ref = ModelRef::fromString(value->get_string());
  if (!ref) return std::nullopt;
  return std::move(*ref);
}

void setDefaultModel(config::Manager &config, const ModelRef &model) {
  config.updateUser([&](config::Partial<config::ConfigValue> &user) {
    if (!user.providers) user.providers.emplace();
    auto &extension = (*user.providers)[std::string(EXTENSION_ID)];
    if (!extension.preferences) extension.preferences.emplace();
    (*extension.preferences)[std::string(DEFAULT_MODEL_PREFERENCE)] = model.toString();
  });
}

std::vector<::Preference::DropdownData::Section> defaultModelSections() {
  auto sections = buildModelDropdownSections(ServiceRegistry::instance()->ai(), Capability::Completion);
  sections.insert(
      sections.begin(),
      ::Preference::DropdownData::Section{
          .options = {::Preference::DropdownData::Option{
              .title = QCoreApplication::translate("AiPreferences", "None"),
              .value = QString::fromUtf8(NO_MODEL.data(), static_cast<qsizetype>(NO_MODEL.size())),
          }}});
  return sections;
}

} // namespace AI
