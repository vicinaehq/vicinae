#include "openrouter-provider.hpp"
#include <algorithm>
#include <optional>
#include <ranges>

namespace AI {

namespace {

bool has(const std::vector<std::string> &values, std::string_view value) {
  return std::ranges::find(values, value) != values.end();
}

std::string displayName(std::string name) {
  if (const auto pos = name.find(": "); pos != std::string::npos) name.erase(0, pos + 2);
  return name;
}

std::optional<Model> toModel(openrouter::ListModelsResponse::Model &&info) {
  if (!has(info.architecture.output_modalities, "text")) return std::nullopt;

  Capabilities caps = Capability::Completion;
  if (has(info.architecture.input_modalities, "image")) caps |= Capability::Vision;
  if (has(info.supported_parameters, "tools")) caps |= Capability::ToolCalling;
  if (has(info.supported_parameters, "reasoning")) caps |= Capability::Thinking;

  return Model{
      .id = std::move(info.id),
      .name = displayName(std::move(info.name)),
      .description = std::move(info.description),
      .caps = caps,
  };
}

} // namespace

OpenRouterProvider::OpenRouterProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "openrouter", "https://openrouter.ai/api/v1") {
  client().setHeader("HTTP-Referer", "https://vicinae.com");
  client().setHeader("X-Title", "Vicinae");
}

QFuture<Result<ModelList>> OpenRouterProvider::fetchModels() {
  return client().get<openrouter::ListModelsResponse>("models").then(
      [](Result<openrouter::ListModelsResponse> res) -> Result<ModelList> {
        if (!res) return std::unexpected(res.error());

        ModelList models;
        models.reserve(res->data.size());
        for (auto &info : res->data) {
          if (auto model = toModel(std::move(info))) models.emplace_back(std::move(*model));
        }
        return models;
      });
}

} // namespace AI
