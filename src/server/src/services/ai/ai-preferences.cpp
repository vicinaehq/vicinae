#include "ai-preferences.hpp"
#include "config/config.hpp"

namespace AI {

const glz::generic *findPreference(const config::Manager &config, std::string_view key) {
  const auto &providers = config.value().providers;
  const auto it = providers.find(std::string(EXTENSION_ID));
  if (it == providers.end() || !it->second.preferences) return nullptr;
  return preferences::find(*it->second.preferences, key);
}

} // namespace AI
