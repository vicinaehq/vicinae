#pragma once
#include "ai/ai-provider.hpp"
#include <exception>
#include <immintrin.h>
#include <optional>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qstringview.h>
#include <qtimer.h>
#include <qurl.h>
#include <stdexcept>

class OllamaStreamedCompletion : public StreamedChatCompletion {
  QNetworkReply *_reply;

  ChatMessage parseChatMessage(const QJsonObject &message) {
    auto content = message.value("content").toString();
    auto srole = message.value("role");
    ChatMessage::Role role = ChatMessage::InvalidChatMessageRole;

    if (srole == "assistant") {
      role = ChatMessage::ChatMessageAssistantRole;
    } else if (srole == "user") {
      role = ChatMessage::ChatMessageUserRole;
    } else if (srole == "system") {
      role = ChatMessage::InvalidChatMessageRole;
    }

    return {content, role};
  }

  void handleRead() {
    auto chunk = _reply->readAll();
    QJsonParseError jsonError;
    auto json = QJsonDocument::fromJson(chunk, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
      qDebug() << "Error parsing chunk as json" << jsonError.errorString();
      return;
    }

    auto object = json.object();
    bool done = object.value("done").toBool();

    ChatCompletionToken token;

    token.modelId = object.value("model").toString();
    token.message = parseChatMessage(object.value("message").toObject());
    emit tokenReady(token);

    if (done) { emit finished(); }
  }

  void handleError(QNetworkReply::NetworkError error) { emit errorOccured(_reply->readAll()); }

public:
  OllamaStreamedCompletion(QNetworkReply *reply) : _reply(reply) {
    reply->setParent(this);
    connect(reply, &QNetworkReply::readyRead, this, &OllamaStreamedCompletion::handleRead);
    connect(reply, &QNetworkReply::errorOccurred, this, &OllamaStreamedCompletion::handleError);
  }
};

class OllamaAiProvider : public AbstractAiProvider {
  QTimer *m_listModelRefreshTimer;
  QNetworkAccessManager *_networkManager;
  QUrl _instanceUrl;
  std::vector<AiModel> m_models;

  QUrl makeEndpoint(const QString &path) const {
    qDebug() << _instanceUrl;
    QUrl endpoint = _instanceUrl;

    endpoint.setPath(_instanceUrl.path() + "/" + path);

    return endpoint;
  }

  QJsonObject serializeChatMessage(const ChatMessage &message) const {
    QJsonObject obj;
    QString srole;

    switch (message.role()) {
    case ChatMessage::ChatMessageUserRole:
      srole = "user";
      break;
    case ChatMessage::ChatMessageAssistantRole:
      srole = "assistant";
      break;
    case ChatMessage::ChatMessageSystemRole:
      srole = "system";
      break;
    default:
      srole = "invalid";
      break;
    }

    obj["content"] = message.content().toString();
    obj["role"] = srole;

    return obj;
  }

  // Returned icon names map to bundled icon names (:icons/<name>.svg)
  QString iconFromModelName(const QString &name) const {
    if (name.startsWith("gemma")) return "gemini"; // gemma icon doesn't render well at small sizes
    if (name.startsWith("mistral")) return "mistral";
    if (name.startsWith("deepseek")) return "deepseek";
    if (name.startsWith("llava")) return "llava";
    if (name.startsWith("qwq")) return "qwen";

    return "ollama";
  }

  QJsonArray serializeChatMessageHistory(const std::vector<ChatMessage> messages) const {
    QJsonArray history;

    for (const auto &message : messages) {
      history.push_back(serializeChatMessage(message));
    }

    return history;
  }

  AiModel parseModel(const QJsonObject &obj) const {
    AiModel model;
    auto details = obj.value("details").toObject();
    auto families = details.value("families").toArray();

    model.id = obj.value("model").toString();
    model.displayName = obj.value("name").toString();
    model.capabilities = AiModel::Capability::Reasoning | AiModel::Capability::Embedding;
    model.iconName = iconFromModelName(obj.value("model").toString());

    for (const auto &value : families) {
      auto family = value.toString();

      if (family == "clip") model.capabilities |= AiModel::Capability::Vision;
    }

    return model;
  }

  std::vector<AiModel> parseModels(const QJsonArray &arr) const {
    std::vector<AiModel> models;

    models.reserve(arr.size());

    for (const auto &node : arr) {
      models.push_back(parseModel(node.toObject()));
    }

    return models;
  }

  QFuture<std::vector<AiModel>> fetchModels() {
    auto endpoint = makeEndpoint("api/tags");
    auto promise = std::make_unique<QPromise<std::vector<AiModel>>>();
    auto future = promise->future();
    QNetworkRequest request(endpoint);
    auto reply = _networkManager->get(request);

    promise->start();
    connect(reply, &QNetworkReply::finished, this, [this, reply, promise = std::move(promise)]() {
      qDebug() << "MODEL REPLY FINISHED";
      if (reply->error() == QNetworkReply::NoError) {
        auto data = reply->readAll();
        auto json = QJsonDocument::fromJson(data);
        auto obj = json.object();
        auto models = parseModels(obj.value("models").toArray());

        qDebug() << "got " << models.size() << "models";
        m_models = models;
        promise->addResult(models);
      } else {
        qDebug() << "exception";
        promise->setException(std::make_exception_ptr(std::runtime_error("Error getting models")));
      }
      promise->finish();
      reply->deleteLater();
    });

    return future;
  }

  std::optional<AiModel> findFirstWithCap(AiModel::Capability cap) const {
    for (const auto &model : m_models) {
      if (model.capabilities & cap) { return model; }
    }

    return std::nullopt;
  }

public:
  QString name() const override { return "ollama"; }
  bool isAlive() const override { return true; }

  std::vector<AiModel> listModels() const override { return m_models; }

  std::optional<AiModel> findBestForTask(AiTaskType type) const override {
    switch (type) {
    case AiTaskType::ReasoningTask:
    case AiTaskType::QuickReasoningTask:
      return findFirstWithCap(AiModel::Capability::Reasoning);
    case AiTaskType::EmbeddingTask:
      return findFirstWithCap(AiModel::Capability::Embedding);
    }

    return std::nullopt;
  }

  StreamedChatCompletion *createStreamedCompletion(const CompletionPayload &payload) const override {
    QJsonObject obj;

    obj["model"] = payload.modelId;
    obj["messages"] = serializeChatMessageHistory(payload.messages);
    obj["stream"] = true;

    QJsonDocument doc;
    QNetworkRequest request;
    auto endpoint = makeEndpoint("api/chat");

    doc.setObject(obj);
    request.setUrl(endpoint);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    qDebug() << "edp" << endpoint;

    auto reply = _networkManager->post(request, doc.toJson(QJsonDocument::JsonFormat::Compact));

    return new OllamaStreamedCompletion(reply);
  }

  void setInstanceUrl(const QUrl &url) {
    _instanceUrl = url;
    fetchModels();
  }

  OllamaAiProvider()
      : _networkManager(new QNetworkAccessManager(this)), m_listModelRefreshTimer(new QTimer(this)) {
    m_listModelRefreshTimer->setInterval(std::chrono::minutes(1));
    m_listModelRefreshTimer->start();
    connect(m_listModelRefreshTimer, &QTimer::timeout, this, [this]() { fetchModels(); });
  }
};
