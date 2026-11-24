#pragma once
#include <deque>
#include <functional>
#include <optional>
#include <QObject>
#include <QPointer>
#include <QString>
#include "services/ai/ai-provider.hpp"
#include "services/ai/text-rewrite.hpp"

namespace AI {

/**
 * Runs model requests that nobody is waiting on, one at a time across every caller so that background work
 * never competes with itself for a model. Each request is tried once.
 */
class BackgroundQueue : public QObject {
  Q_OBJECT

public:
  using Callback = std::function<void(const Result<QString> &)>;

  explicit BackgroundQueue(Service &ai, QObject *parent = nullptr);

  /**
   * The callback is called with the answer of the model or the reason it could not be obtained, unless the
   * context is destroyed or cancels its requests first. Returns false if too many requests are already
   * waiting, in which case the request is dropped.
   */
  bool submit(TextRewrite::Request request, QObject *context, Callback callback);

  /**
   * Drops every request submitted with this context, including the one that is running.
   */
  void cancel(const QObject *context);

private:
  struct Job {
    TextRewrite::Request request;
    QPointer<QObject> context;
    Callback callback;
  };

  void scheduleNext();
  void processNext();
  void deliver(const Result<QString> &result);

  TextRewrite m_rewrite;
  std::deque<Job> m_pending;
  std::optional<Job> m_current;
};

} // namespace AI
