#pragma once
#include <QtQml/qqmlregistration.h>
#include "quick-ai-conversation-model.hpp"
#include "ui/quick/attachment-model.hpp"
#include "quick-ai-document-model.hpp"
#include "ui/views/bridge-view.hpp"
#include "ui/image/image-url.hpp"
#include "services/ai/agentic-loop.hpp"
#include "services/ai/conversation-store.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <memory>
#include <optional>
#include <string>

namespace AI {
class Service;
};
class DictationService;
class TranscriptionSession;

class QuickAIViewHost : public ViewHostBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(QuickAIViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(DocumentModel *documentModel READ documentModel CONSTANT)
  Q_PROPERTY(bool streaming READ streaming NOTIFY streamingChanged)
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

signals:
  void conversationLoaded();
  void attachmentStateChanged();
  void dictationAvailableChanged();
  void dictationStateChanged();
  void recordingTimeChanged();
  void dictationMessageChanged();
  void dictated(const QString &text);
  void streamingChanged();
  void modelChanged();
  void modelSelectorItemsChanged();
  void modelSelectorCurrentItemChanged();

public:
  QuickAIViewHost(QString initialQuery, std::optional<AI::ModelRef> model);
  static void openConversation(BaseView *source, std::string id);
  ~QuickAIViewHost() override;
  void beforePop() override;

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;

  bool supportsSearch() const override { return false; }
  bool needsGlobalStatusBar() const override { return false; }

  QString initialNavigationTitle() const override {
    return m_conversation ? QString::fromStdString(m_conversation->title) : QStringLiteral("Quick AI");
  }
  ImageURL initialNavigationIcon() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  DocumentModel *documentModel() { return &m_document; }
  bool streaming() const { return m_agent && m_agent->running(); }
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

  Q_INVOKABLE bool send(const QString &text);
  Q_INVOKABLE void cancel();
  Q_INVOKABLE void toggleTool(quint64 id) { m_exchanges.toggleTool(id); }
  Q_INVOKABLE void toggleToolGroup(quint64 id) { m_exchanges.toggleToolGroup(id); }
  Q_INVOKABLE void selectModel(const QString &compositeId);
  Q_INVOKABLE void toggleDictation();
  Q_INVOKABLE void cancelDictation();

private:
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
  QuickAIConversationModel m_exchanges;
  QuickAIDocumentModel m_document{&m_exchanges};

  QString m_initialQuery;
  QString m_modelLabel;
  ImageUrl m_modelIcon;

  std::optional<AI::ModelRef> m_selectedModel;
  QVariantList m_modelSelectorItems;
  QVariantMap m_modelSelectorCurrentItem;
};
