#pragma once

#include <QPointer>
#include <vector>
#include "chat-conversation-model.hpp"
#include <document/markdown-model.hpp>

class ChatDocumentModel : public vicinae::document::DocumentModel {
  Q_OBJECT

signals:
  void loadingChanged();

public:
  enum Role {
    KindRole = Qt::UserRole + 1,
    TextRole,
    BlockTypeRole,
    BlockDataRole,
    BlockIndexRole,
    MarkdownModelRole,
    PendingRole,
    FailedRole,
    AttachmentsRole,
    ToolRole
  };
  explicit ChatDocumentModel(ChatConversationModel *conversation, QObject *parent = nullptr);
  bool loading() const { return m_pendingParses > 0; }
  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  std::span<const vicinae::document::DocumentPart> documentParts(int row) const override;
  TextSnapshot textSnapshot(int row) const override;

private:
  struct Content {
    QPointer<vicinae::document::MarkdownModel> markdown;
    QVariantMap tool;
    int sourcePart = 0;
    int offset = 0;
    int rows = 0;
  };
  struct Exchange {
    std::vector<Content> contents;
    std::vector<vicinae::document::DocumentPart> query;
    std::vector<vicinae::document::DocumentPart> error;
    QVariantList attachments;
    int sourceRow = 0;
    int offset = 0;
    int blocks = 0;
    bool pending = true;
  };
  const Exchange &exchangeAt(int row) const;
  const Content *contentAt(const Exchange &exchange, int row) const;
  void addExchange(int index);
  void updateExchange(int index);
  void addContent(int exchange, int part);
  void updateContent(int exchange, int part);
  void updateOffsets();
  ChatConversationModel *m_conversation;
  std::vector<Exchange> m_exchanges;
  int m_pendingParses = 0;
};
