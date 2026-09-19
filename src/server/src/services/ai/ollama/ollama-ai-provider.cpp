#include "ollama-ai-provider.hpp"
#include <format>
#include <memory>
#include <ranges>
#include <span>
#include <glaze/json/write.hpp>
#include <qlogging.h>
#include <qnetworkrequest.h>
#include <qurl.h>
#include "common/qt.hpp"
#include "services/ai/http-completion.hpp"
#include "services/builtin-icon/builtin-icon.hpp"

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

ImageUrl modelIcon(const ollama::FullModelResponse &model) {
  if (model.name.contains("mistral")) return ImageUrl{BuiltinIcon::Mistral};
  if (model.name.contains("llava")) return ImageUrl{BuiltinIcon::Llava};
  if (model.name.contains("deepseek")) return ImageUrl{BuiltinIcon::Deepseek};
  if (model.name.contains("gemma")) return ImageUrl{BuiltinIcon::Google};
  if (model.family.starts_with("qwen")) return ImageUrl{BuiltinIcon::Qwen};

  return ImageUrl{BuiltinIcon::Ollama};
}

Model toModel(const ollama::FullModelResponse &model) {
  return Model{
      .id = model.model,
      .name = model.name,
      .icon = modelIcon(model),
      .caps = model.capabilities,
  };
}

const char *serializeRole(ChatRole role) {
  switch (role) {
  case ChatRole::User:
    return "user";
  case ChatRole::Assistant:
    return "assistant";
  case ChatRole::System:
    return "system";
  case ChatRole::Developer:
    return "developer";
  case ChatRole::Tool:
    return "tool";
  }
  return "none";
}

} // namespace

OllamaProvider::OllamaProvider() {
  connect(&m_handshakeWatcher, &decltype(m_handshakeWatcher)::finished, this, [this]() {
    auto const res = m_handshakeWatcher.result();

    if (!res) {
      qWarning() << "handshake with ollama failed" << res.error();
      return;
    }

    qInfo().nospace() << "Connected to ollama instance " << m_url << " (version=" << res->version << ")";
    m_listWatcher.setFuture(listModelsFull());
  });

  connect(&m_listWatcher, &decltype(m_listWatcher)::finished, this, [this]() {
    if (m_listWatcher.isCanceled() || !m_listWatcher.isFinished()) return;

    auto result = m_listWatcher.result();
    if (!result) {
      qWarning() << "Failed to fetch Ollama models:" << result.error();
      return;
    }

    m_models = std::move(*result);
    emit modelsUpdated();
  });
}

OllamaProvider::~OllamaProvider() { m_listWatcher.cancel(); }

void OllamaProvider::configure(const ProviderFields &fields) {
  auto url = fields.string("url");
  if (url == m_url) return;

  m_url = std::move(url);
  m_client.setBaseUrl(QString::fromStdString(std::format("{}/api", m_url)));
  qInfo() << "Ollama provider now targets" << m_url;

  if (!m_started) return;

  m_listWatcher.cancel();
  m_models.clear();
  emit modelsUpdated();
  start();
}

void OllamaProvider::start() {
  m_started = true;
  m_handshakeWatcher.setFuture(fetchVersion());
}

ModelList OllamaProvider::listModels(const ListModelFilters &) const {
  return m_models | std::views::transform(toModel) | std::ranges::to<std::vector>();
}

std::optional<Model> OllamaProvider::findBestModel(Capabilities caps, Preference) const {
  for (const auto &model : m_models) {
    if (model.capabilities & caps) { return toModel(model); }
  }

  return std::nullopt;
}

QFuture<Result<ollama::VersionResponse>> OllamaProvider::fetchVersion() {
  return m_client.get<ollama::VersionResponse>("/version");
}

QFuture<Result<ollama::ListModelsResponse>> OllamaProvider::fetchModels() {
  return m_client.get<ollama::ListModelsResponse>("/tags");
}

QFuture<OllamaProvider::ModelsResult> OllamaProvider::listModelsFull() {
  using namespace ollama;

  auto promise = std::make_shared<QPromise<ModelsResult>>();

  fetchModels().then([promise, client = m_client](Result<ListModelsResponse> result) mutable {
    if (!result) {
      promise->addResult(std::unexpected(result.error()));
      promise->finish();
      return;
    }

    auto modelList = std::move(*result);
    std::vector<QFuture<Result<ModelShowResponse>>> futures;
    futures.reserve(modelList.models.size());

    for (const auto &model : modelList.models) {
      auto future = client.post<ModelShowResponse, ModelShowRequest>("/show", {.model = model.model});
      futures.emplace_back(future);
    }

    QtFuture::whenAll(futures.begin(), futures.end())
        .then([modelList = std::move(modelList),
               promise](QList<QFuture<Result<ModelShowResponse>>> completed) mutable {
          std::vector<FullModelResponse> models;
          models.reserve(completed.size());

          for (auto [model, future] : std::views::zip(modelList.models, completed)) {
            if (future.isCanceled() || !future.isResultReadyAt(0)) continue;

            auto const &showResult = future.result();
            if (!showResult) {
              qWarning() << "Failed to fetch model info for" << model.model << ":" << showResult.error();
              continue;
            }

            Capabilities caps = parseCapabilities(showResult->capabilities);
            models.emplace_back(
                FullModelResponse(std::move(model.name), std::move(model.model), caps, model.details.family));
          }

          promise->addResult(std::move(models));
          promise->finish();
        });
  });

  return promise->future();
}

std::shared_ptr<AbstractChatCompletionStream>
OllamaProvider::createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) {
  QNetworkRequest req;
  QUrl url = QString::fromStdString(m_url);

  url.setPath("/api/chat");
  req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
  req.setUrl(url);

  ollama::ChatPayload data;

  data.model = modelId;
  data.messages = payload.messages | std::views::transform([](const AI::ChatMessage &msg) {
                    return ollama::ChatMessage(serializeRole(msg.role), msg.value);
                  }) |
                  std::ranges::to<std::vector>();
  data.options.temperature = payload.temperature;

  std::string serializedData;

  if (const auto error = glz::write_json(data, serializedData)) {
    qWarning() << "Failed to serialize ollama payload" << glz::format_error(error);
    return nullptr;
  }

  Model resolvedModel;

  for (const auto &m : m_models) {
    if (m.model == modelId) {
      resolvedModel = toModel(m);
      break;
    }
  }

  return std::shared_ptr<HttpCompletion>(
      new HttpCompletion(req, QByteArray::fromStdString(serializedData), std::move(resolvedModel)),
      QObjectDeleter{});
}

QFuture<TranscriptionResult> OllamaProvider::transcribe(Audio::Recording, const TranscriptionOptions &) {
  return QtFuture::makeReadyValueFuture<TranscriptionResult>(
      std::unexpected("Transcription is not supported"));
}

} // namespace AI
