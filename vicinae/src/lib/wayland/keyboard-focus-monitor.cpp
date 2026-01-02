#include "keyboard-focus-monitor.hpp"
#include <QGuiApplication>
#include <unistd.h>

namespace Wayland {

KeyboardFocusMonitor::KeyboardFocusMonitor(QObject *parent) 
    : QObject(parent) {
  auto *waylandApp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (!waylandApp) return;

  wl_seat *seat = waylandApp->seat();
  if (!seat) return;

  m_keyboard = wl_seat_get_keyboard(seat);
  if (!m_keyboard) return;

  wl_keyboard_add_listener(m_keyboard, &s_keyboardListener, this);
}

KeyboardFocusMonitor::~KeyboardFocusMonitor() {
  if (m_keyboard) { wl_keyboard_destroy(m_keyboard); }
}

bool KeyboardFocusMonitor::isAvailable() const { return m_keyboard != nullptr; }

void KeyboardFocusMonitor::setEnabled(bool enabled) { 
  m_enabled = enabled;
  if (enabled) {
    m_hasFocus = false;
  }
}

void KeyboardFocusMonitor::handleKeyboardEnter(void *data, wl_keyboard *keyboard, uint32_t serial,
                                               wl_surface *surface, wl_array *keys) {
  auto *self = static_cast<KeyboardFocusMonitor *>(data);
  
  if (!self->m_enabled) return;
  
  if (!self->m_hasFocus) {
    self->m_hasFocus = true;
    emit self->focusGained();
  }
}

void KeyboardFocusMonitor::handleKeyboardLeave(void *data, wl_keyboard *keyboard, uint32_t serial,
                                               wl_surface *surface) {
  auto *self = static_cast<KeyboardFocusMonitor *>(data);
  
  if (!self->m_enabled) return;
  
  if (self->m_hasFocus) {
    self->m_hasFocus = false;
    emit self->focusLost();
  }
}

void KeyboardFocusMonitor::handleKeyboardKey(void *data, wl_keyboard *keyboard, uint32_t serial, uint32_t time,
                                             uint32_t key, uint32_t state) {}

void KeyboardFocusMonitor::handleKeyboardModifiers(void *data, wl_keyboard *keyboard, uint32_t serial,
                                                   uint32_t mods_depressed, uint32_t mods_latched,
                                                   uint32_t mods_locked, uint32_t group) {}

void KeyboardFocusMonitor::handleKeyboardKeymap(void *data, wl_keyboard *keyboard, uint32_t format, int fd,
                                                uint32_t size) {
  close(fd);
}

void KeyboardFocusMonitor::handleKeyboardRepeatInfo(void *data, wl_keyboard *keyboard, int32_t rate,
                                                    int32_t delay) {}

} // namespace Wayland
