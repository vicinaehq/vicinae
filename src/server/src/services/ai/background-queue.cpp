#include "background-queue.hpp"
#include <cstddef>
#include <utility>

namespace AI {

namespace {
constexpr std::size_t MAX_PENDING = 32;
}

BackgroundQueue::BackgroundQueue(Service &ai, QObject *parent) : QObject(parent), m_rewrite(ai) {
  connect(&m_rewrite, &TextRewrite::finished, this, [this](const QString &text) { deliver(text); });
  connect(&m_rewrite, &TextRewrite::failed, this,
          [this](const QString &reason) { deliver(std::unexpected(reason.toStdString())); });
}

bool BackgroundQueue::submit(TextRewrite::Request request, QObject *context, Callback callback) {
  if (m_pending.size() >= MAX_PENDING) return false;

  m_pending.emplace_back(
      Job{.request = std::move(request), .context = context, .callback = std::move(callback)});
  scheduleNext();

  return true;
}

void BackgroundQueue::cancel(const QObject *context) {
  std::erase_if(m_pending, [context](const Job &job) { return job.context == context; });

  if (!m_current || m_current->context != context) return;

  m_rewrite.abort();
  m_current.reset();
  scheduleNext();
}

// the next request never starts from inside the completion of the previous one
void BackgroundQueue::scheduleNext() {
  QMetaObject::invokeMethod(this, &BackgroundQueue::processNext, Qt::QueuedConnection);
}

void BackgroundQueue::processNext() {
  if (m_current) return;

  while (!m_pending.empty()) {
    auto job = std::move(m_pending.front());
    m_pending.pop_front();

    if (!job.context) continue;

    m_current = std::move(job);
    m_rewrite.start(m_current->request);
    return;
  }
}

void BackgroundQueue::deliver(const Result<QString> &result) {
  const auto job = std::exchange(m_current, std::nullopt);

  scheduleNext();
  if (job && job->context) job->callback(result);
}

} // namespace AI
