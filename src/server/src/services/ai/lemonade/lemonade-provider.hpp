#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <qfuture.h>
#include "services/ai/ai-provider.hpp"
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

namespace lemonade {

struct ListModelsResponse {
  struct Entry {
    std::string id;
    std::vector<std::string> labels;
    bool downloaded = false;
  };

  std::vector<Entry> data;
};

} // namespace lemonade

class LemonadeProvider : public OpenAICompatibleProvider {
public:
  explicit LemonadeProvider(std::string id);

protected:
  std::string openAIRoot(std::string_view url) const override { return std::string(url) + "/v1"; }
  QFuture<Result<ModelList>> fetchModels() override;
};

} // namespace AI
