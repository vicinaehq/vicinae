#include "wayland-layout-resolver.hpp"
#include <QGuiApplication>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

namespace Keyboard {

namespace {

void onKeymap(void *data, wl_keyboard *, uint32_t format, int32_t fd, uint32_t size) {
  static_cast<WaylandLayoutResolver *>(data)->handleKeymap(format, fd, size);
}

void onEnter(void *, wl_keyboard *, uint32_t, wl_surface *, wl_array *) {}
void onLeave(void *, wl_keyboard *, uint32_t, wl_surface *) {}
void onKey(void *, wl_keyboard *, uint32_t, uint32_t, uint32_t, uint32_t) {}

void onModifiers(void *data, wl_keyboard *, uint32_t, uint32_t depressed, uint32_t latched, uint32_t locked,
                 uint32_t group) {
  static_cast<WaylandLayoutResolver *>(data)->handleModifiers(depressed, latched, locked, group);
}

void onRepeatInfo(void *, wl_keyboard *, int32_t, int32_t) {}

const wl_keyboard_listener LISTENER{
    .keymap = onKeymap,
    .enter = onEnter,
    .leave = onLeave,
    .key = onKey,
    .modifiers = onModifiers,
    .repeat_info = onRepeatInfo,
};

} // namespace

WaylandLayoutResolver::WaylandLayoutResolver() {
  auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (!app || !app->seat()) return;

  m_keyboard = wl_seat_get_keyboard(app->seat());
  wl_keyboard_add_listener(m_keyboard, &LISTENER, this);
}

WaylandLayoutResolver::~WaylandLayoutResolver() {
  if (!m_keyboard) return;
  if (wl_keyboard_get_version(m_keyboard) >= WL_KEYBOARD_RELEASE_SINCE_VERSION) {
    wl_keyboard_release(m_keyboard);
  } else {
    wl_keyboard_destroy(m_keyboard);
  }
}

void WaylandLayoutResolver::handleKeymap(uint32_t format, int fd, uint32_t size) {
  if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
    close(fd);
    return;
  }

  void *map = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (map == MAP_FAILED) return;

  auto *keymap = xkb_keymap_new_from_string(m_context, static_cast<const char *>(map),
                                            XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
  munmap(map, size);
  if (!keymap) return;

  setKeymap(keymap, xkb_state_new(keymap));
}

void WaylandLayoutResolver::handleModifiers(uint32_t depressed, uint32_t latched, uint32_t locked,
                                            uint32_t group) {
  if (m_state) xkb_state_update_mask(m_state, depressed, latched, locked, 0, 0, group);
}

} // namespace Keyboard
