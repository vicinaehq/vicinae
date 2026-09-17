#include "x11-layout-resolver.hpp"
#include <QDebug>
#include <xcb/xcb.h>
// xcb/xkb.h uses `explicit` as a field name
#define explicit explicit_
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>

namespace Keyboard {

X11LayoutResolver::X11LayoutResolver() {
  m_connection = xcb_connect(nullptr, nullptr);
  if (xcb_connection_has_error(m_connection)) {
    qWarning() << "X11LayoutResolver: xcb_connect failed";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    return;
  }

  if (!xkb_x11_setup_xkb_extension(m_connection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
                                   XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, &m_firstEvent,
                                   nullptr)) {
    qWarning() << "X11LayoutResolver: XKB extension unavailable";
    return;
  }

  m_device = xkb_x11_get_core_keyboard_device_id(m_connection);
  if (m_device == -1) return;

  const uint16_t events = XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY |
                          XCB_XKB_EVENT_TYPE_STATE_NOTIFY;
  const uint16_t mapParts = XCB_XKB_MAP_PART_KEY_TYPES | XCB_XKB_MAP_PART_KEY_SYMS |
                            XCB_XKB_MAP_PART_MODIFIER_MAP | XCB_XKB_MAP_PART_EXPLICIT_COMPONENTS |
                            XCB_XKB_MAP_PART_KEY_ACTIONS | XCB_XKB_MAP_PART_VIRTUAL_MODS |
                            XCB_XKB_MAP_PART_VIRTUAL_MOD_MAP;
  xcb_xkb_select_events(m_connection, static_cast<xcb_xkb_device_spec_t>(m_device), events, 0, events,
                        mapParts, mapParts, nullptr);
  xcb_flush(m_connection);

  reload();

  m_notifier = new QSocketNotifier(xcb_get_file_descriptor(m_connection), QSocketNotifier::Read, this);
  connect(m_notifier, &QSocketNotifier::activated, this, &X11LayoutResolver::drainEvents);
}

X11LayoutResolver::~X11LayoutResolver() {
  if (m_connection) xcb_disconnect(m_connection);
}

void X11LayoutResolver::reload() {
  auto *keymap =
      xkb_x11_keymap_new_from_device(m_context, m_connection, m_device, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!keymap) return;
  setKeymap(keymap, xkb_x11_state_new_from_device(keymap, m_connection, m_device));
}

void X11LayoutResolver::drainEvents() {
  for (auto *event = xcb_poll_for_event(m_connection); event; event = xcb_poll_for_event(m_connection)) {
    if ((event->response_type & ~0x80) == m_firstEvent) {
      const auto *notify = reinterpret_cast<xcb_xkb_state_notify_event_t *>(event);
      switch (notify->xkbType) {
      case XCB_XKB_NEW_KEYBOARD_NOTIFY:
      case XCB_XKB_MAP_NOTIFY:
        reload();
        break;
      case XCB_XKB_STATE_NOTIFY:
        if (m_state) {
          xkb_state_update_mask(m_state, notify->baseMods, notify->latchedMods, notify->lockedMods,
                                notify->baseGroup, notify->latchedGroup, notify->lockedGroup);
        }
        break;
      default:
        break;
      }
    }
    free(event);
  }
}

} // namespace Keyboard
