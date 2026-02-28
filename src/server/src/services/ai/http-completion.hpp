#pragma once
#include "ai-provider.hpp"
#include "common/types.hpp"
#include <qcontainerfwd.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>

class NetworkManager : NonCopyable {
public:
  static QNetworkAccessManager *manager() {
    static QNetworkAccessManager mgr;
    return &mgr;
  }
};

namespace AI {
class HttpCompletion : public AbstractChatCompletionStream {
public:
  HttpCompletion(const QNetworkRequest &req, const QByteArray &data) : m_req(req), m_data(data) {}

  bool start() override {
    m_reply = NetworkManager::manager()->post(m_req, m_data);

    connect(m_reply, &QNetworkReply::finished, this, &HttpCompletion::handleFinished);
    connect(m_reply, &QNetworkReply::readyRead, this, &HttpCompletion::handleRead);
    connect(m_reply, &QNetworkReply::errorOccurred, this, &HttpCompletion::handleError);

    return m_reply;
  }

  bool abort() override {
    if (m_reply) {
      m_reply->abort();
      return true;
    }
    return false;
  }

  ~HttpCompletion() override {
    if (m_reply) {
      m_reply->abort();
      m_reply->deleteLater();
    }
  }

private:
  void handleRead() {
    const QByteArray chunk = m_reply->readAll();
    qDebug() << "got chunk" << chunk;
    emit dataAdded("");
  }

  void handleError(QNetworkReply::NetworkError error) {
    emit errorOccured(m_reply->errorString().toStdString());
  }

  void handleFinished() { emit finished(); }

  QNetworkRequest m_req;
  QByteArray m_data;
  QNetworkReply *m_reply = nullptr;
};
}; // namespace AI
