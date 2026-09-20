#include "ollama-ai-provider.hpp"
#include <memory>
#include <ranges>
#include <span>
#include <qlogging.h>
#include <qpromise.h>
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace {

std::optional<Capability> parseCapability(std::string_view str) {
  if (str == "completion") { return Capability::Completion; }
  if (str == "tools") { return Capability::ToolCalling; }
  if (str == "vision") { return Capability::Vision; }
  if (str == "thinking") { return Capability::Thinking; }
  if (str == "embedding") { return Capability::Embedding; }
  // ollama doesn't support transcription models
  return std::nullopt;
}

Capabilities parseCapabilities(std::span<const std::string> strs) {
  Capabilities caps{};

  for (const auto &str : strs) {
    if (auto cap = parseCapability(str)) { caps |= cap.value(); }
  }

  return caps;
}

ImageUrl modelIcon(std::string_view name, std::string_view family) {
  if (name.contains("mistral")) return ImageUrl{BuiltinIcon::Mistral};
  if (name.contains("llava")) return ImageUrl{BuiltinIcon::Llava};
  if (name.contains("deepseek")) return ImageUrl{BuiltinIcon::Deepseek};
  if (name.contains("gemma")) return ImageUrl{BuiltinIcon::Google};
  if (family.starts_with("qwen")) return ImageUrl{BuiltinIcon::Qwen};

  return ImageUrl{BuiltinIcon::Ollama};
}

QFuture<Result<ModelList>> listModelsWithCapabilities(http::Client client) {
  using namespace ollama;

  auto promise = std::make_shared<QPromise<Result<ModelList>>>();

  client.get<ListModelsResponse>("tags").then([promise, client](Result<ListModelsResponse> result) mutable {
    if (!result) {
      promise->addResult(std::unexpected(result.error()));
      promise->finish();
      return;
    }

    auto modelList = std::move(*result);
    std::vector<QFuture<Result<ModelShowResponse>>> futures;
    futures.reserve(modelList.models.size());

    for (const auto &model : modelList.models) {
      futures.emplace_back(client.post<ModelShowResponse, ModelShowRequest>("show", {.model = model.model}));
    }

    QtFuture::whenAll(futures.begin(), futures.end())
        .then([modelList = std::move(modelList),
               promise](QList<QFuture<Result<ModelShowResponse>>> completed) mutable {
          ModelList models;
          models.reserve(completed.size());

          for (auto [model, future] : std::views::zip(modelList.models, completed)) {
            if (future.isCanceled() || !future.isResultReadyAt(0)) continue;

            auto const &showResult = future.result();
            if (!showResult) {
              qWarning() << "Failed to fetch model info for" << model.model << ":" << showResult.error();
              continue;
            }

            models.emplace_back(Model{
                .id = model.model,
                .name = model.name,
                .icon = modelIcon(model.name, model.details.family),
                .caps = parseCapabilities(showResult->capabilities),
            });
          }

          promise->addResult(std::move(models));
          promise->finish();
        });
  });

  return promise->future();
}

} // namespace

OllamaProvider::OllamaProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "ollama", "http://localhost:11434") {}

QFuture<Result<ModelList>> OllamaProvider::fetchModels() {
  m_native.setBaseUrl(QString::fromStdString(url() + "/api/"));

  return m_native.get<ollama::VersionResponse>("version")
      .then([client = m_native,
             url = url()](Result<ollama::VersionResponse> res) -> QFuture<Result<ModelList>> {
        if (!res) {
          qWarning() << "handshake with ollama failed" << res.error();
          return QtFuture::makeReadyValueFuture<Result<ModelList>>(std::unexpected(res.error()));
        }

        qInfo().nospace() << "Connected to ollama instance " << url << " (version=" << res->version << ")";
        return listModelsWithCapabilities(client);
      })
      .unwrap();
}

} // namespace AI
