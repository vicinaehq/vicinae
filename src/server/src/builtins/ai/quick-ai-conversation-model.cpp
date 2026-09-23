#include <QTextBoundaryFinder>
#include <algorithm>
#include <cstddef>
#include <chrono>
#include "quick-ai-conversation-model.hpp"

namespace {
constexpr auto STREAM_UPDATE_INTERVAL = std::chrono::milliseconds(16);
constexpr auto MAX_REVEAL_DELAY = std::chrono::milliseconds(120);
constexpr qsizetype MIN_REVEAL_UNITS = 8;
} // namespace

QuickAIConversationModel::QuickAIConversationModel(QObject *parent) : QAbstractListModel(parent) {
  m_responseUpdateTimer.setSingleShot(true);
  m_responseUpdateTimer.setTimerType(Qt::PreciseTimer);
  connect(&m_responseUpdateTimer, &QTimer::timeout, this, &QuickAIConversationModel::advanceResponse);
}

int QuickAIConversationModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_exchanges.size());
}

QVariant QuickAIConversationModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
  const auto &exchange = m_exchanges[index.row()];
  switch (role) {
  case QueryRole:
    return QString::fromStdString(exchange.query);
  case ResponseRole:
    return QString::fromUtf8(exchange.response.data(), exchange.visibleBytes);
  case ErrorRole:
    return QString::fromStdString(exchange.error);
  case PendingRole:
    return exchange.pending;
  case AttachmentsRole:
    return exchange.attachments;
  default:
    return {};
  }
}

QHash<int, QByteArray> QuickAIConversationModel::roleNames() const {
  return {{QueryRole, "query"},
          {ResponseRole, "response"},
          {ErrorRole, "error"},
          {PendingRole, "pending"},
          {AttachmentsRole, "attachments"}};
}

void QuickAIConversationModel::beginExchange(const std::string &query, QVariantList attachments) {
  flushResponse();
  m_streamClock.start();
  m_lastUpdate.reset();
  m_receivedUnits = 0;
  const int row = rowCount();
  beginInsertRows({}, row, row);
  if (m_exchanges.size() == m_exchanges.capacity())
    m_exchanges.reserve(std::max<std::size_t>(8, m_exchanges.size() * 2));
  m_exchanges.emplace_back(Exchange{.query = query, .attachments = std::move(attachments)});
  endInsertRows();
}

void QuickAIConversationModel::appendResponse(std::string_view text) {
  if (m_exchanges.empty() || !m_exchanges.back().pending || text.empty()) return;
  const auto now = m_streamClock.elapsed();
  m_exchanges.back().response.append(text);
  const auto incoming = QString::fromUtf8(text.data(), text.size());
  m_pendingText += incoming;
  const auto begin = m_receivedUnits;
  m_receivedUnits += incoming.size();
  if (!m_revealBatches.empty() && m_revealBatches.back().receivedAt == now)
    m_revealBatches.back().end = m_receivedUnits;
  else
    m_revealBatches.emplace_back(RevealBatch{begin, m_receivedUnits, now});
  if (m_responseUpdateTimer.isActive()) return;
  if (!m_lastUpdate || now - *m_lastUpdate >= STREAM_UPDATE_INTERVAL.count())
    advanceResponse();
  else
    m_responseUpdateTimer.start(
        std::chrono::milliseconds(STREAM_UPDATE_INTERVAL.count() - (now - *m_lastUpdate)));
}

void QuickAIConversationModel::advanceResponse() {
  if (m_pendingText.isEmpty()) return;
  const auto now = m_streamClock.elapsed();
  // Each arrival contributes a short linear ramp. Overlapping bursts increase the reveal rate.
  auto target = m_revealBatches.front().begin;
  for (const auto &batch : m_revealBatches) {
    const auto elapsed = std::min(now - batch.receivedAt, MAX_REVEAL_DELAY.count());
    target += (batch.end - batch.begin) * elapsed / MAX_REVEAL_DELAY.count();
  }
  const auto visibleUnits = m_receivedUnits - m_pendingText.size();
  qsizetype count = std::min(m_pendingText.size(), std::max(MIN_REVEAL_UNITS, target - visibleUnits));
  if (count < m_pendingText.size()) {
    QTextBoundaryFinder boundary(QTextBoundaryFinder::Grapheme, m_pendingText);
    boundary.setPosition(count);
    if (!boundary.isAtBoundary()) count = boundary.toNextBoundary();
  }
  m_exchanges.back().visibleBytes += QStringView(m_pendingText).first(count).toUtf8().size();
  m_pendingText.remove(0, count);
  m_lastUpdate = now;
  while (!m_revealBatches.empty() && m_revealBatches.front().end <= visibleUnits + count)
    m_revealBatches.pop_front();
  if (!m_pendingText.isEmpty()) m_responseUpdateTimer.start(STREAM_UPDATE_INTERVAL);
  const auto last = index(rowCount() - 1);
  emit dataChanged(last, last, {ResponseRole});
}

void QuickAIConversationModel::flushResponse() {
  m_responseUpdateTimer.stop();
  if (m_pendingText.isEmpty()) return;
  m_pendingText.clear();
  m_revealBatches.clear();
  m_exchanges.back().visibleBytes = m_exchanges.back().response.size();
  const auto last = index(rowCount() - 1);
  emit dataChanged(last, last, {ResponseRole});
}

void QuickAIConversationModel::finishExchange(const std::string &error) {
  if (m_exchanges.empty()) return;
  // Successful completion keeps draining the final burst; cancellation reveals received text immediately.
  if (!error.empty()) flushResponse();
  auto &exchange = m_exchanges.back();
  exchange.pending = false;
  exchange.error = error;
  const auto last = index(rowCount() - 1);
  emit dataChanged(last, last, {PendingRole, ErrorRole});
}
