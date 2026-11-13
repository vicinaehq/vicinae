#pragma once
#include <string>
#include <string_view>
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

class GroqProvider : public OpenAICompatibleProvider {
public:
  explicit GroqProvider(std::string id);

protected:
  Capabilities capabilitiesFor(std::string_view id) const override;
};

} // namespace AI
