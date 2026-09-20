#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <qfuture.h>
#include "services/ai/ai-provider.hpp"
#include "services/ai/openai/openai-compatible-provider.hpp"

namespace AI {

namespace mistral {

struct ListModelsResponse {
  struct ModelInfo {
    std::string id;
    std::string object; // 'model' in all instances.
    std::string name;
    std::string description;
    std::uint32_t max_context_length;

    struct {
      bool completion_chat;
      bool function_calling;
      bool completion_fim;
      bool fine_tuning;
      bool vision;
      bool ocr;
      bool classification;
      bool moderation;
      bool audio;
      bool audio_transcription;
      bool audio_transcription_realtime;
      bool audio_speech;
    } capabilities;
  };

  std::vector<ModelInfo> data;
};

} // namespace mistral

class MistralProvider : public OpenAICompatibleProvider {
public:
  explicit MistralProvider(std::string id);

protected:
  QFuture<Result<ModelList>> fetchModels() override;
};

} // namespace AI
