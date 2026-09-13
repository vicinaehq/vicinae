#include "ai-service.hpp"
#include <memory>
#include <qstring.h>
#include <type_traits>
#include <variant>
#include "services/ai/mistral/mistral-provider.hpp"
#include "services/ai/ollama/ollama-ai-provider.hpp"

namespace AI {

std::unique_ptr<AbstractProvider> Service::createProvider(const ConfigValue::ProviderConfig &config) {
  return std::visit(
      [](const auto &cfg) -> std::unique_ptr<AbstractProvider> {
        using T = std::decay_t<decltype(cfg)>;
        if constexpr (std::is_same_v<T, ConfigValue::OllamaConfig>) {
          return std::make_unique<OllamaProvider>(cfg);
        } else {
          static_assert(std::is_same_v<T, ConfigValue::MistralConfig>);
          return std::make_unique<MistralProvider>(QString::fromStdString(cfg.apiKey));
        }
      },
      config);
}

} // namespace AI
