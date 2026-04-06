#include "generated/tsapi.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <format>
#include <ranges>

class ExtensionAIRouter : public tsapi::AbstractAI {
public:
  ExtensionAIRouter(tsapi::RpcTransport &transport, AI::Service &ai) : AbstractAI(transport), m_ai(ai) {}

  ~ExtensionAIRouter() override = default;

  tsapi::Result<std::vector<tsapi::Model>>::Future listModels() override {
    auto models = m_ai.listModels() | std::views::transform([](const AI::Model &model) {
                    return tsapi::Model{.id = model.id,
                                        .name = model.name,
                                        .capabilities = AI::stringifyCapabilities(model.caps)};
                  }) |
                  std::ranges::to<std::vector>();

    return tsapi::Result<std::vector<tsapi::Model>>::ok(models);
  }

  tsapi::Result<std::string>::Future ask(std::string prompt, tsapi::AskOptions options) override {
    AI::ChatCompletionPayload payload;
    std::optional<AI::ModelRef> model;

    payload.messages = {AI::ChatMessage(AI::ChatRole::User, prompt)};
    payload.temperature = options.temperature.value_or(0.0);

    if (options.model) {
      auto const ref = AI::ModelRef::fromString(*options.model);

      if (!ref) {
        return tsapi::Result<std::string>::fail(
            std::format("Model {} does not refer to a valid model. Use AI.listModels to list all currently "
                        "available models.",
                        *options.model));
      }

      model = std::move(ref).value();
    }

    auto completion = m_ai.createChatCompletion(model, payload);
    auto const completionId = generateNextCompletionId();

    connect(completion.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
            [this, completionId](const std::string &data) { emitdataReceived(completionId, data, false); });
    connect(completion.get(), &AI::AbstractChatCompletionStream::finished, this, [this, completionId]() {
      emitdataReceived(completionId, "", true);
      m_completions.erase(completionId);
    });

    completion->start();
    m_completions.insert({completionId, completion});

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
  std::string generateNextCompletionId() { return std::format("completion-{}", m_completionIdx++); }

  int m_completionIdx = 0;
  AI::Service &m_ai;
  std::unordered_map<std::string, std::shared_ptr<AI::AbstractChatCompletionStream>> m_completions;
};
