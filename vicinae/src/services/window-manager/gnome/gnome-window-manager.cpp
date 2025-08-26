#include "gnome-window-manager.hpp"
#include "utils/environment.hpp"
#include <QDBusConnection>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QProcess>
#include <QGuiApplication>

GnomeWindowManager::GnomeWindowManager() {
  qDebug() << "GnomeWindowManager: Initializing GNOME window manager";
}

QDBusInterface *GnomeWindowManager::getDBusInterface() const {
  if (!m_dbusInterface) {
    m_dbusInterface = std::make_unique<QDBusInterface>(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,
                                                       QDBusConnection::sessionBus());

    if (!m_dbusInterface->isValid()) {
      qWarning() << "GnomeWindowManager: Failed to create D-Bus interface:"
                 << m_dbusInterface->lastError().message();
    } else {
      qDebug() << "GnomeWindowManager: D-Bus interface created successfully";
    }
  }

  return m_dbusInterface.get();
}

QString GnomeWindowManager::callDBusMethod(const QString &method, const QVariantList &args) const {
  auto *interface = getDBusInterface();
  if (!interface || !interface->isValid()) {
    qWarning() << "GnomeWindowManager: D-Bus interface not available for method:" << method;
    return QString();
  }

  QDBusReply<QString> reply = interface->callWithArgumentList(QDBus::Block, method, args);

  if (!reply.isValid()) {
    qWarning() << "GnomeWindowManager: D-Bus call failed for method:" << method
               << "Error:" << reply.error().message();
    return QString();
  }

  return reply.value();
}

bool GnomeWindowManager::callDBusMethodVoid(const QString &method, const QVariantList &args) const {
  auto *interface = getDBusInterface();
  if (!interface || !interface->isValid()) {
    qWarning() << "GnomeWindowManager: D-Bus interface not available for method:" << method;
    return false;
  }

  QDBusReply<void> reply = interface->callWithArgumentList(QDBus::Block, method, args);

  if (!reply.isValid()) {
    qWarning() << "GnomeWindowManager: D-Bus call failed for method:" << method
               << "Error:" << reply.error().message();
    return false;
  }

  return true;
}

QJsonObject GnomeWindowManager::parseJsonResponse(const QString &response) const {
  if (response.isEmpty()) {
    qWarning() << "GnomeWindowManager: Empty response received";
    return QJsonObject();
  }

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8(), &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qWarning() << "GnomeWindowManager: JSON parse error:" << parseError.errorString()
               << "Response:" << response;
    return QJsonObject();
  }

  return doc.object();
}

QJsonArray GnomeWindowManager::parseJsonArrayResponse(const QString &response) const {
  if (response.isEmpty()) {
    qWarning() << "GnomeWindowManager: Empty response received";
    return QJsonArray();
  }

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8(), &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qWarning() << "GnomeWindowManager: JSON parse error:" << parseError.errorString()
               << "Response:" << response;
    return QJsonArray();
  }

  return doc.array();
}

AbstractWindowManager::WindowList GnomeWindowManager::listWindowsSync() const {
  qDebug() << "GnomeWindowManager: Listing windows";

  QString response = callDBusMethod("List");
  if (response.isEmpty()) {
    qWarning() << "GnomeWindowManager: No response from List method";
    return {};
  }

  QJsonArray windowsArray = parseJsonArrayResponse(response);
  if (windowsArray.isEmpty()) {
    qDebug() << "GnomeWindowManager: No windows found";
    return {};
  }

  WindowList windows;
  windows.reserve(windowsArray.size());

  for (const QJsonValue &windowValue : windowsArray) {
    if (!windowValue.isObject()) {
      qWarning() << "GnomeWindowManager: Invalid window object in response";
      continue;
    }

    QJsonObject windowObj = windowValue.toObject();
    auto window = std::make_shared<GnomeWindow>(windowObj);
    windows.push_back(window);
  }

  qDebug() << "GnomeWindowManager: Found" << windows.size() << "windows";
  return windows;
}

std::shared_ptr<AbstractWindowManager::AbstractWindow> GnomeWindowManager::getFocusedWindowSync() const {
  qDebug() << "GnomeWindowManager: Getting focused window";

  // Get all windows and find the focused one
  auto windows = listWindowsSync();

  for (const auto &window : windows) {
    // Cast to GnomeWindow to access GNOME-specific properties
    if (auto gnomeWindow = std::dynamic_pointer_cast<GnomeWindow>(window)) {
      if (gnomeWindow->focused()) {
        qDebug() << "GnomeWindowManager: Found focused window:" << gnomeWindow->title();
        return gnomeWindow;
      }
    }
  }

  qDebug() << "GnomeWindowManager: No focused window found";
  return nullptr;
}

void GnomeWindowManager::focusWindowSync(const AbstractWindow &window) const {
  qDebug() << "GnomeWindowManager: Focusing window:" << window.title();

  // Cast to GnomeWindow to get numeric ID
  const GnomeWindow *gnomeWindow = dynamic_cast<const GnomeWindow *>(&window);
  if (!gnomeWindow) {
    qWarning() << "GnomeWindowManager: Window is not a GnomeWindow instance";
    return;
  }

  uint32_t windowId = gnomeWindow->numericId();
  if (windowId == 0) {
    qWarning() << "GnomeWindowManager: Invalid window ID";
    return;
  }

  qDebug() << "GnomeWindowManager: Attempting to activate window ID:" << windowId;

  QVariantList args;
  args << windowId;

  bool success = callDBusMethodVoid("Activate", args);
  if (success) {
    qDebug() << "GnomeWindowManager: Successfully sent activate request for window ID:" << windowId;
  } else {
    qWarning() << "GnomeWindowManager: Failed to activate window ID:" << windowId;
  }
}

bool GnomeWindowManager::isActivatable() const {
  const QString envDesc = Environment::getEnvironmentDescription();
  qInfo() << "GnomeWindowManager: Detected environment:" << envDesc;

  // Check if we're running on GNOME
  if (!Environment::isGnomeEnvironment()) {
    qInfo() << "GnomeWindowManager: Not in GNOME environment, skipping";
    return false;
  }

  // Test D-Bus connectivity
  auto *interface = getDBusInterface();
  if (!interface || !interface->isValid()) {
    qDebug() << "GnomeWindowManager: D-Bus interface not available";
    return false;
  }

  // Try a simple ping by calling List method
  QString response = callDBusMethod("List");
  bool available = !response.isEmpty();

  qDebug() << "GnomeWindowManager: Activation check result:" << available;
  return available;
}

bool GnomeWindowManager::ping() const {
  // Simple health check by calling List method
  QString response = callDBusMethod("List");
  return !response.isEmpty();
}

void GnomeWindowManager::start() const {
  qDebug() << "GnomeWindowManager: Window manager started";
  // No special startup required for GNOME integration
}

bool GnomeWindowManager::closeWindow(const AbstractWindow &window) const {
  const GnomeWindow *gnomeWindow = dynamic_cast<const GnomeWindow *>(&window);
  if (!gnomeWindow) return false;

  QVariantList args;
  args << gnomeWindow->numericId();

  return callDBusMethodVoid("Close", args);
}

std::shared_ptr<GnomeWindow> GnomeWindowManager::getWindowDetails(uint32_t windowId) const {
  QVariantList args;
  args << windowId;

  QString response = callDBusMethod("Details", args);
  if (response.isEmpty()) { return nullptr; }

  QJsonObject detailsObj = parseJsonResponse(response);
  if (detailsObj.isEmpty()) { return nullptr; }

  // Create a basic window from the details and then update it
  auto window = std::make_shared<GnomeWindow>(detailsObj);
  window->updateWithDetails(detailsObj);

  return window;
}
