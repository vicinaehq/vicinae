#pragma once
#include <qbuffer.h>
#include <qcborvalue.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qtmetamacros.h>

struct CompletionChunk {
  QString model;
  QString createdAt;
  QString response;
  bool done;
};

class CompletionResponse : public QObject {
  Q_OBJECT

  QNetworkReply *reply;

protected slots:
  void readyRead() {
    auto buf = reply->readAll();
    QJsonParseError perror;
    auto doc = QJsonDocument::fromJson(buf, &perror);

    if (perror.error) {
      qDebug() << "chunk parse error!";
      return;
    }

    qDebug() << "chunk" << buf.toStdString();

    auto obj = doc.object();

    CompletionChunk chunk;

    chunk.model = obj.value("model").toString();
    chunk.createdAt = obj.value("createdAt").toString();
    chunk.response = obj.value("response").toString();
    chunk.done = obj.value("done").toBool();

    emit chunkReady(chunk);
  }

public:
  CompletionResponse(QNetworkReply *reply) : reply(reply) {
    connect(reply, &QNetworkReply::readyRead, this, &CompletionResponse::readyRead);
  }

  ~CompletionResponse() { reply->deleteLater(); }

signals:
  void chunkReady(const CompletionChunk &chunk);
};

class OllamaService {
  QUrl baseUrl;
  QNetworkAccessManager *net;

  QNetworkReply *get(const QString &path) {
    QUrl url(baseUrl);
    QNetworkRequest req;

    url.setPath(path);
    req.setUrl(url);

    return net->get(req);
  }

  QNetworkReply *post(const QString &path, const QJsonDocument &payload) {
    QUrl url(baseUrl);
    QNetworkRequest req;

    url.setPath(path);
    req.setUrl(url);
    req.setRawHeader("Content-Type", "application/json");

    return net->post(req, payload.toJson());
  }

public:
  OllamaService() : baseUrl("http://127.0.0.1:11434"), net(new QNetworkAccessManager) {}
  ~OllamaService() { net->deleteLater(); }

  CompletionResponse *generate(const QString &model, const QString &prompt) {
    QJsonDocument doc;
    QJsonObject obj;

    obj["model"] = model;
    obj["prompt"] = prompt;
    obj["suffix"] = "";

    doc.setObject(obj);

    auto reply = post("/api/generate", doc);

    return new CompletionResponse(reply);
  }

  void setBaseUrl(const QUrl &url) { this->baseUrl = url; }
};
