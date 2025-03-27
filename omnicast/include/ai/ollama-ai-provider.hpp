#pragma once
#include "ai/ai-provider.hpp"
#include <exception>
#include <iterator>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qstringview.h>
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
  QNetworkAccessManager *_networkManager;
  QUrl _instanceUrl;

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

  QJsonArray serializeChatMessageHistory(const std::vector<ChatMessage> messages) const {
    QJsonArray history;

    for (const auto &message : messages) {
      history.push_back(serializeChatMessage(message));
    }

    return history;
  }

  AiModel parseModel(const QJsonObject &obj) const {
    AiModel model;

    model.id = obj.value("model").toString();
    model.displayName = obj.value("name").toString();

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

  QFuture<std::vector<AiModel>> fetchModels() const {
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

public:
  QString name() const override { return "ollama"; }
  bool isAlive() const override { return true; }

  QFuture<std::vector<AiModel>> models() const override { return fetchModels(); }

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

  ChatCompletionToken createCompletion(const CompletionPayload &payload) const override { return {}; }

  void setInstanceUrl(const QUrl &url) { _instanceUrl = url; }

  OllamaAiProvider() : _networkManager(new QNetworkAccessManager(this)) {}
};
