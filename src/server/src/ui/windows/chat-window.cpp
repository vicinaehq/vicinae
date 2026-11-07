#include <QQuickWindow>
#include "chat-window.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/bridges/config-bridge.hpp"
#include "ui/qml-dev-loader.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/views/view-utils.hpp"
#ifdef Q_OS_MACOS
#include "ui/quick/macos-chrome-attached.hpp"
#endif

ChatWindow::ChatWindow(ApplicationContext &context, QObject *parent)
    : QObject(parent), m_context(context), m_conversations(context.services->conversations()) {
  connect(&m_conversations, &ConversationListModel::errorOccurred, this, &ChatWindow::setError);
  connect(context.services->conversations(), &AI::ConversationStore::removed, this,
          [this](const QString &id) {
            if (m_session && m_session->conversationId() == id) newChat();
          });

  newChat();
  QmlEngineScope::set(&m_engine, this);
  QmlEngineScope::set(&m_engine, new ConfigBridge(ConfigBridge::OpaqueSurfaces, this));
  QmlDevLoader::attach(&m_engine, [this] {
    const auto geometry = m_window ? m_window->geometry() : QRect{};
    const bool visible = m_window && m_window->isVisible();
    if (m_window) m_window->deleteLater();
    m_window = nullptr;
    loadRoot();
    if (m_window && geometry.isValid()) m_window->setGeometry(geometry);
    if (visible) present();
  });
  loadRoot();
  m_conversations.reload();
}

ChatWindow::~ChatWindow() {
  cancelPending();
  if (m_session) m_session->cancel();
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
  if (!m_window) return;
  m_window->setProperty(PasteService::PASTE_TARGET_PROPERTY, true);
  connect(m_window, &QQuickWindow::closing, this, &ChatWindow::closed);
}

void ChatWindow::present() {
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
#ifdef Q_OS_MACOS
  macosActivateApp();
#endif
  emit focusRequested();
}

void ChatWindow::openMostRecent() {
  m_context.services->conversations()->list().then(
      this, [this](const AI::Result<std::vector<AI::ConversationInfo>> &result) {
        if (!result || result->empty()) return;
        // the user may have started a new chat while the list was loading
        if (!m_session->conversationId().isEmpty() || !m_session->draft().isEmpty()) return;
        openConversation(QString::fromStdString(result->front().id));
      });
}

void ChatWindow::adopt(ChatSession *session) {
  if (session) activateSession(session);
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
  session->setParent(this);
  if (m_session == session) {
    emit focusRequested();
    return;
  }
  if (auto *previous = m_session.data()) {
    previous->cancel();
    previous->deleteLater();
  }
  m_session = session;
  emit sessionChanged();
}

void ChatWindow::newChat() {
  if (m_session && m_session->conversationId().isEmpty() && !m_session->loading()) {
    cancelPending();
    emit focusRequested();
    return;
  }
  activateSession(new ChatSession(*m_context.services, this));
}

void ChatWindow::openConversation(const QString &id) {
  if (id.isEmpty()) return;
  if (m_session->conversationId() == id) {
    cancelPending();
    emit focusRequested();
    return;
  }
  cancelPending();
  clearError();
  auto *session = new ChatSession(*m_context.services, this);
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
