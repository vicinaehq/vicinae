#include <QQuickWindow>
#include <algorithm>
#include "chat-window.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/bridges/config-bridge.hpp"
#include "ui/qml-dev-loader.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/views/view-utils.hpp"
#ifdef Q_OS_MACOS
#include "ui/quick/macos-chrome-attached.hpp"
#endif

ChatWindow::ChatWindow(ApplicationContext &context)
    : m_context(context), m_conversations(ServiceRegistry::instance()->conversations()) {
  connect(&m_conversations, &ConversationListModel::errorOccurred, this, &ChatWindow::setError);
  connect(ServiceRegistry::instance()->conversations(), &AI::ConversationStore::removed, this,
          [this](const QString &id) {
            if (m_session && m_session->conversationId() == id) {
              m_session = nullptr;
              newChat();
            }
            releaseIdleSessions();
          });
}

ChatWindow::~ChatWindow() {
  for (const auto &[session, presenter] : m_sessions) {
    if (session && session->parent() == this) session->cancel();
  }
}

void ChatWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;
  QmlEngineScope::set(&m_engine, this);
  QmlEngineScope::set(&m_engine, new ConfigBridge(ConfigBridge::OpaqueSurfaces, this));
  QmlDevLoader::attach(&m_engine, [this] {
    const auto geometry = m_window ? m_window->geometry() : QRect{};
    const bool visible = m_window && m_window->isVisible();
    if (m_window) m_window->deleteLater();
    m_window = nullptr;
    loadRoot();
    if (m_window && geometry.isValid()) m_window->setGeometry(geometry);
    if (visible) show();
  });
  loadRoot();
  m_conversations.reload();
}

void ChatWindow::loadRoot() {
  const auto component =
#ifdef Q_OS_MACOS
      u"ChatWindowMacOS";
#elif defined(Q_OS_WIN)
      u"ChatWindowWindows";
#else
      u"ChatWindow";
#endif
  const auto roots = m_engine.rootObjects();
  m_engine.load(qml::componentUrl(component));
  for (auto *root : m_engine.rootObjects()) {
    if (!roots.contains(root)) m_window = qobject_cast<QQuickWindow *>(root);
  }
  if (!m_window) qWarning() << "Chat window did not load";
}

void ChatWindow::show(ChatSession *session) {
  if (session)
    activateSession(session);
  else if (!m_session)
    newChat();
  ensureInitialized();
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
#ifdef Q_OS_MACOS
  macosActivateApp();
#endif
  emit focusRequested();
}

void ChatWindow::trackSession(ChatSession *session) {
  std::erase_if(m_sessions, [](const auto &entry) { return entry.session.isNull(); });
  if (std::ranges::find(m_sessions, session, [](const auto &entry) { return entry.session.data(); }) !=
      m_sessions.end())
    return;
  m_sessions.reserve(m_sessions.size() + 1);
  m_sessions.emplace_back(SessionEntry{session, session->parent() == this ? nullptr : session->parent()});
  connect(session, &ChatSession::streamingChanged, this, &ChatWindow::releaseIdleSessions,
          Qt::QueuedConnection);
}

ChatSession *ChatWindow::findSession(std::string_view id) const {
  for (const auto &[session, presenter] : m_sessions) {
    if (session && session->conversationId().toStdString() == id) return session;
  }
  return nullptr;
}

void ChatWindow::cancelPending() {
  if (!m_pending) return;
  m_pending->deleteLater();
  m_pending = nullptr;
  emit loadingChanged();
}

void ChatWindow::activateSession(ChatSession *session) {
  cancelPending();
  clearError();
  trackSession(session);
  session->setParent(this);
  if (m_session == session) {
    emit focusRequested();
    return;
  }
  m_session = session;
  emit sessionChanged();
  releaseIdleSessions();
}

void ChatWindow::newChat() {
  if (m_session && m_session->conversationId().isEmpty() && !m_session->loading()) {
    cancelPending();
    emit focusRequested();
    return;
  }
  for (const auto &[session, presenter] : m_sessions) {
    if (session && !presenter && session->conversationId().isEmpty()) {
      activateSession(session);
      return;
    }
  }
  activateSession(new ChatSession(m_session ? m_session->selectedModel() : std::nullopt, this));
}

void ChatWindow::releaseIdleSessions() {
  std::erase_if(m_sessions, [this](const auto &entry) {
    auto *session = entry.session.data();
    if (!session) return true;
    if (session == m_session || entry.presenter || session->parent() != this || session->streaming() ||
        !session->draft().isEmpty() || session->attachments()->count() > 0 || session->recording() ||
        session->transcribing())
      return false;
    session->deleteLater();
    return true;
  });
}

void ChatWindow::openConversation(const QString &id) {
  if (id.isEmpty()) return;
  if (auto *session = findSession(id.toStdString())) {
    activateSession(session);
    return;
  }
  cancelPending();
  clearError();
  auto *session = new ChatSession(std::nullopt, this);
  m_pending = session;
  emit loadingChanged();
  connect(session, &ChatSession::conversationLoaded, this, [this, session] {
    if (m_pending != session) return;
    m_pending = nullptr;
    emit loadingChanged();
    activateSession(session);
  });
  connect(session, &ChatSession::loadFailed, this, [this, session](const QString &message) {
    if (m_pending != session) return;
    cancelPending();
    setError(message);
  });
  session->load(id.toStdString());
}

void ChatWindow::renameConversation(const QString &id, const QString &title) {
  const auto name = title.simplified().left(100);
  if (id.isEmpty() || name.isEmpty()) return;
  m_context.services->conversations()->setTitle(id.toStdString(), name.toStdString());
}

void ChatWindow::deleteConversation(const QString &id) {
  if (id.isEmpty()) return;
  cancelPending();
  m_context.services->conversations()
      ->remove(id.toStdString())
      .then(this, [this](const AI::Result<void> &result) {
        if (!result) { setError(tr("Could not delete conversation.")); }
      });
}

void ChatWindow::openSettings() { m_context.settings->openTab(QStringLiteral("ai")); }

void ChatWindow::searchConversations(QQuickItem *toggleAnchor) {
  const ActivateEntrypointOptions options{
      .props = {.launchContext = LaunchContext::object_t{{"openInWindow", true}}},
      .toggleAnchor = toggleAnchor};
  if (!m_context.navigation->activateEntrypoint({"ai", "conversations"}, options)) {
    setError(tr("Could not open conversation search."));
    return;
  }
  if (auto *window = m_context.navigation->window(); window && m_context.navigation->isWindowOpened()) {
    window->raise();
    window->requestActivate();
  }
}

void ChatWindow::clearError() { setError({}); }

void ChatWindow::setError(const QString &error) {
  if (m_error == error) return;
  m_error = error;
  emit errorChanged();
}
