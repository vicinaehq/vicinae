#include "mistral-provider.hpp"
#include <ranges>

namespace AI {

namespace {

Model toModel(const mistral::ListModelsResponse::ModelInfo &info) {
  Model model;
  model.id = info.id;
  model.name = info.name;
  model.description = info.description;

  if (info.capabilities.completion_chat) { model.caps |= Capability::Completion; }
  if (info.capabilities.vision) { model.caps |= Capability::Vision; }
  if (info.capabilities.function_calling) { model.caps |= Capability::ToolCalling; }
  if (info.capabilities.audio_transcription) { model.caps |= Capability::Transcription; }
  if (info.capabilities.ocr) { model.caps |= Capability::OCR; }

  return model;
}

} // namespace

MistralProvider::MistralProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "mistral", "https://api.mistral.ai/v1") {}

QFuture<Result<ModelList>> MistralProvider::fetchModels() {
  return client().get<mistral::ListModelsResponse>("models").then(
      [](Result<mistral::ListModelsResponse> res) -> Result<ModelList> {
        if (!res) return std::unexpected(res.error());
        return res->data | std::views::transform(toModel) |
               std::views::filter([](auto &&m) { return m.id == m.name && m.caps; }) |
               std::ranges::to<ModelList>();
      });
}

} // namespace AI
