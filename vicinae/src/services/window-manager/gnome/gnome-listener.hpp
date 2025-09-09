#pragma once
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QTimer>

namespace Gnome {

using WindowAddress = QString;

/**
 * GNOME Event Listener
 * Listens to D-Bus signals from the GNOME Shell Vicinae extension
 * Follows the same pattern as GnomeClipboardServer
 */
class EventListener : public QObject {
  Q_OBJECT

signals:
  void openwindow(const WindowAddress &addr, const QString &workspaceName, const QString &wmClass,
                  const QString &title) const;
  void closewindow(const WindowAddress &addr) const;
  void focuswindow(const WindowAddress &addr) const;
  void movewindow(const WindowAddress &addr, int x, int y, uint width, uint height) const;
  void statewindow(const WindowAddress &addr, const QString &state) const;
  void workspacechanged(const QString &workspaceId) const;
  void monitorlayoutchanged() const;

public:
  EventListener();
  ~EventListener() override;

  bool start();
  bool isActive() const { return m_isActive; }

private slots:
  void onOpenWindowSignal(const QString &windowAddress, const QString &workspaceName, const QString &wmClass,
                          const QString &title);
  void onCloseWindowSignal(const QString &windowAddress);
  void onFocusWindowSignal(const QString &windowAddress);
  void onMoveWindowSignal(const QString &windowAddress, int x, int y, uint width, uint height);
  void onStateWindowSignal(const QString &windowAddress, const QString &state);
  void onWorkspaceChangedSignal(const QString &workspaceId);
  void onMonitorLayoutChangedSignal();
  void onReconnectTimer();

private:
  bool m_isActive = false;
  QString m_service = "org.gnome.Shell";
  QString m_path = "/org/gnome/Shell/Extensions/Windows";
  QString m_interface = "org.gnome.Shell.Extensions.Windows";

  QDBusConnection m_bus;
  QTimer *m_reconnectTimer = nullptr;

  bool setupDBusConnection();
  void cleanupDBusConnection();
  bool testExtensionAvailability() const;
};

} // namespace Gnome
