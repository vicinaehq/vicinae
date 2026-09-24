#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include <optional>
#include <string>
#include "chat-conversation-model.hpp"
#include "chat-document-model.hpp"
#include "services/ai/agentic-loop.hpp"
#include "services/ai/conversation-store.hpp"
#include "ui/quick/attachment-model.hpp"

namespace AI {
class Service;
};
class DictationService;
class TranscriptionSession;

class ChatSession : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(ChatSession)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString conversationId READ conversationId NOTIFY conversationChanged)
  Q_PROPERTY(QString title READ title NOTIFY conversationChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(QString draft READ draft WRITE setDraft NOTIFY draftChanged)
  Q_PROPERTY(DocumentModel *documentModel READ documentModel CONSTANT)
  Q_PROPERTY(bool streaming READ streaming NOTIFY streamingChanged)
  Q_PROPERTY(bool awaitingResponse READ awaitingResponse NOTIFY activityChanged)
  Q_PROPERTY(bool thinking READ thinking NOTIFY activityChanged)
  Q_PROPERTY(AttachmentModel *attachments READ attachments CONSTANT)
  Q_PROPERTY(bool canSend READ canSend NOTIFY attachmentStateChanged)
  Q_PROPERTY(QString attachmentMessage READ attachmentMessage NOTIFY attachmentStateChanged)
  Q_PROPERTY(QString modelLabel READ modelLabel NOTIFY modelChanged)
  Q_PROPERTY(ImageUrl modelIcon READ modelIcon NOTIFY modelChanged)
  Q_PROPERTY(QVariantList modelSelectorItems READ modelSelectorItems NOTIFY modelSelectorItemsChanged)
  Q_PROPERTY(QVariantMap modelSelectorCurrentItem READ modelSelectorCurrentItem NOTIFY
                 modelSelectorCurrentItemChanged)
  Q_PROPERTY(bool dictationAvailable READ dictationAvailable NOTIFY dictationAvailableChanged)
  Q_PROPERTY(bool recording READ recording NOTIFY dictationStateChanged)
  Q_PROPERTY(bool transcribing READ transcribing NOTIFY dictationStateChanged)
  Q_PROPERTY(QString recordingTime READ recordingTime NOTIFY recordingTimeChanged)
  Q_PROPERTY(QString dictationMessage READ dictationMessage NOTIFY dictationMessageChanged)

public:
  Q_INVOKABLE bool send(const QString &text);
  Q_INVOKABLE void cancel();
  Q_INVOKABLE void toggleTool(quint64 id) { m_exchanges.toggleTool(id); }
  Q_INVOKABLE void toggleToolGroup(quint64 id) { m_exchanges.toggleToolGroup(id); }
  Q_INVOKABLE void selectModel(const QString &compositeId);
  Q_INVOKABLE void toggleDictation();
  Q_INVOKABLE void cancelDictation();

signals:
  void conversationLoaded();
  void loadFailed(const QString &message);
  void conversationChanged();
  void loadingChanged();
  void draftChanged();
  void attachmentStateChanged();
  void dictationAvailableChanged();
  void dictationStateChanged();
  void recordingTimeChanged();
  void dictationMessageChanged();
  void dictated(const QString &text);
  void streamingChanged();
  void activityChanged();
  void modelChanged();
  void modelSelectorItemsChanged();
  void modelSelectorCurrentItemChanged();

public:
  explicit ChatSession(std::optional<AI::ModelRef> model = std::nullopt, QObject *parent = nullptr);
  ~ChatSession() override;
  void load(std::string id);
  const std::optional<AI::ModelRef> &selectedModel() const { return m_selectedModel; }

  QString conversationId() const {
    return m_conversation ? QString::fromStdString(m_conversation->id) : QString{};
  }
  QString title() const {
    return m_conversation ? QString::fromStdString(m_conversation->title) : tr("New chat");
  }
  bool loading() const { return m_loading; }
  QString draft() const { return m_draft; }
  void setDraft(const QString &text) {
    if (m_draft == text) return;
    m_draft = text;
    emit draftChanged();
  }

  DocumentModel *documentModel() { return &m_document; }
  bool streaming() const { return m_agent && m_agent->running(); }
  bool awaitingResponse() const;
  bool thinking() const { return streaming() && m_agent->activity() == AI::ResponseActivity::Thinking; }
  AttachmentModel *attachments() { return &m_attachments; }
  bool canSend() const;
  QString attachmentMessage() const;
  QString modelLabel() const { return m_modelLabel; }
  ImageUrl modelIcon() const { return m_modelIcon; }
  QVariantList modelSelectorItems() const { return m_modelSelectorItems; }
  QVariantMap modelSelectorCurrentItem() const { return m_modelSelectorCurrentItem; }

  bool dictationAvailable() const { return m_dictationAvailable; }
  bool recording() const;
  bool transcribing() const;
  QString recordingTime() const;
  QString dictationMessage() const { return m_dictationMessage; }

private:
  void initialize();
  void saveTurn();
  void generateTitle();
  void addTool(const AI::Agent::ToolCall &call);
  void updateTool(const AI::Agent::ToolCall &call);
  void restoreConversation(AI::Conversation conversation);
  void restoreNextTurn();
  void finishLoading();
  QVariantList attachmentItems(const std::vector<AI::ConversationAttachment> &attachments) const;

  AI::ConversationStore *m_store = nullptr;
  std::optional<AI::ConversationInfo> m_conversation;
  std::optional<AI::ConversationTurn> m_turn;
  std::optional<AI::Conversation> m_restoring;
  std::vector<AI::Agent::Message> m_restoredMessages;
  std::vector<AI::Agent::ToolCall> m_restoredCalls;
  std::size_t m_messageOffset = 0;
  std::size_t m_callOffset = 0;
  std::size_t m_restoreIndex = 0;
  int m_turnCount = 0;
  bool m_loading = false;
  bool m_removed = false;

  void sendQuery(const std::string &query);
  bool needsVision() const;
  bool modelSupports(AI::Capability capability) const;
  void rebuildModelSelectorItems();
  void updateDictationAvailable();
  void startDictation();
  void showDictationMessage(const QString &message);

  AI::Service *m_aiService = nullptr;
  DictationService *m_dictationService = nullptr;
  TranscriptionSession *m_dictation = nullptr;
  QTimer m_dictationMessageTimer;
  bool m_dictationAvailable = false;
  QString m_dictationMessage;
  AI::Agent *m_agent = nullptr;
  AttachmentModel m_attachments;
  ChatConversationModel m_exchanges;
  ChatDocumentModel m_document{&m_exchanges};

  QString m_draft;
  QString m_modelLabel;
  ImageUrl m_modelIcon;

  std::optional<AI::ModelRef> m_selectedModel;
  QVariantList m_modelSelectorItems;
  QVariantMap m_modelSelectorCurrentItem;
};
