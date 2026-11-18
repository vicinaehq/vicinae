#pragma once
#include <memory>
#include <optional>
#include <string>
#include <QObject>
#include <QString>
#include <QTimer>
#include "services/ai/ai-provider.hpp"

namespace AI {
class Service;
}

/**
 * Runs one prompt that transforms text with a model, in a single request. Emits exactly one of finished or
 * failed per start, unless aborted.
 */
class TextRewrite : public QObject {
  Q_OBJECT

signals:
  void finished(const QString &text);
  void failed(const QString &reason);

public:
  struct Request {
    /** A model reference, as "provider:id". */
    std::string model;
    std::string systemPrompt;
    std::string prompt;
    /** A tag the prompt wraps the text in, stripped from the response if the model echoes it back. */
    std::optional<std::string> echoTag;
  };

  explicit TextRewrite(AI::Service &ai, QObject *parent = nullptr);
  ~TextRewrite() override;

  void start(const Request &request);
  void abort();
  bool running() const { return m_running; }

private:
  void complete();
  void fail(const QString &reason);
  void release();

  AI::Service &m_ai;
  std::shared_ptr<AI::AbstractChatCompletionStream> m_stream;
  std::string m_text;
  std::optional<std::string> m_echoTag;
  QTimer m_timeout;
  bool m_running = false;
};
