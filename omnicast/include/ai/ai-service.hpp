#pragma once
#include "ai/ai-provider.hpp"
#include <algorithm>

namespace AI {

class Manager {
  struct ManagedProvider {
    std::unique_ptr<AbstractAiProvider> provider;
    int priority;
  };

  std::vector<ManagedProvider> m_providers;

public:
  Manager() {}

  bool registerProvider(std::unique_ptr<AbstractAiProvider> provider, int priority = 10) {
    m_providers.push_back({.provider = std::move(provider), .priority = priority});
    /*
std::sort(m_providers.begin(), m_providers.end(),
          [](const ManagedProvider &a, const ManagedProvider &b) { return a.priority < b.priority; });
                      */

    return true;
  }

  std::optional<AiModel> findBestModelForTask(AiTaskType type) const {
    for (const auto &entry : m_providers) {
      if (auto model = entry.provider->findBestForTask(type)) { return model; }
    }

    return std::nullopt;
  }

  std::vector<AiModel> listModels() {
    std::vector<AiModel> models;

    models.reserve(10);

    for (const auto &entry : m_providers) {
      auto entryModels = entry.provider->listModels();

      models.insert(models.end(), entryModels.begin(), entryModels.end());
    }

    return models;
  }

  StreamedChatCompletion *createCompletion(const QString &prompt, const QString &modelId = "") {
    return createCompletion({ChatMessage(prompt)}, modelId);
  }

  StreamedChatCompletion *createCompletion(const std::vector<ChatMessage> &context,
                                           const QString &modelId = "") {
    for (const auto &entry : m_providers) {
      if (modelId.isEmpty()) {
        if (auto best = entry.provider->findBestForTask(AiTaskType::QuickReasoningTask)) {
          return entry.provider->createStreamedCompletion({.modelId = best->id, .messages = context});
        }

        continue;
      }

      for (const auto &model : entry.provider->listModels()) {
        if (model.id == modelId) {
          return entry.provider->createStreamedCompletion({.modelId = modelId, .messages = context});
        }
      }
    }

    return nullptr;
  }
};

}; // namespace AI
