#pragma once
#include <optional>
#include <string>
#include <vector>
#include <qfuture.h>
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

namespace openrouter {

struct ListModelsResponse {
  struct Model {
    struct Architecture {
      std::vector<std::string> input_modalities;
      std::vector<std::string> output_modalities;
    };

    std::string id;
    std::string name;
    std::optional<std::string> description;
    Architecture architecture;
    std::vector<std::string> supported_parameters;
  };

  std::vector<Model> data;
};

} // namespace openrouter

class OpenRouterProvider : public OpenAICompatibleProvider {
public:
  explicit OpenRouterProvider(std::string id);

protected:
  QFuture<Result<ModelList>> fetchModels() override;
};

} // namespace AI
