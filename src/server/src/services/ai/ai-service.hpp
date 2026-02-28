#include <memory>
#include "ai-provider.hpp"

namespace AI {
class Service {
public:
  void registerProvider(std::unique_ptr<AI::AbstractProvider> provider) {
    m_providers.emplace_back(std::move(provider));
  }

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const {
    for (const auto &provider : m_providers) {
      // if (const auto model = provider->findBestModel(AI::Capability::Chat)) {
      return provider->createChatCompletion({.modelId = payload.modelId, .messages = payload.messages});
      //}
    }

    return nullptr;
  }

private:
  std::vector<std::unique_ptr<AI::AbstractProvider>> m_providers;
};
}; // namespace AI
