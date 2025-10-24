#pragma once
#include <xkbcommon/xkbcommon.h>
#include "virtual-keyboard-unstable-v1-client-protocol.h"

namespace Wayland {
class VirtualKeyboard {
public:
  enum Modifier {
    MOD_NONE = 0,
    MOD_SHIFT = 1,
    MOD_CAPSLOCK = 2,
    MOD_CTRL = 4,
    MOD_ALT = 8,
    MOD_LOGO = 64,
    MOD_ALTGR = 128
  };

  VirtualKeyboard();
  ~VirtualKeyboard();

  /**
   * Whether we were able to create a virtual keyboard in the current wayland environment.
   * The compositor might have refused to let us create one or the compositor might not implement
   * virtual-keyboard-unstable-v1.xml.
   */
  bool isAvailable() const;

  /**
   * Send key with an optional set of mods
   */
  bool sendKeySequence(xkb_keysym_t key, uint32_t mods = Modifier::MOD_NONE);

  bool isMapped(xkb_keysym_t sym) const;

  /**
   * Upload the keymap to the compositor. If the keyboard attempts to send an unregisted key
   * the keymap is reuploaded automatically, but you can set the keymap beforehand to avoid
   * excessive overhead if you send many different keys.
   */
  bool uploadKeymap(const std::vector<xkb_keysym_t> &keysyms);

private:
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  constexpr static const struct wl_registry_listener _listener = {.global = handleGlobal,
                                                                  .global_remove = globalRemove};

  void roundtrip();
  std::string generateKeymap(const std::vector<xkb_keysym_t> &keys) const;
  uint32_t mappedKeyCode(xkb_keysym_t sym);
  std::optional<size_t> indexOfKey(xkb_keysym_t sym) const;
  void applyMods(uint32_t mods);
  void releaseMods();
  void sendKey(uint32_t code);

  std::vector<xkb_keysym_t> m_map;

  // wayland stuff
  wl_display *m_display = nullptr;
  zwp_virtual_keyboard_manager_v1 *m_iface = nullptr;
  zwp_virtual_keyboard_v1 *m_keyboard = nullptr;
};
}; // namespace Wayland
