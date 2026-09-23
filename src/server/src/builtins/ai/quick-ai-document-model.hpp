#pragma once

#include <QPointer>
#include <vector>
#include "quick-ai-conversation-model.hpp"
#include "ui/quick/markdown-model.hpp"

class QuickAIDocumentModel : public DocumentModel {
  Q_OBJECT

public:
  enum Role {
    KindRole = Qt::UserRole + 1,
    TextRole,
    BlockTypeRole,
    BlockDataRole,
    BlockIndexRole,
    MarkdownModelRole,
    PendingRole,
    FailedRole
  };
  explicit QuickAIDocumentModel(QuickAIConversationModel *conversation, QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  std::span<const DocumentPart> documentParts(int row) const override;

private:
  struct Exchange {
    QPointer<MarkdownModel> markdown;
    std::vector<DocumentPart> query;
    std::vector<DocumentPart> error;
    int offset = 0;
    int blocks = 0;
    bool pending = true;
  };
  const Exchange &exchangeAt(int row) const;
  void addExchange(int index);
  void updateExchange(int index);
  void updateOffsets();
  QuickAIConversationModel *m_conversation;
  std::vector<Exchange> m_exchanges;
};
