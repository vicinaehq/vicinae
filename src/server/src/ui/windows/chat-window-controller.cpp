#include "chat-window-controller.hpp"
#include "ui/windows/chat-window.hpp"

ChatWindowController::ChatWindowController(ApplicationContext &ctx) : m_ctx(ctx) {}

ChatWindow &ChatWindowController::window() {
  if (!m_window) {
    m_window = new ChatWindow(m_ctx, this);
    connect(m_window, &ChatWindow::closed, m_window, &QObject::deleteLater);
  }
  return *m_window;
}

void ChatWindowController::show() {
  const bool opened = !m_window;
  auto &chat = window();
  if (opened) chat.openMostRecent();
  chat.present();
}

void ChatWindowController::show(ChatSession *session) {
  auto &chat = window();
  chat.adopt(session);
  chat.present();
}

void ChatWindowController::openConversation(const QString &id) {
  auto &chat = window();
  chat.openConversation(id);
  chat.present();
}

ChatSession *ChatWindowController::session() const { return m_window ? m_window->session() : nullptr; }
