#include "gnome-clipboard-server.hpp"
#include "utils/environment.hpp"
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QTimer>
#include <QMetaObject>

GnomeClipboardServer::GnomeClipboardServer() : m_bus(QDBusConnection::sessionBus()) {
  using namespace std::chrono_literals;

  m_reconnectTimer = new QTimer(this);
  m_reconnectTimer->setSingleShot(false);
  m_reconnectTimer->setInterval(5s);
  connect(m_reconnectTimer, &QTimer::timeout, this, &GnomeClipboardServer::onReconnectTimer);
}

GnomeClipboardServer::~GnomeClipboardServer() { cleanupDBusConnection(); }

bool GnomeClipboardServer::isActivatable() const {
  if (!Environment::isGnomeEnvironment()) return false;

  qInfo() << "GnomeClipboardServer: GNOME environment detected, checking for extension...";

  // Check if D-Bus session bus is available
  if (!QDBusConnection::sessionBus().isConnected()) {
    qWarning() << "GnomeClipboardServer: D-Bus session bus not available";
    return false;
  }

  // Test if the vicinae@dagimg-dot extension is available
  if (!testExtensionAvailability()) {
    qWarning() << "GnomeClipboardServer: vicinae@dagimg-dot extension not installed or not running";
    qWarning() << "GnomeClipboardServer: Please install the GNOME extension for clipboard support";
    qWarning() << "GnomeClipboardServer: Extension available at: "
                  "https://github.com/dagimg-dot/vicinae-gnome-extension";
    qWarning() << "GnomeClipboardServer: Falling back to dummy clipboard server";
    return false;
  }

  qInfo() << "GnomeClipboardServer: GNOME extension detected and available";
  return true;
}

int GnomeClipboardServer::activationPriority() const {
  // Higher priority than WlrClipboardServer (15) since GNOME can't use wlr
  return 20;
}

QString GnomeClipboardServer::id() const { return "gnome-clipboard"; }

bool GnomeClipboardServer::isAlive() const { return m_isConnected && m_interface && m_interface->isValid(); }

bool GnomeClipboardServer::start() {
  qInfo() << "GnomeClipboardServer: Starting GNOME clipboard server...";

  if (!setupDBusConnection()) {
    qCritical() << "GnomeClipboardServer: Failed to set up D-Bus connection";
    return false;
  }

  qInfo() << "GnomeClipboardServer: Successfully started and listening for clipboard changes";
  return true;
}

bool GnomeClipboardServer::testExtensionAvailability() const {
  // Test if the extension D-Bus interface exists
  QDBusInterface testInterface(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, QDBusConnection::sessionBus());

  if (!testInterface.isValid()) {
    qDebug() << "GnomeClipboardServer: Extension interface test failed:"
             << testInterface.lastError().message();
    return false;
  }

  // Try to call a simple method to verify the interface is working
  QDBusReply<QStringList> reply = testInterface.call("GetClipboardMimeTypes");
  if (!reply.isValid()) {
    qDebug() << "GnomeClipboardServer: Extension method call test failed:" << reply.error().message();
    return false;
  }

  return true;
}

bool GnomeClipboardServer::setupDBusConnection() {
  cleanupDBusConnection();

  // Check D-Bus connection
  if (!m_bus.isConnected()) {
    qWarning() << "GnomeClipboardServer: D-Bus session bus not connected";
    return false;
  }

  // Create D-Bus interface
  m_interface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, m_bus, this);

  if (!m_interface->isValid()) {
    qWarning() << "GnomeClipboardServer: Failed to create D-Bus interface:"
               << m_interface->lastError().message();
    delete m_interface;
    m_interface = nullptr;
    return false;
  }

  // Connect to ClipboardChanged signal
  // Signal signature: (ayss) = array of bytes, string, string
  bool connected = m_bus.connect(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, "ClipboardChanged", this,
                                 SLOT(handleClipboardChanged(QByteArray, QString, QString)));

  if (!connected) {
    qWarning() << "GnomeClipboardServer: Failed to connect to ClipboardChanged signal";
    delete m_interface;
    m_interface = nullptr;
    return false;
  }

  // Start listening to clipboard changes via the extension
  QDBusReply<void> reply = m_interface->call("ListenToClipboardChanges");
  if (!reply.isValid()) {
    qWarning() << "GnomeClipboardServer: Failed to start listening to clipboard changes:"
               << reply.error().message();
    delete m_interface;
    m_interface = nullptr;
    return false;
  }

  m_isConnected = true;
  qInfo() << "GnomeClipboardServer: D-Bus connection established successfully";

  return true;
}

void GnomeClipboardServer::cleanupDBusConnection() {
  if (m_reconnectTimer) { m_reconnectTimer->stop(); }

  if (m_interface) {
    // Stop listening to clipboard changes
    m_interface->call("StopListening");

    // Disconnect from D-Bus signal
    m_bus.disconnect(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, "ClipboardChanged", this,
                     SLOT(handleClipboardChanged(QByteArray, QString, QString)));

    delete m_interface;
    m_interface = nullptr;
  }

  m_isConnected = false;
}

void GnomeClipboardServer::handleClipboardChanged(const QByteArray &content, const QString &mimeType,
                                                  const QString &sourceApp) {
  qDebug() << "GnomeClipboardServer: Received clipboard change from" << sourceApp << "with mime type"
           << mimeType << "and size" << content.size() << "bytes";

  try {
    ClipboardSelection selection;

    ClipboardDataOffer offer;
    offer.mimeType = mimeType;
    offer.data = content;

    qDebug() << "GnomeClipboardServer: Processing binary data (" << mimeType
             << "), size:" << offer.data.size() << "bytes";

    selection.offers.push_back(offer);
    selection.sourceApp = sourceApp.isEmpty() ? std::nullopt : std::optional<QString>(sourceApp);

    // Emit the selection to the clipboard service
    emit selectionAdded(selection);

    qDebug() << "GnomeClipboardServer: Successfully processed clipboard change from" << sourceApp;

  } catch (const std::exception &e) {
    qWarning() << "GnomeClipboardServer: Error processing clipboard change:" << e.what();
  } catch (...) { qWarning() << "GnomeClipboardServer: Unknown error processing clipboard change"; }
}

void GnomeClipboardServer::handleDBusDisconnection() {
  qWarning() << "GnomeClipboardServer: D-Bus connection lost, attempting to reconnect...";
  m_isConnected = false;

  if (!m_reconnectTimer->isActive()) { m_reconnectTimer->start(); }
}

void GnomeClipboardServer::onReconnectTimer() {
  qInfo() << "GnomeClipboardServer: Attempting to reconnect to D-Bus...";

  if (testExtensionAvailability() && setupDBusConnection()) {
    qInfo() << "GnomeClipboardServer: Reconnection successful";
    m_reconnectTimer->stop();
  } else {
    qDebug() << "GnomeClipboardServer: Reconnection failed, will retry...";
  }
}

void GnomeClipboardServer::attemptReconnection() {
  if (!isAlive() && !m_reconnectTimer->isActive()) {
    qWarning() << "GnomeClipboardServer: Connection lost, starting reconnection attempts";
    m_reconnectTimer->start();
  }
}

bool GnomeClipboardServer::setClipboardContent(QMimeData *data) {
  if (!m_interface || !m_interface->isValid()) {
    qWarning() << "GnomeClipboardServer: D-Bus interface not available";
    return false;
  }

  QStringList formats = data->formats();
  QString preferredFormat = selectPreferredFormat(formats);

  if (preferredFormat.isEmpty()) {
    qWarning() << "GnomeClipboardServer: No supported formats in QMimeData";
    return false;
  }

  QByteArray contentData = data->data(preferredFormat);
  if (contentData.isEmpty()) {
    qWarning() << "GnomeClipboardServer: Empty data for format" << preferredFormat;
    return false;
  }

  qDebug() << "GnomeClipboardServer: Setting clipboard content" << preferredFormat << contentData.size()
           << "bytes";

  return preferredFormat.startsWith("text/") ? setTextContent(data->text())
                                             : setBinaryContent(contentData, preferredFormat);
}

QString GnomeClipboardServer::selectPreferredFormat(const QStringList &formats) {
  if (formats.contains("text/plain")) return "text/plain";
  if (formats.contains("text/html")) return "text/html";

  for (const QString &format : formats) {
    if (format.startsWith("image/")) return format;
  }

  return formats.isEmpty() ? QString() : formats.first();
}

bool GnomeClipboardServer::setTextContent(const QString &text) {
  QDBusReply<void> reply = m_interface->call("SetContent", text);
  if (!reply.isValid()) {
    qWarning() << "GnomeClipboardServer: Failed to set text content:" << reply.error().message();
    return false;
  }
  return true;
}

bool GnomeClipboardServer::setBinaryContent(const QByteArray &data, const QString &mimeType) {
  QDBusReply<void> reply = m_interface->call("SetContentBinary", data, mimeType);
  if (!reply.isValid()) {
    qWarning() << "GnomeClipboardServer: Failed to set binary content:" << reply.error().message();
    return false;
  }
  return true;
}
