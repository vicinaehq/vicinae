#include "lemonade-provider.hpp"
#include <ranges>
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace {

Capabilities capabilityFor(std::string_view label) {
  if (label == "chat") return Capability::Completion;
  if (label == "tool-calling") return Capability::ToolCalling;
  if (label == "vision") return Capability::Vision;
  if (label == "reasoning") return Capability::Thinking;
  if (label == "transcription") return Capability::Transcription;
  if (label == "embeddings" || label == "embedding") return Capability::Embedding;
  return 0;
}

Model toModel(const lemonade::ListModelsResponse::Entry &entry) {
  Model model;
  model.id = entry.id;
  model.name = entry.id;
  model.icon = ImageUrl{BuiltinIcon::Lemonade};
  for (const auto &label : entry.labels)
    model.caps |= capabilityFor(label);
  return model;
}

} // namespace

LemonadeProvider::LemonadeProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "lemonade", "http://localhost:13305") {}

QFuture<Result<ModelList>> LemonadeProvider::fetchModels() {
  return client().get<lemonade::ListModelsResponse>("models").then(
      [](Result<lemonade::ListModelsResponse> res) -> Result<ModelList> {
        if (!res) return std::unexpected(res.error());
        return res->data | std::views::filter([](const auto &entry) { return entry.downloaded; }) |
               std::views::transform(toModel) |
               std::views::filter([](const Model &m) { return m.caps != 0; }) | std::ranges::to<ModelList>();
      });
}

} // namespace AI
