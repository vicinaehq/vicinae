#include "ai-provider-registry.hpp"
#include "mistral/mistral-provider.hpp"
#include "ollama/ollama-ai-provider.hpp"

namespace AI {

ProviderRegistry makeBuiltinRegistry() {
  ProviderRegistry registry;

  registry.add("ollama", [](const ConfigValue::ProviderConfig &config) -> std::shared_ptr<AbstractProvider> {
    if (auto *cfg = std::get_if<ConfigValue::OllamaConfig>(&config))
      return std::make_shared<OllamaProvider>(*cfg);
    return nullptr;
  });

  registry.add("mistral", [](const ConfigValue::ProviderConfig &config) -> std::shared_ptr<AbstractProvider> {
    if (auto *cfg = std::get_if<ConfigValue::MistralConfig>(&config))
      return std::make_shared<MistralProvider>(QString::fromStdString(cfg->apiKey));
    return nullptr;
  });

  return registry;
}

} // namespace AI
