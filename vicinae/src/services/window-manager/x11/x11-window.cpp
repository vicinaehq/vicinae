#include "x11-window.hpp"
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <cstring>
#include <qlogging.h>

// Helper function to intern an atom
static xcb_atom_t intern_atom(xcb_connection_t *conn, const char *name, bool only_if_exists = false) {
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, only_if_exists ? 1 : 0, strlen(name), name);
  xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);

  if (!reply) { return XCB_ATOM_NONE; }

  xcb_atom_t atom = reply->atom;
  free(reply);
  return atom;
}

// Helper function to get a string property
static QString get_string_property(xcb_connection_t *conn, xcb_window_t window, xcb_atom_t property,
                                    xcb_atom_t type = XCB_ATOM_STRING) {
  xcb_get_property_cookie_t cookie =
      xcb_get_property(conn, 0, window, property, type, 0, 1024); // Max 1024 bytes
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply || xcb_get_property_value_length(reply) == 0) {
    if (reply) free(reply);
    return QString();
  }

  const char *value = static_cast<const char *>(xcb_get_property_value(reply));
  int length = xcb_get_property_value_length(reply);

  QString result = QString::fromUtf8(value, length);
  free(reply);
  return result;
}

// Helper function to get a cardinal (uint32_t) property
static std::optional<uint32_t> get_cardinal_property(xcb_connection_t *conn, xcb_window_t window,
                                                      xcb_atom_t property) {
  xcb_get_property_cookie_t cookie = xcb_get_property(conn, 0, window, property, XCB_ATOM_CARDINAL, 0, 1);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply || xcb_get_property_value_length(reply) < 4) {
    if (reply) free(reply);
    return std::nullopt;
  }

  uint32_t value = *static_cast<uint32_t *>(xcb_get_property_value(reply));
  free(reply);
  return value;
}

// Helper function to check if atom is in list
static bool has_atom_in_list(xcb_connection_t *conn, xcb_window_t window, xcb_atom_t property,
                              xcb_atom_t target_atom) {
  xcb_get_property_cookie_t cookie = xcb_get_property(conn, 0, window, property, XCB_ATOM_ATOM, 0, 64);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, nullptr);

  if (!reply) { return false; }

  xcb_atom_t *atoms = static_cast<xcb_atom_t *>(xcb_get_property_value(reply));
  int count = xcb_get_property_value_length(reply) / sizeof(xcb_atom_t);

  bool found = false;
  for (int i = 0; i < count; i++) {
    if (atoms[i] == target_atom) {
      found = true;
      break;
    }
  }

  free(reply);
  return found;
}

X11Window::X11Window(xcb_connection_t *connection, xcb_window_t window) : m_window(window) {
  m_id = QString::number(window);
  queryProperties(connection);
}

void X11Window::queryProperties(xcb_connection_t *connection) {
  m_title = queryTitle(connection);
  m_wmClass = queryWmClass(connection);
  m_pid = queryPid(connection);
  m_workspace = queryWorkspace(connection);
  m_bounds = queryBounds(connection);
  m_canClose = queryCanClose(connection);
}

QString X11Window::queryTitle(xcb_connection_t *connection) {
  // Try _NET_WM_NAME (UTF-8) first
  xcb_atom_t net_wm_name = intern_atom(connection, "_NET_WM_NAME");
  xcb_atom_t utf8_string = intern_atom(connection, "UTF8_STRING");

  if (net_wm_name != XCB_ATOM_NONE && utf8_string != XCB_ATOM_NONE) {
    QString title = get_string_property(connection, m_window, net_wm_name, utf8_string);
    if (!title.isEmpty()) { return title; }
  }

  // Fallback to WM_NAME (legacy)
  QString title = get_string_property(connection, m_window, XCB_ATOM_WM_NAME);
  return title.isEmpty() ? QString("(No Title)") : title;
}

QString X11Window::queryWmClass(xcb_connection_t *connection) {
  // WM_CLASS contains two null-terminated strings: instance and class
  // We want the class name (second string)
  QString wmClass = get_string_property(connection, m_window, XCB_ATOM_WM_CLASS);

  if (wmClass.isEmpty()) { return QString("(Unknown)"); }

  // WM_CLASS format is: "instance\0class\0"
  // Find the second string (class name)
  int nullPos = wmClass.indexOf('\0');
  if (nullPos >= 0 && nullPos + 1 < wmClass.length()) {
    QString className = wmClass.mid(nullPos + 1);
    // Remove trailing null if present
    if (className.endsWith('\0')) { className.chop(1); }
    return className.isEmpty() ? QString("(Unknown)") : className;
  }

  return wmClass;
}

std::optional<int> X11Window::queryPid(xcb_connection_t *connection) {
  xcb_atom_t net_wm_pid = intern_atom(connection, "_NET_WM_PID");
  if (net_wm_pid == XCB_ATOM_NONE) { return std::nullopt; }

  auto pid = get_cardinal_property(connection, m_window, net_wm_pid);
  if (!pid.has_value()) { return std::nullopt; }

  return static_cast<int>(*pid);
}

std::optional<QString> X11Window::queryWorkspace(xcb_connection_t *connection) {
  xcb_atom_t net_wm_desktop = intern_atom(connection, "_NET_WM_DESKTOP");
  if (net_wm_desktop == XCB_ATOM_NONE) { return std::nullopt; }

  auto desktop = get_cardinal_property(connection, m_window, net_wm_desktop);
  if (!desktop.has_value()) { return std::nullopt; }

  // 0xFFFFFFFF means "all desktops"
  if (*desktop == 0xFFFFFFFF) { return QString("all"); }

  return QString::number(*desktop);
}

std::optional<AbstractWindowManager::WindowBounds> X11Window::queryBounds(xcb_connection_t *connection) {
  xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection, m_window);
  xcb_get_geometry_reply_t *reply = xcb_get_geometry_reply(connection, cookie, nullptr);

  if (!reply) { return std::nullopt; }

  AbstractWindowManager::WindowBounds bounds;
  bounds.x = reply->x;
  bounds.y = reply->y;
  bounds.width = reply->width;
  bounds.height = reply->height;

  free(reply);
  return bounds;
}

bool X11Window::queryCanClose(xcb_connection_t *connection) {
  // Check if window supports WM_DELETE_WINDOW protocol
  xcb_atom_t wm_protocols = intern_atom(connection, "WM_PROTOCOLS");
  xcb_atom_t wm_delete_window = intern_atom(connection, "WM_DELETE_WINDOW");

  if (wm_protocols == XCB_ATOM_NONE || wm_delete_window == XCB_ATOM_NONE) { return false; }

  return has_atom_in_list(connection, m_window, wm_protocols, wm_delete_window);
}
