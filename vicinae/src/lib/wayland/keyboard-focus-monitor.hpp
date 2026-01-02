#pragma once
#include <QObject>
#include <wayland-client.h>

namespace Wayland {

class KeyboardFocusMonitor : public QObject {
  Q_OBJECT

public:
  explicit KeyboardFocusMonitor(QObject *parent = nullptr);
  ~KeyboardFocusMonitor();

  bool isAvailable() const;
  void setEnabled(bool enabled);

signals:
  void focusGained();
  void focusLost();

private:
  static void handleKeyboardEnter(void *data, wl_keyboard *keyboard, uint32_t serial, wl_surface *surface,
                                  wl_array *keys);
  static void handleKeyboardLeave(void *data, wl_keyboard *keyboard, uint32_t serial, wl_surface *surface);
  static void handleKeyboardKey(void *data, wl_keyboard *keyboard, uint32_t serial, uint32_t time,
                                uint32_t key, uint32_t state);
  static void handleKeyboardModifiers(void *data, wl_keyboard *keyboard, uint32_t serial,
                                      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group);
  static void handleKeyboardKeymap(void *data, wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size);
  static void handleKeyboardRepeatInfo(void *data, wl_keyboard *keyboard, int32_t rate, int32_t delay);

  static constexpr const struct wl_keyboard_listener s_keyboardListener = {
      .keymap = handleKeyboardKeymap,
      .enter = handleKeyboardEnter,
      .leave = handleKeyboardLeave,
      .key = handleKeyboardKey,
      .modifiers = handleKeyboardModifiers,
      .repeat_info = handleKeyboardRepeatInfo,
  };

  wl_keyboard *m_keyboard = nullptr;
  bool m_hasFocus = false;
  bool m_enabled = false;
};

} // namespace Wayland
