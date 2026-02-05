#include "gnome-window-manager.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/gnome/gnome-workspace.hpp"
#include "services/window-manager/window-manager.hpp"
#include "utils/environment.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include <QDBusConnection>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QProcess>
#include <QGuiApplication>

GnomeWindowManager::GnomeWindowManager() {
  m_eventListener = std::make_unique<Gnome::EventListener>();

  m_debounceTimer.setSingleShot(true);
  m_debounceTimer.setInterval(50);
  connect(&m_debounceTimer, &QTimer::timeout, this, [this]() {
    if (m_pendingWindowsChanged) {
      m_pendingWindowsChanged = false;
      emit windowsChanged();
    }
  });

  connect(m_eventListener.get(), &Gnome::EventListener::openwindow, this, [this]() {
    m_pendingWindowsChanged = true;
    m_debounceTimer.start();
  });

  connect(m_eventListener.get(), &Gnome::EventListener::closewindow, this,
          [this](const Gnome::WindowAddress &addr) {
            auto wm = ServiceRegistry::instance()->windowManager();
            if (wm->findWindowById(addr)) {
              m_pendingWindowsChanged = true;
              m_debounceTimer.start();
            }
          });

  connect(m_eventListener.get(), &Gnome::EventListener::focuswindow, this,
          [this](const Gnome::WindowAddress &addr) {
            auto wm = ServiceRegistry::instance()->windowManager();
            auto windows = wm->listWindows();

            qDebug() << "GnomeWindowManager: focus update for" << addr;

            for (auto &win : windows) {
              if (win->id() == addr) {
                if (auto gnomeWin = std::dynamic_pointer_cast<GnomeWindow>(win)) {
                  gnomeWin->setFocused(true);

                  for (auto &otherWin : windows) {
                    if (otherWin != win) {
                      if (auto otherGnomeWin = std::dynamic_pointer_cast<GnomeWindow>(otherWin)) {
                        if (otherGnomeWin->focused()) { otherGnomeWin->setFocused(false); }
                      }
                    }
                  }

                  emit windowFocused(addr);
                  return;
                }
              }
            }
          });
}

GnomeWindowManager::~GnomeWindowManager() {}

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

    // Map simple Flatpak IDs to complex path-based IDs for app database matching
    QString originalWmClass = windowObj.value("wm_class").toString();
    QString mappedWmClass = mapToComplexId(originalWmClass);
    if (mappedWmClass != originalWmClass) { window->setMappedWmClass(mappedWmClass); }

    windows.push_back(window);
  }

  qDebug() << "GnomeWindowManager: Found" << windows.size() << "windows";
  return windows;
}

std::shared_ptr<AbstractWindowManager::AbstractWindow> GnomeWindowManager::getFocusedWindowSync() const {
  qDebug() << "GnomeWindowManager: Getting focused window via D-Bus";

  // Use the D-Bus method that handles Vicinae window filtering
  QString response = callDBusMethod("GetFocusedWindowSync");
  if (response.isEmpty()) {
    qWarning() << "GnomeWindowManager: No response from GetFocusedWindowSync method";
    return nullptr;
  }

  QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
  if (doc.isNull() || !doc.isObject()) {
    qWarning() << "GnomeWindowMinto clipboard and window managementanager: Invalid JSON response from "
                  "GetFocusedWindowSync";
    return nullptr;
  }

  QJsonObject windowObj = doc.object();
  if (windowObj.isEmpty()) {
    qDebug() << "GnomeWindowManager: No focused window (null response)";
    return nullptr;
  }

  // Create a GnomeWindow from the response
  auto window = std::make_shared<GnomeWindow>(windowObj);
  qDebug() << "GnomeWindowManager: Found focused window:" << window->title();
  return window;
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
  if (!Environment::isGnomeEnvironment()) return false;

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

void GnomeWindowManager::start() {
  qDebug() << "GnomeWindowManager: Window manager started";

  // Start the event listener
  if (m_eventListener && !m_eventListener->isActive()) {
    if (m_eventListener->start()) {
      qDebug() << "GnomeWindowManager: Event listener started successfully";
    } else {
      qWarning() << "GnomeWindowManager: Failed to start event listener";
    }
  }
}

bool GnomeWindowManager::closeWindow(const AbstractWindow &window) const {
  const GnomeWindow *gnomeWindow = dynamic_cast<const GnomeWindow *>(&window);
  if (!gnomeWindow) return false;

  QVariantList args;
  args << gnomeWindow->numericId();

  if (!callDBusMethodVoid("Close", args)) { return false; }

  emit windowsChanged();

  return true;
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

QString GnomeWindowManager::mapToComplexId(const QString &simpleId) const {
  // Get the app database from the service registry
  auto appDb = ServiceRegistry::instance()->appDb();
  if (!appDb) {
    qWarning() << "GnomeWindowManager: App database not available for ID mapping";
    return simpleId;
  }

  // Find app with complex path-based ID that ends with the simple Flatpak ID
  auto apps = appDb->list();
  for (const auto &app : apps) {
    QString appId = app->id();
    if (appId.endsWith(simpleId + ".desktop") || appId.endsWith(simpleId)) { return appId; }
  }

  return simpleId;
}

AbstractWindowManager::WorkspaceList GnomeWindowManager::listWorkspaces() const {
  qDebug() << "GnomeWindowManager: Listing workspaces";

  QString response = callDBusMethod("ListWorkspaces");
  if (response.isEmpty()) {
    qWarning() << "GnomeWindowManager: No response from ListWorkspaces method";
    return {};
  }

  QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
  if (doc.isNull()) {
    qWarning() << "GnomeWindowManager: Failed to parse JSON response from ListWorkspaces";
    return {};
  }

  QJsonArray workspacesArray;
  if (doc.isArray()) {
    workspacesArray = doc.array();
  } else if (doc.isObject()) {
    // Handle case where response is wrapped in an object
    QJsonObject wrapper = doc.object();
    if (wrapper.contains("workspaces") && wrapper.value("workspaces").isArray()) {
      workspacesArray = wrapper.value("workspaces").toArray();
    }
  }

  if (workspacesArray.isEmpty()) {
    qDebug() << "GnomeWindowManager: No workspaces found";
    return {};
  }

  WorkspaceList workspaces;
  workspaces.reserve(workspacesArray.size());

  for (const QJsonValue &workspaceValue : workspacesArray) {
    if (!workspaceValue.isObject()) {
      qWarning() << "GnomeWindowManager: Invalid workspace object in response";
      continue;
    }

    QJsonObject workspaceObj = workspaceValue.toObject();
    auto workspace = std::make_shared<Gnome::Workspace>(workspaceObj);
    workspaces.push_back(workspace);
  }

  qDebug() << "GnomeWindowManager: Found" << workspaces.size() << "workspaces";
  return workspaces;
}

std::shared_ptr<AbstractWindowManager::AbstractWorkspace> GnomeWindowManager::getActiveWorkspace() const {
  qDebug() << "GnomeWindowManager: Getting active workspace";

  QString response = callDBusMethod("GetActiveWorkspace");
  if (response.isEmpty()) {
    qWarning() << "GnomeWindowManager: No response from GetActiveWorkspace method";
    return nullptr;
  }

  QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
  if (doc.isNull()) {
    qWarning() << "GnomeWindowManager: Failed to parse JSON response from GetActiveWorkspace";
    return nullptr;
  }

  QJsonObject workspaceObj;
  if (doc.isObject()) {
    workspaceObj = doc.object();
  } else {
    qWarning() << "GnomeWindowManager: Expected JSON object for workspace";
    return nullptr;
  }

  if (workspaceObj.isEmpty()) {
    qWarning() << "GnomeWindowManager: Invalid workspace object in response";
    return nullptr;
  }

  auto workspace = std::make_shared<Gnome::Workspace>(workspaceObj);
  qDebug() << "GnomeWindowManager: Found active workspace:" << workspace->name();
  return workspace;
}

bool GnomeWindowManager::pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) {
  if (app && app->isTerminalEmulator()) {
    return sendShortcutSync(*window, Keyboard::Shortcut::osPaste().shifted());
  }
  return sendShortcutSync(*window, Keyboard::Shortcut::osPaste());
}

bool GnomeWindowManager::sendShortcutSync(const AbstractWindow &window, const Keyboard::Shortcut &shortcut) {
  qDebug() << "GnomeWindowManager: Sending shortcut to window:" << window.title();

  // Cast to GnomeWindow to get numeric ID
  const GnomeWindow *gnomeWindow = dynamic_cast<const GnomeWindow *>(&window);
  if (!gnomeWindow) {
    qWarning() << "GnomeWindowManager: Window is not a GnomeWindow instance";
    return false;
  }

  uint32_t windowId = gnomeWindow->numericId();
  if (windowId == 0) {
    qWarning() << "GnomeWindowManager: Invalid window ID";
    return false;
  }

  // Convert modifiers to string format expected by GNOME extension
  QStringList modStrings;
  if (shortcut.mods().testFlag(Qt::KeyboardModifier::ControlModifier)) { modStrings << "CONTROL"; }
  if (shortcut.mods().testFlag(Qt::KeyboardModifier::ShiftModifier)) { modStrings << "SHIFT"; }
  if (shortcut.mods().testFlag(Qt::KeyboardModifier::AltModifier)) { modStrings << "ALT"; }
  if (shortcut.mods().testFlag(Qt::KeyboardModifier::MetaModifier)) { modStrings << "SUPER"; }

  QString modifiersStr = modStrings.join("|");
  if (modifiersStr.isEmpty()) { modifiersStr = "NONE"; }

  // Convert Qt key to string
  QString keyStr = QKeySequence(shortcut.key()).toString().toLower();

  qDebug() << "GnomeWindowManager: Sending shortcut - key:" << keyStr << "modifiers:" << modifiersStr
           << "to window ID:" << windowId;

  QVariantList args;
  args << windowId << keyStr << modifiersStr;

  bool success = callDBusMethodVoid("SendShortcut", args);
  if (success) {
    qDebug() << "GnomeWindowManager: Successfully sent shortcut to window ID:" << windowId;
  } else {
    qWarning() << "GnomeWindowManager: Failed to send shortcut to window ID:" << windowId;
  }

  return success;
}
