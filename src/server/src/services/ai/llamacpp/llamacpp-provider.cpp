#include "llamacpp-provider.hpp"
#include <algorithm>
#include <ranges>
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace {

// Without --alias the id is the GGUF path the server was started with.
std::string displayName(std::string_view id) {
  std::string_view name = id;
  if (const auto pos = name.find_last_of("/\\"); pos != std::string_view::npos) name.remove_prefix(pos + 1);
  if (name.ends_with(".gguf")) name.remove_suffix(5);
  return std::string(name.empty() ? id : name);
}

Model toModel(const llamacpp::ListModelsResponse::Entry &entry) {
  Model model;
  model.id = entry.id;
  model.name = displayName(entry.id);
  model.icon = ImageUrl{BuiltinIcon::LlamaCpp};
  model.caps = Capability::Completion | Capability::ToolCalling;
  if (entry.architecture && std::ranges::contains(entry.architecture->input_modalities, "image")) {
    model.caps |= Capability::Vision;
  }
  return model;
}

} // namespace

LlamaCppProvider::LlamaCppProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "llama-cpp", "http://localhost:8080") {}

QFuture<Result<ModelList>> LlamaCppProvider::fetchModels() {
  return client().get<llamacpp::ListModelsResponse>("models").then(
      [](Result<llamacpp::ListModelsResponse> res) -> Result<ModelList> {
        if (!res) return std::unexpected(res.error());
        return res->data | std::views::transform(toModel) | std::ranges::to<ModelList>();
      });
}

} // namespace AI
