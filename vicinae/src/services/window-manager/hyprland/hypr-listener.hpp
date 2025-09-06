#pragma once
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qstringview.h>
#include <qtmetamacros.h>
namespace Hyprland {
using WindowAddress = QString;

/**
 * https://wiki.hypr.land/IPC/
 * .socket2.sock
 */
class EventListener : public QObject {
signals:
  void openwindow(const WindowAddress &addr, const QString &workspaceName, const QString &wmClass,
                  const QString &title) const;
  void closewindow(const WindowAddress &addr) const;

public:
  EventListener();

  bool start();

private:
  void handleRead();
  void processEvent(const std::string &event);

  QSocketNotifier *m_notifier = new QSocketNotifier(QSocketNotifier::Type::Read, this);
  std::array<char, 1 << 16> m_buf;
  std::string m_message;

  Q_OBJECT
};

}; // namespace Hyprland
