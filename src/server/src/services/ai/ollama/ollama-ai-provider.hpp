#include "builtin_icon.hpp"
#include "image-url.hpp"
#include "services/ai/ai-provider.hpp"
#include <cstdint>
#include <expected>
#include <glaze/core/opts.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include "services/ai/http-completion.hpp"
#include <glaze/json/write.hpp>
#include <qcontainerfwd.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <ranges>

namespace AI {

struct OllamaConfig {
  std::string url = "http://localhost:11434";
  std::optional<std::string> pwd;
};

class JsonClient {
public:
  template <typename T>
  static QFuture<AI::Result<T>> waitForRequest(QPromise<AI::Result<T>> promise, QNetworkReply *reply) {
    auto future = promise.future();

    QObject::connect(reply, &QNetworkReply::finished, [promise = std::move(promise), reply]() mutable {
      if (reply->error() != QNetworkReply::NoError) {
        promise.addResult(std::unexpected(reply->errorString().toStdString()));
        promise.finish();
        reply->deleteLater();
        return;
      }

      auto const data = reply->readAll();
      reply->deleteLater();
      T res;

      if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(res, data.constData())) {
        promise.addResult(std::unexpected(std::string("Failed to parse response: ") +
                                          glz::format_error(error, data.constData())));
        promise.finish();
        return;
      }

      promise.addResult(std::move(res));
      promise.finish();
    });

    return future;
  }

  template <typename T> QFuture<AI::Result<T>> get(const QUrl &url) {
    qInfo() << "[GET]" << url;
    QPromise<AI::Result<T>> promise;
    QNetworkRequest req;

    req.setUrl(url);

    auto reply = NetworkManager::manager()->get(req);

    return waitForRequest<T>(std::move(promise), reply);
  }

  template <typename T, typename U> QFuture<AI::Result<T>> post(const QUrl &url, const U &data) {
    qInfo() << "[POST]" << url;
    std::string payload;
    QPromise<AI::Result<T>> promise;
    auto future = promise.future();
    QNetworkRequest req;

    if (auto const error = glz::write_json(data, payload)) {
      promise.addResult(std::unexpected(std::string("Failed to serialize request")));
      promise.finish();
      return future;
    }

    req.setUrl(url);
    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto reply = NetworkManager::manager()->post(req, payload.c_str());

    return waitForRequest<T>(std::move(promise), reply);
  }
};

class OllamaProvider : public AbstractProvider {
  struct ModelShowResponse {
    std::vector<std::string> capabilities;
  };

  struct FullModelResponse {
    std::string name;
    std::string model;
    Capabilities capabilities;
    std::string family;
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

  struct ChatMessage {
    std::string role;
    std::string content;
  };

  struct ChatPayload {
    std::string model;
    std::vector<ChatMessage> messages;
    bool stream = true;
  };

  static std::optional<Capability> parseCapability(std::string_view str) {
    if (str == "completion") { return Capability::Chat; }
    if (str == "tools") { return Capability::ToolCalling; }
    if (str == "vision") { return Capability::Vision; }
    if (str == "thinking") { return Capability::Thinking; }
    if (str == "embedding") { return Capability::Embedding; }
    // ollama doesn't support transcription models
    return std::nullopt;
  }

  static Capabilities parseCapabilities(std::span<const std::string> strs) {
    Capabilities caps{};

    for (const auto &str : strs) {
      if (auto cap = parseCapability(str)) { caps |= cap.value(); }
    }

    return caps;
  }

  static AI::Model toModel(const FullModelResponse &model) {
    return AI::Model{
        .id = model.model,
        .name = model.name,
        .icon = getModelIcon(model),
        .caps = model.capabilities,
    };
  }

  std::string id() const override { return "ollama"; }

  std::optional<ImageUrl> icon() const override { return {}; }

  ModelList listModels(const ListModelFilters &filters = {}) const override {
    return m_models | std::views::transform([](const FullModelResponse &model) { return toModel(model); }) |
           std::ranges::to<std::vector>();
  }

  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override {
    for (const auto &model : m_models) {
      if (model.capabilities & caps) { return toModel(model); }
    }

    return std::nullopt;
  }

  QUrl makeUrl(const QString &path) {
    QUrl url = QString::fromStdString(m_cfg.url);
    url.setPath(path);
    return url;
  }

  static ImageUrl getModelIcon(const FullModelResponse &model) {
    if (model.name.contains("mistral")) return ImageUrl{BuiltinIcon::Mistral};
    if (model.name.contains("llava")) return ImageUrl{BuiltinIcon::Llava};
    if (model.name.contains("deepseek")) return ImageUrl{BuiltinIcon::Deepseek};
    if (model.name.contains("gemma")) return ImageUrl{BuiltinIcon::Gemma};
    if (model.family.starts_with("qwen")) return ImageUrl{BuiltinIcon::Qwen};

    return ImageUrl{BuiltinIcon::Ollama};
  }

  QFuture<Result<std::vector<FullModelResponse>>> listModelsFull() {
    auto promise = std::make_shared<QPromise<Result<std::vector<FullModelResponse>>>>();

    fetchModels().then([this, promise](Result<ListModelsResponse> result) mutable {
      if (!result) {
        promise->addResult(std::unexpected(result.error()));
        promise->finish();
        return;
      }

      auto modelList = std::move(*result);
      std::vector<QFuture<Result<ModelShowResponse>>> futures;
      futures.reserve(modelList.models.size());

      for (const auto &model : modelList.models) {
        futures.emplace_back(modelShow(model.model));
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
              models.emplace_back(FullModelResponse(std::move(model.name), std::move(model.model), caps,
                                                    model.details.family));
            }

            promise->addResult(std::move(models));
            promise->finish();
          });
    });

    return promise->future();
  }

  QFuture<Result<ListModelsResponse>> fetchModels() {
    return JsonClient{}.get<ListModelsResponse>(makeUrl("/api/tags"));
  }

  QFuture<Result<ModelShowResponse>> modelShow(const std::string &model) {
    return JsonClient{}.post<ModelShowResponse, ModelShowRequest>(makeUrl("/api/show"), {.model = model});
  }

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const override {
    QNetworkRequest req;
    QUrl url = QString::fromStdString(m_cfg.url);

    url.setPath("/api/chat");
    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto serializeRole = [](AI::ChatRole role) {
      switch (role) {
      case AI::ChatRole::User:
        return "user";
      case AI::ChatRole::Assistant:
        return "assistant";
      case AI::ChatRole::System:
        return "system";
      }
      return "none";
    };

    req.setUrl(url);

    auto tr = [&](const AI::ChatMessage &msg) -> ChatMessage {
      return ChatMessage(serializeRole(msg.role), msg.value);
    };

    ChatPayload data;

    data.model = payload.modelId;
    data.messages = payload.messages | std::views::transform(tr) | std::ranges::to<std::vector>();

    std::string serializedData;

    if (const auto error = glz::write_json(data, serializedData)) {
      qWarning() << "Failed to serialize ollama payload" << glz::format_error(error);
      return nullptr;
    }

    qDebug() << url << serializedData;

    return std::make_shared<HttpCompletion>(req, QByteArray::fromStdString(serializedData));
  }

  QFuture<TranscriptionResult> transcribe() const override { return {}; }

  using Watcher = QFutureWatcher<Result<std::vector<FullModelResponse>>>;

public:
  bool initalize(const glz::raw_json &json) override {
    /*
if (const auto error = glz::read_json(m_cfg, json.str)) {
          return false;
  }
  */

    connect(&m_listWatcher, &Watcher::finished, this, [this]() {
      if (m_listWatcher.isCanceled() || !m_listWatcher.isFinished()) return;

      auto result = m_listWatcher.result();
      if (!result) {
        qWarning() << "Failed to fetch Ollama models:" << result.error();
        return;
      }

      m_models = std::move(*result);
      emit modelsUpdated();
    });

    m_listWatcher.setFuture(listModelsFull());

    return true;
  }

private:
  Watcher m_listWatcher;
  OllamaConfig m_cfg;
  std::vector<FullModelResponse> m_models;
};

}; // namespace AI
