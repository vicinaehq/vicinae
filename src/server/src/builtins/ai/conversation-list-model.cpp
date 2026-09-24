#include <QDateTime>
#include <algorithm>
#include "conversation-list-model.hpp"

ConversationListModel::ConversationListModel(AI::ConversationStore *store, QObject *parent)
    : QAbstractListModel(parent), m_store(store) {
  connect(store, &AI::ConversationStore::changed, this, &ConversationListModel::reload);
}

int ConversationListModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_rows.size());
}

QVariant ConversationListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
  const auto &row = m_rows[index.row()];
  switch (role) {
  case IdRole:
    return QString::fromStdString(row.id);
  case TitleRole:
    return QString::fromStdString(row.title);
  case GroupRole: {
    const auto days = QDateTime::fromMSecsSinceEpoch(row.updatedAt).date().daysTo(QDate::currentDate());
    return days == 0   ? tr("Today")
           : days == 1 ? tr("Yesterday")
           : days < 7  ? tr("Previous 7 days")
                       : tr("Older");
  }
  default:
    return {};
  }
}

QHash<int, QByteArray> ConversationListModel::roleNames() const {
  return {{IdRole, "conversationId"}, {TitleRole, "title"}, {GroupRole, "dateGroup"}};
}

void ConversationListModel::reload() {
  if (m_loading) {
    m_reloadPending = true;
    return;
  }
  m_loading = true;
  m_store->list().then(this, [this](AI::Result<std::vector<AI::ConversationInfo>> result) {
    m_loading = false;
    if (result) {
      updateRows(std::move(*result));
    } else {
      emit errorOccurred(tr("Could not load conversations."));
    }
    if (std::exchange(m_reloadPending, false)) reload();
  });
}

void ConversationListModel::updateRows(std::vector<AI::ConversationInfo> rows) {
  const bool sameOrder =
      std::ranges::equal(rows, m_rows, {}, &AI::ConversationInfo::id, &AI::ConversationInfo::id);
  if (!sameOrder) beginResetModel();
  m_rows = std::move(rows);
  if (!sameOrder)
    endResetModel();
  else if (!m_rows.empty())
    emit dataChanged(index(0), index(rowCount() - 1), {TitleRole, GroupRole});
}
