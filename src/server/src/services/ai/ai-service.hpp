#include <algorithm>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include "ai-provider.hpp"

namespace AI {
class Service : public QObject {
  Q_OBJECT

signals:
  void modelsChanged();

public:
  ~Service() override = default;

  void registerProvider(std::unique_ptr<AI::AbstractProvider> provider) {
    connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
    m_providers.emplace_back(std::move(provider));
  }

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const {
    for (const auto &provider : m_providers) {
      if (const auto model = provider->findBestModel(AI::Capability::Chat)) {
        return provider->createChatCompletion({.messages = payload.messages});
      }
    }

    return nullptr;
  }

  AbstractProvider *getProviderById(std::string_view id) {
    if (auto it = std::ranges::find_if(m_providers, [&](auto &&provider) { return provider->id() == id; });
        it != m_providers.end()) {
      return it->get();
    }

    return nullptr;
  }

  const auto &providers() const { return m_providers; }

  std::vector<AI::ProviderModel> listModels() {
    std::vector<AI::ProviderModel> models;

    models.reserve(m_providers.size() * 50);

    for (const auto &provider : m_providers) {
      for (auto &model : provider->listModels()) {
        ProviderModel pmodel(provider->id(), std::move(model));
        models.emplace_back(pmodel);
      }
    }

    return models;
  }

private:
  std::vector<std::unique_ptr<AI::AbstractProvider>> m_providers;
};
}; // namespace AI
