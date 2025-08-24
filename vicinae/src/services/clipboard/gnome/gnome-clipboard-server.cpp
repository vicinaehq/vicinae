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
  qInfo() << "GnomeClipboardServer: Initializing GNOME clipboard server";

  // Set up reconnection timer
  m_reconnectTimer = new QTimer(this);
  m_reconnectTimer->setSingleShot(false);
  m_reconnectTimer->setInterval(5000); // Try reconnection every 5 seconds
  connect(m_reconnectTimer, &QTimer::timeout, this, &GnomeClipboardServer::onReconnectTimer);
}

GnomeClipboardServer::~GnomeClipboardServer() { cleanupDBusConnection(); }

bool GnomeClipboardServer::isActivatable() const {
  const QString envDesc = Environment::getEnvironmentDescription();
  qInfo() << "GnomeClipboardServer: Detected environment:" << envDesc;

  if (!Environment::isGnomeEnvironment()) {
    qInfo() << "GnomeClipboardServer: Not in GNOME environment, skipping";
    return false;
  }

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
                  "https://github.com/MelakuDemeke/vicinae-gnome-extension";
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
  // Signal signature: (stssssts) = string, uint64, string, string, string, string, uint64, string
  bool connected = m_bus.connect(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, "ClipboardChanged", this,
                                 SLOT(handleClipboardChanged(QString, qulonglong, QString, QString, QString,
                                                             QString, qulonglong, QString)));

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
                     SLOT(handleClipboardChanged(QString, qulonglong, QString, QString, QString, QString,
                                                 qulonglong, QString)));

    delete m_interface;
    m_interface = nullptr;
  }

  m_isConnected = false;
}

void GnomeClipboardServer::handleClipboardChanged(const QString &content, qulonglong timestamp,
                                                  const QString &source, const QString &mimeType,
                                                  const QString &contentType, const QString &contentHash,
                                                  qulonglong size, const QString &sourceApp) {
  qDebug() << "GnomeClipboardServer: Received clipboard change from" << sourceApp << "with mime type"
           << mimeType << "and size" << size;

  try {
    // Create ClipboardSelection from D-Bus signal data
    ClipboardSelection selection;

    // Create the main data offer
    ClipboardDataOffer offer;
    offer.mimeType = mimeType;

    // Handle different content types appropriately
    if (mimeType.startsWith("image/") || mimeType.startsWith("application/")) {
      // For images and binary data, handle both data URLs and plain base64
      QString base64String;

      if (content.startsWith("data:")) {
        // Handle data URL format: data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAA...
        int commaIndex = content.indexOf(',');
        if (commaIndex != -1) {
          base64String = content.mid(commaIndex + 1);
          qDebug() << "GnomeClipboardServer: Extracted base64 from data URL";
        } else {
          qWarning() << "GnomeClipboardServer: Invalid data URL format for" << mimeType;
          base64String = content; // Fallback to treating as plain base64
        }
      } else {
        // Plain base64 data
        base64String = content;
        qDebug() << "GnomeClipboardServer: Processing plain base64 data";
      }

      QByteArray base64Content = base64String.toUtf8();
      offer.data = QByteArray::fromBase64(base64Content);

      if (offer.data.isEmpty() && !base64Content.isEmpty()) {
        qWarning() << "GnomeClipboardServer: Failed to decode base64 data for" << mimeType;
        qWarning() << "GnomeClipboardServer: First 100 chars:" << base64String.left(100);
        // Fall back to treating as text
        offer.data = base64Content;
      } else {
        qDebug() << "GnomeClipboardServer: Successfully decoded" << mimeType
                 << "data, original size:" << base64Content.size() << "decoded size:" << offer.data.size()
                 << "bytes";
      }
    } else {
      // For text data, use as-is
      offer.data = content.toUtf8();
      qDebug() << "GnomeClipboardServer: Processing text data (" << mimeType
               << "), size:" << offer.data.size() << "bytes";
    }

    selection.offers.push_back(offer);
    selection.sourceApp = sourceApp.isEmpty() ? std::nullopt : std::optional<QString>(sourceApp);

    // Handle additional common MIME types based on content type
    if (contentType == "text" && mimeType == "text/plain") {
      // For plain text, we might also want to offer text/html if it contains markup
      // This is a simple heuristic - in practice, the extension should provide multiple offers
      if (content.contains("<") && content.contains(">")) {
        ClipboardDataOffer htmlOffer;
        htmlOffer.mimeType = "text/html";
        htmlOffer.data = content.toUtf8();
        selection.offers.push_back(htmlOffer);
      }
    }

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
