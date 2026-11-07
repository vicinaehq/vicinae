#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <qfuture.h>
#include "services/ai/ai-provider.hpp"
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

namespace audiocpp {

struct ListModelsResponse {
  struct Entry {
    std::string id;
  };

  std::vector<Entry> data;
};

} // namespace audiocpp

class AudioCppProvider : public OpenAICompatibleProvider {
public:
  explicit AudioCppProvider(std::string id);

protected:
  std::string openAIRoot(std::string_view url) const override { return std::string(url) + "/v1"; }
  QFuture<Result<ModelList>> fetchModels() override;
  void addTranscriptionFields(http::FormData &form, std::string_view modelId,
                              const TranscriptionOptions &options) const override;
};

} // namespace AI
