#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <qfuture.h>
#include "internal/http-client.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

namespace ollama {

struct ModelShowResponse {
  std::vector<std::string> capabilities;
};

struct VersionResponse {
  std::string version;
};

struct ListModelsResponse {
  struct Model {
    struct Details {
      std::string family;
    };

    std::string name;
    std::string model;
    std::uint64_t size = 0;
    Details details;
  };

  std::vector<Model> models;
};

struct ModelShowRequest {
  std::string model;
  bool verbose = false;
};

} // namespace ollama

class OllamaProvider : public OpenAICompatibleProvider {
public:
  explicit OllamaProvider(std::string id);

protected:
  std::string openAIRoot(std::string_view url) const override { return std::string(url) + "/v1"; }
  QFuture<Result<ModelList>> fetchModels() override;

private:
  http::Client m_native;
};

} // namespace AI
