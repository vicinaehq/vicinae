#include "gnome-window-manager.hpp"
#include "services/window-manager/gnome/gnome-workspace.hpp"
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
  qDebug() << "GnomeWindowManager: Initializing GNOME window manager";

  m_eventListener = std::make_unique<Gnome::EventListener>();

  // Connect event listener signals to our windowsChanged signal
  connect(m_eventListener.get(), &Gnome::EventListener::openwindow, this, [this]() {
    qDebug() << "GnomeWindowManager: Received openwindow event, emitting windowsChanged";
    emit windowsChanged();
  });

  connect(m_eventListener.get(), &Gnome::EventListener::closewindow, this, [this]() {
    qDebug() << "GnomeWindowManager: Received closewindow event, emitting windowsChanged";
    emit windowsChanged();
  });

  connect(m_eventListener.get(), &Gnome::EventListener::focuswindow, this,
          [this](const Gnome::WindowAddress &addr) {
            qDebug() << "GnomeWindowManager: Received focuswindow event for window" << addr
                     << ", emitting windowsChanged";
            emit windowsChanged();
          });

  connect(m_eventListener.get(), &Gnome::EventListener::movewindow, this, [this]() {
    qDebug() << "GnomeWindowManager: Received movewindow event, emitting windowsChanged";
    emit windowsChanged();
  });

  connect(m_eventListener.get(), &Gnome::EventListener::statewindow, this, [this]() {
    qDebug() << "GnomeWindowManager: Received statewindow event, emitting windowsChanged";
    emit windowsChanged();
  });

  connect(m_eventListener.get(), &Gnome::EventListener::workspacechanged, this, [this]() {
    qDebug() << "GnomeWindowManager: Received workspacechanged event, emitting windowsChanged";
    emit windowsChanged();
  });

  connect(m_eventListener.get(), &Gnome::EventListener::monitorlayoutchanged, this, [this]() {
    qDebug() << "GnomeWindowManager: Received monitorlayoutchanged event, emitting windowsChanged";
    emit windowsChanged();
  });
}

GnomeWindowManager::~GnomeWindowManager() {
  // Event listener will be automatically cleaned up
  qDebug() << "GnomeWindowManager: Destroyed";
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

AbstractWindowManager::WindowList GnomeWindowManager::findAppWindowsGnome(const Application &app) const {
  // Use fresh window data for GNOME to ensure active window dots work correctly
  auto freshWindows = listWindowsSync();

  auto pred = [&](auto &&win) {
    QString winWmClass = win->wmClass().toLower();
    bool matches = false;

    // Check all app window classes
    for (const auto &appClass : app.windowClasses()) {
      QString appClassLower = appClass.toLower();

      // Exact match
      if (appClassLower == winWmClass) {
        matches = true;
        break;
      }

      // GNOME-specific: Handle .desktop suffix mismatches
      // GNOME reports with .desktop, app expects without
      // e.g., GNOME: "org.gnome.Nautilus.desktop" vs App: "org.gnome.Nautilus"
      if (winWmClass.endsWith(".desktop") && appClassLower == winWmClass.chopped(8)) {
        matches = true;
        break;
      }

      // Reverse: GNOME reports without .desktop, app has it
      // e.g., GNOME: "equibop" vs App: "equibop.desktop"
      if (appClassLower.endsWith(".desktop") && winWmClass == appClassLower.chopped(8)) {
        matches = true;
        break;
      }
    }

    return matches;
  };

  return freshWindows | std::views::filter(pred) | std::ranges::to<std::vector>();
}

AbstractWindowManager::WindowList GnomeWindowManager::findWindowByClassGnome(const QString &wmClass) const {
  // Use fresh window data for GNOME
  auto freshWindows = listWindowsSync();

  auto pred = [&](auto &&win) {
    QString winWmClass = win->wmClass().toLower();
    QString searchClass = wmClass.toLower();

    // Exact match
    if (winWmClass == searchClass) return true;

    // GNOME-specific: Handle .desktop suffix mismatches
    if (winWmClass.endsWith(".desktop") && searchClass == winWmClass.chopped(8)) return true;
    if (searchClass.endsWith(".desktop") && winWmClass == searchClass.chopped(8)) return true;

    return false;
  };

  return freshWindows | std::views::filter(pred) | std::ranges::to<std::vector>();
}
