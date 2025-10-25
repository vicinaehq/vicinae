#include "x11-window-manager.hpp"
#include "x11-event-listener.hpp"
#include "x11-window.hpp"
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QtCore/qnativeinterface.h>
#include <qlogging.h>
#include <qnumeric.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <cstring>

X11WindowManager::X11WindowManager() : m_connection(nullptr), m_screen(nullptr), m_ewmhSupported(false) {}

X11WindowManager::~X11WindowManager() {
  // Connection is owned by Qt's xcb platform, don't disconnect
  m_connection = nullptr;
  m_screen = nullptr;
}

bool X11WindowManager::isActivatable() const {
  // Check if we're running on X11 (xcb platform)
  return QGuiApplication::platformName() == "xcb";
}

void X11WindowManager::start() {
  // Initialize connection
  m_connection = getConnection();
  m_screen = getScreen();

  if (!m_connection || !m_screen) {
    qWarning() << "X11WindowManager: Failed to connect to X server";
    return;
  }

  // Check for EWMH support
  m_ewmhSupported = checkEWMHSupport();

  if (!m_ewmhSupported) {
    qWarning() << "X11WindowManager: Window manager does not support EWMH";
    qWarning() << "X11WindowManager: Some features may not work correctly";
  } else {
    qDebug() << "X11WindowManager: EWMH support detected";
  }

  if (!m_eventListener) {
    m_eventListener = std::make_unique<X11EventListener>();

    connect(m_eventListener.get(), &X11EventListener::windowListChanged, this,
            [this]() {
            qDebug() << "X11WindowManager: Window list changed, emitting windowsChanged";
            emit windowsChanged(); });
    connect(m_eventListener.get(), &X11EventListener::activeWindowChanged, this,
            [this]() {
            qDebug() << "X11WindowManager: Active window changed, emitting windowsChanged";
            emit windowsChanged(); });
    connect(m_eventListener.get(), &X11EventListener::windowTitleChanged, this,
            [this](xcb_window_t window) {
            qDebug() << "X11WindowManager: Window title changed for window" << window << ", emitting windowsChanged";
            emit windowsChanged(); });

    if (!m_eventListener->start()) {
      qWarning() << "X11WindowManager: Failed to start X11 event listener";
      m_eventListener.reset();
    } else {
      qDebug() << "X11WindowManager: X11 event listener started";
    }
  }
}

xcb_connection_t *X11WindowManager::getConnection() const {
  if (m_connection) { return m_connection; }

  // Get native X11 connection from Qt
  auto *nativeInterface = qApp->nativeInterface<QNativeInterface::QX11Application>();
  if (!nativeInterface) {
    qWarning() << "X11WindowManager: Failed to get Qt X11 native interface";
    return nullptr;
  }

  m_connection = nativeInterface->connection();
  if (!m_connection) {
    qWarning() << "X11WindowManager: Failed to get XCB connection from Qt";
    return nullptr;
  }

  return m_connection;
}

xcb_screen_t *X11WindowManager::getScreen() const {
  if (m_screen) { return m_screen; }

  auto *conn = getConnection();
  if (!conn) { return nullptr; }

  // Get the first screen
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(conn));
  m_screen = iter.data;

  return m_screen;
}

xcb_window_t X11WindowManager::getRootWindow() const {
  auto *screen = getScreen();
  return screen ? screen->root : XCB_WINDOW_NONE;
}

xcb_atom_t X11WindowManager::internAtom(const char *name, bool only_if_exists) const {
  QString atomName(name);

  // Check cache first
  if (m_atomCache.contains(atomName)) { return m_atomCache[atomName]; }

  auto *conn = getConnection();
  if (!conn) { return XCB_ATOM_NONE; }

  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, only_if_exists ? 1 : 0, strlen(name), name);
  xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);

  if (!reply) { return XCB_ATOM_NONE; }

  xcb_atom_t atom = reply->atom;
  free(reply);

  // Cache the atom
  m_atomCache[atomName] = atom;

  return atom;
}

bool X11WindowManager::checkEWMHSupport() const {
  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return false; }

  // Check for _NET_SUPPORTING_WM_CHECK
  xcb_atom_t net_supporting_wm_check = internAtom("_NET_SUPPORTING_WM_CHECK");
  if (net_supporting_wm_check == XCB_ATOM_NONE) { return false; }

  xcb_get_property_cookie_t cookie =
      xcb_get_property(conn, 0, root, net_supporting_wm_check, XCB_ATOM_WINDOW, 0, 1);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply || xcb_get_property_value_length(reply) < 4) {
    if (reply) free(reply);
    return false;
  }

  free(reply);
  return true;
}

std::vector<xcb_window_t> X11WindowManager::getClientList() const {
  std::vector<xcb_window_t> windows;

  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return windows; }

  xcb_atom_t net_client_list = internAtom("_NET_CLIENT_LIST");
  if (net_client_list == XCB_ATOM_NONE) { return windows; }

  xcb_get_property_cookie_t cookie = xcb_get_property(conn, 0, root, net_client_list, XCB_ATOM_WINDOW, 0, 1024);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply) { return windows; }

  xcb_window_t *window_list = static_cast<xcb_window_t *>(xcb_get_property_value(reply));
  int count = xcb_get_property_value_length(reply) / sizeof(xcb_window_t);

  windows.reserve(count);
  for (int i = 0; i < count; i++) {
    windows.push_back(window_list[i]);
  }

  free(reply);
  return windows;
}

xcb_window_t X11WindowManager::getActiveWindow() const {
  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return XCB_WINDOW_NONE; }

  xcb_atom_t net_active_window = internAtom("_NET_ACTIVE_WINDOW");
  if (net_active_window == XCB_ATOM_NONE) { return XCB_WINDOW_NONE; }

  xcb_get_property_cookie_t cookie = xcb_get_property(conn, 0, root, net_active_window, XCB_ATOM_WINDOW, 0, 1);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply || xcb_get_property_value_length(reply) < 4) {
    if (reply) free(reply);
    return XCB_WINDOW_NONE;
  }

  xcb_window_t active = *static_cast<xcb_window_t *>(xcb_get_property_value(reply));
  free(reply);
  return active;
}

AbstractWindowManager::WindowList X11WindowManager::listWindowsSync() const {
  WindowList windows;

  auto *conn = getConnection();
  if (!conn) { return windows; }

  auto client_list = getClientList();

  windows.reserve(client_list.size());
  for (xcb_window_t window : client_list) {
    try {
      auto x11_window = std::make_shared<X11Window>(conn, window);
      windows.push_back(x11_window);
    } catch (const std::exception &e) {
      qWarning() << "X11WindowManager: Failed to create X11Window for" << window << ":" << e.what();
    }
  }

  return windows;
}

std::shared_ptr<AbstractWindowManager::AbstractWindow> X11WindowManager::getFocusedWindowSync() const {
  auto *conn = getConnection();
  if (!conn) { return nullptr; }

  xcb_window_t active = getActiveWindow();
  if (active == XCB_WINDOW_NONE) { return nullptr; }

  try {
    return std::make_shared<X11Window>(conn, active);
  } catch (const std::exception &e) {
    qWarning() << "X11WindowManager: Failed to create X11Window for active window:" << e.what();
    return nullptr;
  }
}

void X11WindowManager::focusWindowSync(const AbstractWindow &window) const {
  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return; }

  const X11Window &x11_window = static_cast<const X11Window &>(window);
  xcb_window_t window_id = x11_window.windowId();

  // Method 1: Check if window is on a different desktop and switch to it first
  xcb_atom_t net_wm_desktop = internAtom("_NET_WM_DESKTOP");
  xcb_atom_t net_current_desktop = internAtom("_NET_CURRENT_DESKTOP");

  if (net_wm_desktop != XCB_ATOM_NONE && net_current_desktop != XCB_ATOM_NONE) {
    // Get window's desktop
    xcb_get_property_cookie_t win_desk_cookie =
        xcb_get_property(conn, 0, window_id, net_wm_desktop, XCB_ATOM_CARDINAL, 0, 1);
    xcb_get_property_reply_t *win_desk_reply = xcb_get_property_reply(conn, win_desk_cookie, nullptr);

    if (win_desk_reply && xcb_get_property_value_length(win_desk_reply) >= 4) {
      uint32_t window_desktop = *static_cast<uint32_t *>(xcb_get_property_value(win_desk_reply));

      // Get current desktop
      xcb_get_property_cookie_t cur_desk_cookie =
          xcb_get_property(conn, 0, root, net_current_desktop, XCB_ATOM_CARDINAL, 0, 1);
      xcb_get_property_reply_t *cur_desk_reply = xcb_get_property_reply(conn, cur_desk_cookie, nullptr);

      if (cur_desk_reply && xcb_get_property_value_length(cur_desk_reply) >= 4) {
        uint32_t current_desktop = *static_cast<uint32_t *>(xcb_get_property_value(cur_desk_reply));

        // If window is on a different desktop, switch to it
        if (window_desktop != current_desktop && window_desktop != 0xFFFFFFFF) {
          xcb_client_message_event_t switch_event;
          memset(&switch_event, 0, sizeof(switch_event));
          switch_event.response_type = XCB_CLIENT_MESSAGE;
          switch_event.window = root;
          switch_event.format = 32;
          switch_event.type = net_current_desktop;
          switch_event.data.data32[0] = window_desktop;
          switch_event.data.data32[1] = XCB_CURRENT_TIME;

          xcb_send_event(conn, 0, root,
                        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                        reinterpret_cast<const char *>(&switch_event));
        }

        free(cur_desk_reply);
      }

      free(win_desk_reply);
    }
  }

  // Method 2: Map the window (make it visible)
  xcb_map_window(conn, window_id);

  // Method 3: Raise window to top
  const uint32_t values[] = {XCB_STACK_MODE_ABOVE};
  xcb_configure_window(conn, window_id, XCB_CONFIG_WINDOW_STACK_MODE, values);

  // Method 4: Send _NET_ACTIVE_WINDOW (EWMH way)
  xcb_atom_t net_active_window = internAtom("_NET_ACTIVE_WINDOW");
  if (net_active_window != XCB_ATOM_NONE) {
    xcb_client_message_event_t event;
    memset(&event, 0, sizeof(event));

    event.response_type = XCB_CLIENT_MESSAGE;
    event.window = window_id;
    event.format = 32;
    event.type = net_active_window;
    event.data.data32[0] = 2;                // source indication: 2 = pager
    event.data.data32[1] = XCB_CURRENT_TIME; // timestamp
    event.data.data32[2] = 0;                // requestor's currently active window

    xcb_send_event(conn, 0, root, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                   reinterpret_cast<const char *>(&event));
  }

  // Method 5: Direct input focus (fallback for non-EWMH WMs)
  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_PARENT, window_id, XCB_CURRENT_TIME);

  xcb_flush(conn);
}

bool X11WindowManager::closeWindow(const AbstractWindow &window) const {
  auto *conn = getConnection();
  if (!conn) { return false; }

  const X11Window &x11_window = static_cast<const X11Window &>(window);
  xcb_window_t window_id = x11_window.windowId();

  if (!x11_window.canClose()) {
    qWarning() << "X11WindowManager: Window" << window_id << "does not support WM_DELETE_WINDOW";
    return false;
  }

  xcb_atom_t wm_protocols = internAtom("WM_PROTOCOLS");
  xcb_atom_t wm_delete_window = internAtom("WM_DELETE_WINDOW");

  if (wm_protocols == XCB_ATOM_NONE || wm_delete_window == XCB_ATOM_NONE) { return false; }

  // Send WM_DELETE_WINDOW client message
  xcb_client_message_event_t event;
  memset(&event, 0, sizeof(event));

  event.response_type = XCB_CLIENT_MESSAGE;
  event.window = window_id;
  event.format = 32;
  event.type = wm_protocols;
  event.data.data32[0] = wm_delete_window;
  event.data.data32[1] = XCB_CURRENT_TIME;

  xcb_send_event(conn, 0, window_id, XCB_EVENT_MASK_NO_EVENT, reinterpret_cast<const char *>(&event));
  xcb_flush(conn);

  return true;
}

bool X11WindowManager::hasWorkspaces() const { return m_ewmhSupported; }

std::optional<uint32_t> X11WindowManager::getNumberOfDesktops() const {
  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return std::nullopt; }

  xcb_atom_t net_number_of_desktops = internAtom("_NET_NUMBER_OF_DESKTOPS");
  if (net_number_of_desktops == XCB_ATOM_NONE) { return std::nullopt; }

  xcb_get_property_cookie_t cookie =
      xcb_get_property(conn, 0, root, net_number_of_desktops, XCB_ATOM_CARDINAL, 0, 1);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply || xcb_get_property_value_length(reply) < 4) {
    if (reply) free(reply);
    return std::nullopt;
  }

  uint32_t count = *static_cast<uint32_t *>(xcb_get_property_value(reply));
  free(reply);
  return count;
}

std::optional<uint32_t> X11WindowManager::getCurrentDesktop() const {
  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return std::nullopt; }

  xcb_atom_t net_current_desktop = internAtom("_NET_CURRENT_DESKTOP");
  if (net_current_desktop == XCB_ATOM_NONE) { return std::nullopt; }

  xcb_get_property_cookie_t cookie =
      xcb_get_property(conn, 0, root, net_current_desktop, XCB_ATOM_CARDINAL, 0, 1);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply || xcb_get_property_value_length(reply) < 4) {
    if (reply) free(reply);
    return std::nullopt;
  }

  uint32_t desktop = *static_cast<uint32_t *>(xcb_get_property_value(reply));
  free(reply);
  return desktop;
}

QStringList X11WindowManager::getDesktopNames() const {
  QStringList names;

  auto *conn = getConnection();
  auto root = getRootWindow();

  if (!conn || root == XCB_WINDOW_NONE) { return names; }

  xcb_atom_t net_desktop_names = internAtom("_NET_DESKTOP_NAMES");
  xcb_atom_t utf8_string = internAtom("UTF8_STRING");

  if (net_desktop_names == XCB_ATOM_NONE || utf8_string == XCB_ATOM_NONE) { return names; }

  xcb_get_property_cookie_t cookie = xcb_get_property(conn, 0, root, net_desktop_names, utf8_string, 0, 1024);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply) { return names; }

  // Desktop names are null-separated UTF-8 strings
  const char *value = static_cast<const char *>(xcb_get_property_value(reply));
  int length = xcb_get_property_value_length(reply);

  QString allNames = QString::fromUtf8(value, length);
  names = allNames.split('\0', Qt::SkipEmptyParts);

  free(reply);
  return names;
}

// Simple workspace implementation for X11
class X11Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  X11Workspace(uint32_t index, const QString &name) : m_index(index), m_name(name) {}

  QString id() const override { return QString::number(m_index); }
  QString name() const override { return m_name.isEmpty() ? QString("Desktop %1").arg(m_index + 1) : m_name; }
  QString monitor() const override { return QString(); } // X11 doesn't have per-monitor workspace concept
  bool hasFullScreen() const override { return false; }

private:
  uint32_t m_index;
  QString m_name;
};

AbstractWindowManager::WorkspaceList X11WindowManager::listWorkspaces() const {
  WorkspaceList workspaces;

  if (!m_ewmhSupported) { return workspaces; }

  auto desktop_count = getNumberOfDesktops();
  if (!desktop_count.has_value() || *desktop_count == 0) { return workspaces; }

  auto desktop_names = getDesktopNames();

  workspaces.reserve(*desktop_count);
  for (uint32_t i = 0; i < *desktop_count; i++) {
    QString name = (i < static_cast<uint32_t>(desktop_names.size())) ? desktop_names[i] : QString();
    workspaces.push_back(std::make_shared<X11Workspace>(i, name));
  }

  return workspaces;
}

std::shared_ptr<AbstractWindowManager::AbstractWorkspace> X11WindowManager::getActiveWorkspace() const {
  if (!m_ewmhSupported) { return nullptr; }

  auto current = getCurrentDesktop();
  if (!current.has_value()) { return nullptr; }

  auto desktop_names = getDesktopNames();
  QString name =
      (*current < static_cast<uint32_t>(desktop_names.size())) ? desktop_names[*current] : QString();

  return std::make_shared<X11Workspace>(*current, name);
}

bool X11WindowManager::ping() const {
  auto *conn = getConnection();
  if (!conn) { return false; }

  // Check if connection is still valid
  if (xcb_connection_has_error(conn)) { return false; }

  return true;
}
