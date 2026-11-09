#pragma once
#include <QObject>
#include <QPointer>
#include <QString>

class ApplicationContext;
class ChatSession;
class ChatWindow;

/**
 * Opens the chat window on demand. The window, its QML engine and its session are released as soon as it
 * is closed.
 */
class ChatWindowController : public QObject {
  Q_OBJECT

public:
  explicit ChatWindowController(ApplicationContext &ctx);

  /** Shows the window, on the most recent conversation when it wasn't open. */
  void show();
  /** Shows the window on a session handed over by another presenter, which the window takes ownership of. */
  void show(ChatSession *session);
  void openConversation(const QString &id);

  /** The session of the open window, if any. */
  ChatSession *session() const;

private:
  ChatWindow &window();

  ApplicationContext &m_ctx;
  QPointer<ChatWindow> m_window;
};
