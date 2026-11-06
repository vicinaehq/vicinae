#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <qfuture.h>
#include "services/ai/ai-provider.hpp"
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

namespace llamacpp {

struct ListModelsResponse {
  struct Entry {
    struct Architecture {
      std::vector<std::string> input_modalities;
      std::vector<std::string> output_modalities;
    };

    std::string id;
    std::optional<Architecture> architecture;
  };

  std::vector<Entry> data;
};

} // namespace llamacpp

class LlamaCppProvider : public OpenAICompatibleProvider {
public:
  explicit LlamaCppProvider(std::string id);

protected:
  std::string openAIRoot(std::string_view url) const override { return std::string(url) + "/v1"; }
  QFuture<Result<ModelList>> fetchModels() override;
};

} // namespace AI
