#include "generated/tsapi.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-preferences.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <QTimer>
#include <format>
#include <ranges>

class ExtensionAIRouter : public tsapi::AbstractAI {
public:
  ExtensionAIRouter(tsapi::RpcTransport &transport, AI::Service &ai) : AbstractAI(transport), m_ai(ai) {}

  ~ExtensionAIRouter() override = default;

  tsapi::Result<std::vector<tsapi::Model>>::Future listModels() override {
    auto models = m_ai.listModels(AI::Capability::Completion) |
                  std::views::transform([](const AI::ProviderModel &model) {
                    return tsapi::Model{.id = model.ref.toString(),
                                        .name = model.name,
                                        .capabilities = AI::stringifyCapabilities(model.caps)};
                  }) |
                  std::ranges::to<std::vector>();

    return tsapi::Result<std::vector<tsapi::Model>>::ok(models);
  }

  tsapi::Result<std::string>::Future ask(std::string prompt, tsapi::AskOptions options) override {
    AI::ChatCompletionPayload payload{.thinking = AI::ThinkingMode::Low, .oneShot = true};
    std::optional<AI::ModelRef> model;

    payload.messages = {AI::ChatMessage::fromText(AI::ChatRole::User, std::move(prompt))};
    payload.temperature = options.temperature.value_or(0.0);

    if (options.model) {
      auto ref = AI::ModelRef::fromString(*options.model);

      if (!ref) {
        return tsapi::Result<std::string>::fail(
            std::format("Model {} does not refer to a valid model. Use AI.getModels to list all currently "
                        "available models.",
                        *options.model));
      }

      model = std::move(*ref);
    } else {
      model = AI::preferredChatModel(*ServiceRegistry::instance()->config());

      if (!model || !m_ai.isAvailable(*model, AI::Capability::Completion)) {
        return tsapi::Result<std::string>::fail(
            "No AI model is selected. Pick one in the AI settings or AI Chat, or pass one explicitly.");
      }
    }

    auto completion = m_ai.createChatCompletion(model, payload);

    if (!completion) {
      return tsapi::Result<std::string>::fail(
          std::format("Model {} is not available. Use AI.getModels to list all currently available models.",
                      model->toString()));
    }

    auto const completionId = generateNextCompletionId();

    connect(completion.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
            [this, completionId](const std::string &data) { emitdataReceived(completionId, data, false); });
    connect(completion.get(), &AI::AbstractChatCompletionStream::finished, this, [this, completionId]() {
      emitdataReceived(completionId, "", true);
      release(completionId);
    });
    connect(completion.get(), &AI::AbstractChatCompletionStream::errorOccurred, this,
            [this, completionId](const std::string &error) {
              emitcompletionFailed(completionId, error);
              release(completionId);
            });

    m_completions.insert({completionId, completion});

    if (!completion->start()) {
      release(completionId);
      return tsapi::Result<std::string>::fail(
          std::format("Could not start a completion with {}.", model->toString()));
    }

    return tsapi::Result<std::string>::ok(completionId);
  }

  tsapi::Result<bool>::Future abort(std::string completionId) override {
    if (auto it = m_completions.find(completionId); it != m_completions.end()) {
      m_completions.erase(it);
      return tsapi::Result<bool>::ok(true);
    }
    return tsapi::Result<bool>::ok(false);
  }

private:
  // Runs from the stream's own signals, which can't destroy it before they return.
  void release(const std::string &completionId) {
    const auto it = m_completions.find(completionId);
    if (it == m_completions.end()) return;
    QTimer::singleShot(0, this, [stream = std::move(it->second)] {});
    m_completions.erase(it);
  }

  std::string generateNextCompletionId() { return std::format("completion-{}", m_completionIdx++); }

  int m_completionIdx = 0;
  AI::Service &m_ai;
  std::unordered_map<std::string, std::shared_ptr<AI::AbstractChatCompletionStream>> m_completions;
};
