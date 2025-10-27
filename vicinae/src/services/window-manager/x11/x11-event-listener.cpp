#include "x11-event-listener.hpp"

#include <QDebug>
#include <QSocketNotifier>
#include <cstring>
#include <qlogging.h>
#include <qnumeric.h>
#include <utility>
#include <xcb/xproto.h>

X11EventListener::X11EventListener(QObject *parent) : QObject(parent) {
  m_debounceTimer.setSingleShot(true);
  m_debounceTimer.setInterval(50);
  connect(&m_debounceTimer, &QTimer::timeout, this, &X11EventListener::emitDebouncedSignals);
}

X11EventListener::~X11EventListener() {
  if (m_notifier) { m_notifier->setEnabled(false); }

  if (m_connection) {
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    m_screen = nullptr;
    m_root = XCB_WINDOW_NONE;
  }
}

bool X11EventListener::start() {
  if (m_connection) { return true; }

  m_connection = xcb_connect(nullptr, nullptr);
  if (int error = xcb_connection_has_error(m_connection)) {
    qWarning() << "X11EventListener: xcb_connect failed with error" << error;
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    return false;
  }

  const xcb_setup_t *setup = xcb_get_setup(m_connection);
  if (!setup) {
    qWarning() << "X11EventListener: failed to fetch X setup";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    return false;
  }

  xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
  if (it.rem == 0) {
    qWarning() << "X11EventListener: no screens available";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    return false;
  }

  m_screen = it.data;
  m_root = m_screen ? m_screen->root : XCB_WINDOW_NONE;
  if (m_root == XCB_WINDOW_NONE) {
    qWarning() << "X11EventListener: failed to determine root window";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    m_screen = nullptr;
    return false;
  }

  m_atomClientList = internAtom("_NET_CLIENT_LIST");
  m_atomActiveWindow = internAtom("_NET_ACTIVE_WINDOW");
  m_atomWmName = internAtom("_NET_WM_NAME");

  subscribeToRootEvents();

  m_clients = fetchClientList();
  m_activeWindow = fetchActiveWindow();

  int fd = xcb_get_file_descriptor(m_connection);
  if (fd < 0) {
    qWarning() << "X11EventListener: invalid XCB file descriptor";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    m_screen = nullptr;
    m_root = XCB_WINDOW_NONE;
    return false;
  }

  m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
  connect(m_notifier, &QSocketNotifier::activated, this, &X11EventListener::drainEvents);

  return true;
}

void X11EventListener::subscribeToRootEvents() {
  if (!m_connection || m_root == XCB_WINDOW_NONE) { return; }

  // Only subscribe to substructure events for window creation/destruction
  // Property change events were commented out as they may cause compatibility issues
  uint32_t mask = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
  xcb_change_window_attributes(m_connection, m_root, XCB_CW_EVENT_MASK, &mask);
  xcb_flush(m_connection);
}

xcb_atom_t X11EventListener::internAtom(const char *name) const {
  if (!m_connection) { return XCB_ATOM_NONE; }

  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(m_connection, 0, std::strlen(name), name);
  xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(m_connection, cookie, nullptr);
  if (!reply) { return XCB_ATOM_NONE; }

  xcb_atom_t atom = reply->atom;
  free(reply);
  return atom;
}

std::unordered_set<xcb_window_t> X11EventListener::fetchClientList() const {
  std::unordered_set<xcb_window_t> result;
  if (!m_connection || m_root == XCB_WINDOW_NONE || m_atomClientList == XCB_ATOM_NONE) { return result; }

  xcb_get_property_cookie_t cookie =
      xcb_get_property(m_connection, 0, m_root, m_atomClientList, XCB_ATOM_WINDOW, 0, 1 << 12);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(m_connection, cookie, nullptr);
  if (!reply) { return result; }

  int length = xcb_get_property_value_length(reply);
  auto *windows = static_cast<xcb_window_t const *>(xcb_get_property_value(reply));
  int count = length / static_cast<int>(sizeof(xcb_window_t));
  result.reserve(count);
  for (int i = 0; i < count; ++i) {
    result.insert(windows[i]);
  }

  free(reply);
  return result;
}

xcb_window_t X11EventListener::fetchActiveWindow() const {
  if (!m_connection || m_root == XCB_WINDOW_NONE || m_atomActiveWindow == XCB_ATOM_NONE) {
    return XCB_WINDOW_NONE;
  }

  xcb_get_property_cookie_t cookie =
      xcb_get_property(m_connection, 0, m_root, m_atomActiveWindow, XCB_ATOM_WINDOW, 0, 1);
  xcb_get_property_reply_t *reply = xcb_get_property_reply(m_connection, cookie, nullptr);
  if (!reply || xcb_get_property_value_length(reply) < static_cast<int>(sizeof(xcb_window_t))) {
    if (reply) { free(reply); }
    return XCB_WINDOW_NONE;
  }

  xcb_window_t window = *static_cast<const xcb_window_t *>(xcb_get_property_value(reply));
  free(reply);
  return window;
}

void X11EventListener::drainEvents() {
  if (!m_connection) { return; }

  if (m_notifier) { m_notifier->setEnabled(false); }

  while (auto *event = xcb_poll_for_event(m_connection)) {
    uint8_t type = event->response_type & ~0x80;
    switch (type) {
    case XCB_PROPERTY_NOTIFY: {
      auto *property = reinterpret_cast<xcb_property_notify_event_t *>(event);
      if (property->atom == m_atomClientList) {
        auto now = fetchClientList();
        if (now != m_clients) {
          m_clients = std::move(now);
          m_pendingWindowListChanged = true;
        }
      } else if (property->atom == m_atomActiveWindow) {
        xcb_window_t active = fetchActiveWindow();
        if (active != m_activeWindow) {
          m_activeWindow = active;
          m_pendingActiveWindowChanged = true;
        }
      } else if (property->atom == m_atomWmName) {
        if (m_clients.empty() || m_clients.contains(property->window)) {
          m_pendingTitlesChanged.insert(property->window);
        }
      }
      break;
    }
    case XCB_CREATE_NOTIFY:
      m_pendingWindowListChanged = true;
      break;
    case XCB_DESTROY_NOTIFY:
      m_pendingWindowListChanged = true;
      break;
    case XCB_REPARENT_NOTIFY:
      m_pendingWindowListChanged = true;
      break;
    default:
      break;
    }

    free(event);
  }

  // Restart debounce timer if any signal is pending
  if (m_pendingWindowListChanged || m_pendingActiveWindowChanged || !m_pendingTitlesChanged.empty()) {
    m_debounceTimer.start();
  }

  if (m_notifier) { m_notifier->setEnabled(true); }
}

void X11EventListener::emitDebouncedSignals() {
  // Emit all collected signals
  if (m_pendingWindowListChanged) {
    emit X11EventListener::windowListChanged();
    m_pendingWindowListChanged = false;
  }

  if (m_pendingActiveWindowChanged) {
    emit X11EventListener::activeWindowChanged();
    m_pendingActiveWindowChanged = false;
  }

  for (auto window : m_pendingTitlesChanged) {
    emit windowTitleChanged(window);
  }
  m_pendingTitlesChanged.clear();
}
