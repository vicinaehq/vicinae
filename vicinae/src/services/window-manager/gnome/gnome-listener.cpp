#include "gnome-listener.hpp"
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QLoggingCategory>
#include <chrono>

namespace Gnome {

EventListener::EventListener() : m_bus(QDBusConnection::sessionBus()) {
  using namespace std::chrono_literals;

  // Initialize reconnection timer (same pattern as GnomeClipboardServer)
  m_reconnectTimer = new QTimer(this);
  m_reconnectTimer->setSingleShot(false);
  m_reconnectTimer->setInterval(5s); // 5 seconds like clipboard server
  connect(m_reconnectTimer, &QTimer::timeout, this, &EventListener::onReconnectTimer);
}

EventListener::~EventListener() { cleanupDBusConnection(); }

bool EventListener::start() {
  qInfo() << "GnomeEventListener: Starting GNOME event listener...";

  if (!setupDBusConnection()) {
    qCritical() << "GnomeEventListener: Failed to set up D-Bus connection";
    return false;
  }

  qInfo() << "GnomeEventListener: Successfully started and listening for window events";
  return true;
}

bool EventListener::setupDBusConnection() {
  cleanupDBusConnection();

  // Check D-Bus connection
  if (!m_bus.isConnected()) {
    qWarning() << "GnomeEventListener: D-Bus session bus not connected";
    return false;
  }

  // Test if the GNOME extension is available
  if (!testExtensionAvailability()) {
    qWarning() << "GnomeEventListener: GNOME extension not available";
    return false;
  }

  // Connect to all window signals
  bool openConnected = m_bus.connect(m_service, m_path, m_interface, "openwindow", this,
                                     SLOT(onOpenWindowSignal(QString, QString, QString, QString)));
  bool closeConnected =
      m_bus.connect(m_service, m_path, m_interface, "closewindow", this, SLOT(onCloseWindowSignal(QString)));
  bool focusConnected =
      m_bus.connect(m_service, m_path, m_interface, "focuswindow", this, SLOT(onFocusWindowSignal(QString)));
  bool moveConnected = m_bus.connect(m_service, m_path, m_interface, "movewindow", this,
                                     SLOT(onMoveWindowSignal(QString, int, int, uint, uint)));
  bool stateConnected = m_bus.connect(m_service, m_path, m_interface, "statewindow", this,
                                      SLOT(onStateWindowSignal(QString, QString)));
  bool workspaceConnected = m_bus.connect(m_service, m_path, m_interface, "workspacechanged", this,
                                          SLOT(onWorkspaceChangedSignal(QString)));
  bool monitorConnected = m_bus.connect(m_service, m_path, m_interface, "monitorlayoutchanged", this,
                                        SLOT(onMonitorLayoutChangedSignal()));

  if (!openConnected || !closeConnected || !focusConnected || !moveConnected || !stateConnected ||
      !workspaceConnected || !monitorConnected) {
    qWarning() << "GnomeEventListener: Failed to connect to one or more D-Bus signals";
    return false;
  }

  m_isActive = true;
  qInfo() << "GnomeEventListener: D-Bus connection established successfully";
  return true;
}

void EventListener::cleanupDBusConnection() {
  if (m_reconnectTimer) { m_reconnectTimer->stop(); }

  if (m_isActive) {
    // Disconnect from all D-Bus signals
    m_bus.disconnect(m_service, m_path, m_interface, "openwindow", this,
                     SLOT(onOpenWindowSignal(QString, QString, QString, QString)));
    m_bus.disconnect(m_service, m_path, m_interface, "closewindow", this, SLOT(onCloseWindowSignal(QString)));
    m_bus.disconnect(m_service, m_path, m_interface, "focuswindow", this, SLOT(onFocusWindowSignal(QString)));
    m_bus.disconnect(m_service, m_path, m_interface, "movewindow", this,
                     SLOT(onMoveWindowSignal(QString, int, int, uint, uint)));
    m_bus.disconnect(m_service, m_path, m_interface, "statewindow", this,
                     SLOT(onStateWindowSignal(QString, QString)));
    m_bus.disconnect(m_service, m_path, m_interface, "workspacechanged", this,
                     SLOT(onWorkspaceChangedSignal(QString)));
    m_bus.disconnect(m_service, m_path, m_interface, "monitorlayoutchanged", this,
                     SLOT(onMonitorLayoutChangedSignal()));
  }

  m_isActive = false;
}

bool EventListener::testExtensionAvailability() const {
  // Test if the GNOME extension interface exists
  QDBusInterface testInterface(m_service, m_path, m_interface, m_bus);

  if (!testInterface.isValid()) {
    qDebug() << "GnomeEventListener: Extension interface test failed:" << testInterface.lastError().message();
    return false;
  }

  // Try to call a simple method to verify the interface is working
  QDBusReply<QString> reply = testInterface.call("List");
  if (!reply.isValid()) {
    qDebug() << "GnomeEventListener: Extension method call test failed:" << reply.error().message();
    return false;
  }

  return true;
}

void EventListener::onOpenWindowSignal(const QString &windowAddress, const QString &workspaceName,
                                       const QString &wmClass, const QString &title) {
  qDebug() << "GnomeEventListener: Window opened:" << title << "(" << wmClass << ")";
  emit openwindow(windowAddress, workspaceName, wmClass, title);
}

void EventListener::onCloseWindowSignal(const QString &windowAddress) {
  qDebug() << "GnomeEventListener: Window closed:" << windowAddress;
  emit closewindow(windowAddress);
}

void EventListener::onFocusWindowSignal(const QString &windowAddress) {
  qDebug() << "GnomeEventListener: Window focused:" << windowAddress;
  emit focuswindow(windowAddress);
}

void EventListener::onMoveWindowSignal(const QString &windowAddress, int x, int y, uint width, uint height) {
  qDebug() << "GnomeEventListener: Window moved:" << windowAddress
           << QString("(%1,%2 %3x%4)").arg(x).arg(y).arg(width).arg(height);
  emit movewindow(windowAddress, x, y, width, height);
}

void EventListener::onStateWindowSignal(const QString &windowAddress, const QString &state) {
  qDebug() << "GnomeEventListener: Window state changed:" << windowAddress << "->" << state;
  emit statewindow(windowAddress, state);
}

void EventListener::onWorkspaceChangedSignal(const QString &workspaceId) {
  qDebug() << "GnomeEventListener: Active workspace changed:" << workspaceId;
  emit workspacechanged(workspaceId);
}

void EventListener::onMonitorLayoutChangedSignal() {
  qDebug() << "GnomeEventListener: Monitor layout changed";
  emit monitorlayoutchanged();
}

void EventListener::onReconnectTimer() {
  qInfo() << "GnomeEventListener: Attempting to reconnect to D-Bus...";

  if (testExtensionAvailability() && setupDBusConnection()) {
    qInfo() << "GnomeEventListener: Reconnection successful";
    m_reconnectTimer->stop();
  } else {
    qDebug() << "GnomeEventListener: Reconnection failed, will retry...";
  }
}

} // namespace Gnome
