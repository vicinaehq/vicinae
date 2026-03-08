#pragma once
#include "ai-provider.hpp"
#include <glaze/core/opts.hpp>
#include <glaze/json/read.hpp>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include "services/ai/json-client.hpp"

namespace AI {
struct StreamChunk {
  struct Message {
    std::string content;
  };

  std::optional<Message> message;
  std::optional<std::string> error;
  bool done = false;
};

class HttpCompletion : public AbstractChatCompletionStream {
public:
  HttpCompletion(const QNetworkRequest &req, const QByteArray &data, Model model) : m_req(req), m_data(data) {
    setModel(std::move(model));
  }

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
      if (!m_finished) {
        qInfo() << "Aborted streaming completion from" << model().id;
        m_reply->abort();
      }
      m_reply->deleteLater();
    }
  }

private:
  void handleRead() {
    m_buffer.append(m_reply->readAll());

    while (true) {
      int idx = m_buffer.indexOf('\n');
      if (idx < 0) break;

      QByteArray line = m_buffer.left(idx).trimmed();
      m_buffer.remove(0, idx + 1);

      if (line.isEmpty()) continue;

      StreamChunk chunk;
      if (auto const err = glz::read<glz::opts{.error_on_unknown_keys = false}>(
              chunk, std::string_view(line.constData(), line.size()))) {
        qWarning() << "Failed to parse stream chunk:" << glz::format_error(err, line.constData());
        continue;
      }

      if (chunk.error) {
        emit errorOccured(*chunk.error);
        return;
      }

      if (chunk.message && !chunk.message->content.empty()) { emit dataAdded(chunk.message->content); }
    }
  }

  void handleError(QNetworkReply::NetworkError error) {
    emit errorOccured(m_reply->errorString().toStdString());
  }

  void handleFinished() {
    m_finished = true;
    emit finished();
  }

  QNetworkRequest m_req;
  QByteArray m_data;
  QNetworkReply *m_reply = nullptr;
  QByteArray m_buffer;
  bool m_finished = false;
};
}; // namespace AI
