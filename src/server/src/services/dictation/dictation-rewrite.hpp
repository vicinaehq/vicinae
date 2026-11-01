#pragma once
#include <memory>
#include <string>
#include <QObject>
#include <QString>
#include <QTimer>
#include "services/ai/ai-provider.hpp"
#include "services/dictation/dictation-styles.hpp"

namespace AI {
class Service;
}

/**
 * Rewrites one transcript with a style. Emits exactly one of finished or failed per start, unless aborted.
 */
class DictationRewrite : public QObject {
  Q_OBJECT

signals:
  void finished(const QString &text);
  void failed(const QString &reason);

public:
  explicit DictationRewrite(AI::Service &ai, QObject *parent = nullptr);
  ~DictationRewrite() override;

  void start(const DictationStyles::Style &style, const QString &transcript);
  void abort();
  bool running() const { return m_running; }

private:
  void complete();
  void fail(const QString &reason);
  void release();

  AI::Service &m_ai;
  std::shared_ptr<AI::AbstractChatCompletionStream> m_stream;
  std::string m_text;
  QTimer m_timeout;
  bool m_running = false;
};
