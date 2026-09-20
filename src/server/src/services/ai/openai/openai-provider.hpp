#pragma once
#include <string>
#include <string_view>
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

class OpenAIProvider : public OpenAICompatibleProvider {
public:
  explicit OpenAIProvider(std::string id);

protected:
  Capabilities capabilitiesFor(std::string_view id) const override;
  openai::ChatRequest makeRequest(std::string_view modelId, const Model &model,
                                  const ChatCompletionPayload &payload) const override;
};

} // namespace AI
