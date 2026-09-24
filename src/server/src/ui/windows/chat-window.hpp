#pragma once

#include <QPointer>
#include <QQmlApplicationEngine>
#include <vector>
#include "builtins/ai/chat-session.hpp"
#include "builtins/ai/conversation-list-model.hpp"
#include "common/context.hpp"
#include "ui/qml-engine-scope.hpp"

class QQuickWindow;

class ChatWindow : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Chat)
  QML_SINGLETON
  Q_PROPERTY(ChatSession *session READ session NOTIFY sessionChanged)
  Q_PROPERTY(ConversationListModel *conversations READ conversations CONSTANT)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
  Q_INVOKABLE void newChat();
  Q_INVOKABLE void openConversation(const QString &id);
  Q_INVOKABLE void renameConversation(const QString &id, const QString &title);
  Q_INVOKABLE void deleteConversation(const QString &id);
  Q_INVOKABLE void openSettings();
  Q_INVOKABLE void searchConversations(QQuickItem *toggleAnchor = nullptr);
  Q_INVOKABLE void clearError();

signals:
  void sessionChanged();
  void loadingChanged();
  void errorChanged();
  void focusRequested();

public:
  explicit ChatWindow(ApplicationContext &context);
  ~ChatWindow() override;
  static ChatWindow *create(QQmlEngine *engine, QJSEngine *) {
    return QmlEngineScope::get<ChatWindow>(engine);
  }
  void show(ChatSession *session = nullptr);
  void trackSession(ChatSession *session);
  ChatSession *findSession(std::string_view id) const;
  ChatSession *session() const { return m_session; }
  ConversationListModel *conversations() { return &m_conversations; }
  bool loading() const { return m_pending; }
  QString error() const { return m_error; }

private:
  void ensureInitialized();
  void loadRoot();
  void activateSession(ChatSession *session);
  void cancelPending();
  void releaseIdleSessions();
  void setError(const QString &error);
  ApplicationContext &m_context;
  QQmlApplicationEngine m_engine;
  ConversationListModel m_conversations;
  QPointer<QQuickWindow> m_window;
  QPointer<ChatSession> m_session;
  QPointer<ChatSession> m_pending;
  struct SessionEntry {
    QPointer<ChatSession> session;
    QPointer<QObject> presenter;
  };
  std::vector<SessionEntry> m_sessions;
  QString m_error;
  bool m_initialized = false;
};
