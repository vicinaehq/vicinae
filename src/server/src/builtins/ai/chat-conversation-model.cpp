#include <QTextBoundaryFinder>
#include <QJsonDocument>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <chrono>
#include <ranges>

#include "chat-conversation-model.hpp"

namespace {

constexpr auto STREAM_UPDATE_INTERVAL = std::chrono::milliseconds(16);
constexpr auto MIN_REVEAL_DELAY = std::chrono::milliseconds(400);
constexpr auto MAX_REVEAL_DELAY = std::chrono::milliseconds(1200);
constexpr auto REVEAL_RECOVERY_TIME = std::chrono::seconds(4);
constexpr auto FINISH_REVEAL_DELAY = std::chrono::milliseconds(160);

} // namespace

ChatConversationModel::ChatConversationModel(QObject *parent) : QAbstractListModel(parent) {
  m_responseUpdateTimer.setSingleShot(true);
  m_responseUpdateTimer.setTimerType(Qt::PreciseTimer);

  connect(&m_responseUpdateTimer, &QTimer::timeout, this, &ChatConversationModel::advanceResponse);
}

int ChatConversationModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_exchanges.size());
}

QVariant ChatConversationModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};

  const auto &exchange = m_exchanges[index.row()];

  switch (role) {
  case QueryRole:
    return QString::fromStdString(exchange.query);
  case ResponseRole: {
    QString response;

    for (const auto &content : exchange.contents) {
      if (const auto *text = std::get_if<Response>(&content))
        response += QString::fromUtf8(text->text.data(), text->visibleBytes);
    }

    return response;
  }
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

QHash<int, QByteArray> ChatConversationModel::roleNames() const {
  return {{QueryRole, "query"},
          {ResponseRole, "response"},
          {ErrorRole, "error"},
          {PendingRole, "pending"},
          {AttachmentsRole, "attachments"}};
}

void ChatConversationModel::beginExchange(const std::string &query, QVariantList attachments) {
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

void ChatConversationModel::appendSavedResponse(std::string text) {
  if (text.empty()) return;
  auto &contents = m_exchanges.back().contents;
  const auto size = text.size();
  contents.reserve(contents.size() + 1);
  contents.emplace_back(Response{std::move(text), size, true});
  emit contentAdded(rowCount() - 1, contents.size() - 1);
}

void ChatConversationModel::appendResponse(std::string_view text) {
  if (m_exchanges.empty() || !m_exchanges.back().pending || text.empty()) return;

  auto &contents = m_exchanges.back().contents;

  if (contents.empty() || !std::holds_alternative<Response>(contents.back())) {
    m_streamClock.start();
    m_lastUpdate.reset();
    m_lastArrival.reset();
    m_revealDelay = MIN_REVEAL_DELAY.count();
    m_receivedUnits = 0;

    contents.reserve(contents.size() + 1);
    contents.emplace_back(Response{});
    emit contentAdded(rowCount() - 1, contents.size() - 1);
  }

  const auto now = m_streamClock.elapsed();

  if (m_lastArrival) {
    const auto gap = now - *m_lastArrival;

    // Widen quickly for delayed arrivals, then recover by elapsed time rather than chunk count.
    const auto recovered = m_revealDelay * std::exp(-gap / (REVEAL_RECOVERY_TIME.count() * 1000.0));
    m_revealDelay = std::clamp(std::max(gap * 1.5, recovered), static_cast<double>(MIN_REVEAL_DELAY.count()),
                               static_cast<double>(MAX_REVEAL_DELAY.count()));
  }
  m_lastArrival = now;

  std::get<Response>(contents.back()).text.append(text);
  const auto incoming = QString::fromUtf8(text.data(), text.size());
  m_pendingText += incoming;

  const auto begin = m_receivedUnits;
  m_receivedUnits += incoming.size();

  if (!m_revealBatches.empty() && m_revealBatches.back().receivedAt == now)
    m_revealBatches.back().end = m_receivedUnits;
  else
    m_revealBatches.emplace_back(RevealBatch{begin, m_receivedUnits, now, qRound64(m_revealDelay)});

  if (m_responseUpdateTimer.isActive()) return;

  if (!m_lastUpdate || now - *m_lastUpdate >= STREAM_UPDATE_INTERVAL.count())
    advanceResponse();
  else
    m_responseUpdateTimer.start(
        std::chrono::milliseconds(STREAM_UPDATE_INTERVAL.count() - (now - *m_lastUpdate)));
}

void ChatConversationModel::advanceResponse() {
  if (m_pendingText.isEmpty()) return;

  const auto now = m_streamClock.elapsed();

  // Sum fractional progress before rounding so small chunks also reveal at a steady pace.
  double target = m_revealBatches.front().begin;

  for (const auto &batch : m_revealBatches) {
    const auto elapsed = std::min(now - batch.receivedAt, batch.duration);
    target += (batch.end - batch.begin) * static_cast<double>(elapsed) / batch.duration;
  }

  const auto visibleUnits = m_receivedUnits - m_pendingText.size();
  qsizetype count = std::clamp(static_cast<qsizetype>(target) - visibleUnits,
                               m_lastUpdate ? qsizetype{0} : qsizetype{1}, m_pendingText.size());
  m_lastUpdate = now;

  if (count == 0) {
    m_responseUpdateTimer.start(STREAM_UPDATE_INTERVAL);
    return;
  }

  if (count < m_pendingText.size()) {
    QTextBoundaryFinder boundary(QTextBoundaryFinder::Grapheme, m_pendingText);
    boundary.setPosition(count);
    if (!boundary.isAtBoundary()) count = boundary.toNextBoundary();
  }

  std::get<Response>(m_exchanges.back().contents.back()).visibleBytes +=
      QStringView(m_pendingText).first(count).toUtf8().size();
  m_pendingText.remove(0, count);

  while (!m_revealBatches.empty() && m_revealBatches.front().end <= visibleUnits + count)
    m_revealBatches.pop_front();

  if (!m_pendingText.isEmpty()) m_responseUpdateTimer.start(STREAM_UPDATE_INTERVAL);
  publishResponse();
}

void ChatConversationModel::flushResponse() {
  m_responseUpdateTimer.stop();
  if (m_pendingText.isEmpty()) return;

  m_pendingText.clear();
  m_revealBatches.clear();

  auto &response = std::get<Response>(m_exchanges.back().contents.back());
  response.visibleBytes = response.text.size();

  publishResponse();
}

void ChatConversationModel::publishResponse() {
  emit contentChanged(rowCount() - 1, m_exchanges.back().contents.size() - 1);

  const auto last = index(rowCount() - 1);
  emit dataChanged(last, last, {ResponseRole});
}

void ChatConversationModel::addTool(Tool tool) {
  if (m_exchanges.empty()) return;

  flushResponse();

  auto &contents = m_exchanges.back().contents;
  const bool newGroup = contents.empty() || !std::holds_alternative<ToolGroup>(contents.back());

  if (newGroup) {
    contents.reserve(contents.size() + 1);
    contents.emplace_back(ToolGroup{});
  }

  auto &group = std::get<ToolGroup>(contents.back());
  if (group.calls.size() == 1 && group.calls.front().expanded) group.expanded = true;

  const auto json = QJsonDocument::fromJson(tool.arguments.toUtf8());
  if (!json.isNull()) tool.arguments = QString::fromUtf8(json.toJson()).trimmed();

  if (group.calls.size() == group.calls.capacity())
    group.calls.reserve(std::max<std::size_t>(4, group.calls.size() * 2));
  group.calls.emplace_back(std::move(tool));

  if (newGroup)
    emit contentAdded(rowCount() - 1, contents.size() - 1);
  else
    emit contentChanged(rowCount() - 1, contents.size() - 1);
}

void ChatConversationModel::updateTool(quint64 id, QString status, std::optional<QString> output,
                                       std::optional<qint64> durationMs, std::optional<QString> statusText) {
  for (int row = 0; row < rowCount(); ++row) {
    auto &contents = m_exchanges[row].contents;

    for (std::size_t part = 0; part < contents.size(); ++part) {
      auto *group = std::get_if<ToolGroup>(&contents[part]);
      if (!group) continue;

      const auto tool = std::ranges::find(group->calls, id, &Tool::id);
      if (tool == group->calls.end()) continue;

      tool->status = std::move(status);
      tool->output = std::move(output);
      tool->durationMs = durationMs;
      tool->statusText = std::move(statusText);

      emit contentChanged(row, part);
      return;
    }
  }
}

void ChatConversationModel::toggleTool(quint64 id) {
  for (int row = 0; row < rowCount(); ++row) {
    auto &contents = m_exchanges[row].contents;

    for (std::size_t part = 0; part < contents.size(); ++part) {
      auto *group = std::get_if<ToolGroup>(&contents[part]);
      if (!group) continue;

      const auto tool = std::ranges::find(group->calls, id, &Tool::id);
      if (tool == group->calls.end()) continue;

      tool->expanded = !tool->expanded;
      emit contentChanged(row, part);
      return;
    }
  }
}

void ChatConversationModel::toggleToolGroup(quint64 id) {
  for (int row = 0; row < rowCount(); ++row) {
    auto &contents = m_exchanges[row].contents;

    for (std::size_t part = 0; part < contents.size(); ++part) {
      auto *group = std::get_if<ToolGroup>(&contents[part]);
      if (!group || group->calls.front().id != id) continue;

      group->expanded = !group->expanded;
      emit contentChanged(row, part);
      return;
    }
  }
}

void ChatConversationModel::finishExchange(const std::string &error) {
  if (m_exchanges.empty()) return;

  if (!error.empty()) {
    flushResponse();
  } else if (!m_pendingText.isEmpty()) {
    const auto now = m_streamClock.elapsed();
    m_revealBatches.clear();
    m_revealBatches.emplace_back(RevealBatch{m_receivedUnits - m_pendingText.size(), m_receivedUnits, now,
                                             FINISH_REVEAL_DELAY.count()});
    m_lastUpdate = now;
    m_responseUpdateTimer.start(STREAM_UPDATE_INTERVAL);
  }

  auto &exchange = m_exchanges.back();
  exchange.pending = false;
  exchange.error = error;

  const auto last = index(rowCount() - 1);
  emit dataChanged(last, last, {PendingRole, ErrorRole});
}
