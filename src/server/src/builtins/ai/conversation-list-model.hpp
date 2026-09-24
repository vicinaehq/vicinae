#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include "services/ai/conversation-store.hpp"

class ConversationListModel : public QAbstractListModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(ConversationListModel)
  QML_UNCREATABLE("")
public:
  Q_INVOKABLE QString idAt(int row) const { return data(index(row), IdRole).toString(); }

signals:
  void errorOccurred(const QString &message);

public:
  explicit ConversationListModel(AI::ConversationStore *store, QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  void reload();

private:
  enum Role { IdRole = Qt::UserRole + 1, TitleRole, GroupRole };
  void updateRows(std::vector<AI::ConversationInfo> rows);
  AI::ConversationStore *m_store;
  std::vector<AI::ConversationInfo> m_rows;
  bool m_loading = false;
  bool m_reloadPending = false;
};
