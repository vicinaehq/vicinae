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

std::optional<ModelRef> preferredChatModel(const config::Manager &config) {
  const auto *value = findPreference(config, CHAT_MODEL_PREFERENCE);
  if (!value || !value->is_string()) return std::nullopt;
  auto ref = ModelRef::fromString(value->get_string());
  if (!ref) return std::nullopt;
  return std::move(*ref);
}

void setPreferredChatModel(config::Manager &config, const ModelRef &model) {
  config.updateUser([&](config::Partial<config::ConfigValue> &user) {
    if (!user.providers) user.providers.emplace();
    auto &extension = (*user.providers)[std::string(EXTENSION_ID)];
    if (!extension.preferences) extension.preferences.emplace();
    (*extension.preferences)[std::string(CHAT_MODEL_PREFERENCE)] = model.toString();
  });
}

std::vector<::Preference::DropdownData::Section> chatModelSections() {
  return buildModelDropdownSections(ServiceRegistry::instance()->ai(), Capability::Completion);
}

} // namespace AI
