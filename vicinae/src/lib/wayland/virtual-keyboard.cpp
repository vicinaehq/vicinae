#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <qlogging.h>
#include <qnamespace.h>
#include <strings.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <sys/mman.h>
#include <QGuiApplication>
#include "virtual-keyboard.hpp"

namespace chrono = std::chrono;
using namespace std::chrono_literals;

namespace Wayland {
static constexpr const auto delayMs = chrono::duration_cast<chrono::microseconds>(2ms).count();

VirtualKeyboard::VirtualKeyboard() {
  auto *waylandApp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (!waylandApp) { return; }

  m_display = waylandApp->display();
  if (!m_display) { return; }

  wl_seat *seat = waylandApp->seat();
  if (!seat) { return; }

  wl_keyboard *kb = waylandApp->keyboard();
  if (!kb) { return; }

  wl_registry *registry = wl_display_get_registry(m_display);
  if (!registry) { return; }

  wl_registry_add_listener(registry, &_listener, this);
  wl_display_dispatch(m_display);
  wl_display_roundtrip(m_display);

  if (!m_iface) { return; }

  m_keyboard = zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(m_iface, seat);

  if (!m_keyboard) return;
}

VirtualKeyboard::~VirtualKeyboard() {
  if (!m_map.empty()) { releaseMods(); }
}

bool VirtualKeyboard::isAvailable() const { return m_iface && m_keyboard; }

bool VirtualKeyboard::sendKeySequence(xkb_keysym_t key, uint32_t mods) {
  if (!isAvailable()) return false;

  uint32_t keyCode = mappedKeyCode(key);

  if (mods) { applyMods(mods); }
  usleep(delayMs);
  sendKey(keyCode);
  usleep(delayMs);
  if (mods) { releaseMods(); }
  usleep(delayMs);

  return true;
}

void VirtualKeyboard::roundtrip() { wl_display_roundtrip(m_display); }

void VirtualKeyboard::applyMods(uint32_t mods) {
  zwp_virtual_keyboard_v1_modifiers(m_keyboard, mods & ~Modifier::MOD_CAPSLOCK, 0,
                                    mods & Modifier::MOD_CAPSLOCK, 0);
  roundtrip();
}

void VirtualKeyboard::sendKey(uint32_t code) {
  zwp_virtual_keyboard_v1_key(m_keyboard, 0, code, WL_KEYBOARD_KEY_STATE_PRESSED);
  roundtrip();
  usleep(delayMs);
  zwp_virtual_keyboard_v1_key(m_keyboard, 0, code, WL_KEYBOARD_KEY_STATE_RELEASED);
  roundtrip();
}

void VirtualKeyboard::releaseMods() {
  zwp_virtual_keyboard_v1_modifiers(m_keyboard, 0, 0, 0, 0);
  roundtrip();
}

std::string VirtualKeyboard::generateKeymap(const std::vector<xkb_keysym_t> &keys) const {
  std::ostringstream keymap;

  keymap << "xkb_keymap {\n";
  keymap << "xkb_keycodes \"(unnamed)\" {\n";
  keymap << "minimum = 8;\n";
  keymap << "maximum = " << (keys.size() + 9) << ";\n";

  for (size_t i = 0; i < keys.size(); i++) {
    keymap << "<K" << (i + 1) << "> = " << (i + 9) << ";\n";
  }
  keymap << "};\n";

  keymap << "xkb_types \"(unnamed)\" { include \"complete\" };\n";
  keymap << "xkb_compatibility \"(unnamed)\" { include \"complete\" };\n";

  keymap << "xkb_symbols \"(unnamed)\" {\n";
  for (size_t i = 0; i < keys.size(); i++) {
    char sym_name[256];
    xkb_keysym_get_name(keys[i], sym_name, sizeof(sym_name));
    keymap << "key <K" << (i + 1) << "> {[" << sym_name << "]};\n";
  }
  keymap << "};\n";
  keymap << "};\n";
  keymap << '\0';

  return keymap.str();
}

bool VirtualKeyboard::uploadKeymap(const std::vector<xkb_keysym_t> &keysyms) {
  int fd = memfd_create("vicinae-virtual-keymap", MFD_CLOEXEC);

  if (fd < 0) {
    qWarning() << "VirtualKeyboard::uploadKeymap: memfd_create failed" << strerror(errno);
    return -1;
  }

  std::string keymap = generateKeymap(keysyms);

  if (write(fd, keymap.data(), keymap.size()) < 0) {
    qWarning() << "VirtualKeyboard::uploadKeymap: failed to write" << strerror(errno);
    close(fd);
    return false;
  }

  lseek(fd, 0, SEEK_SET);
  zwp_virtual_keyboard_v1_keymap(m_keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, fd, keymap.size());
  wl_display_roundtrip(m_display);
  close(fd);
  return true;
}

void VirtualKeyboard::handleGlobal(void *data, struct wl_registry *registry, uint32_t name,
                                   const char *interface, uint32_t version) {
  static const wl_interface &iface = zwp_virtual_keyboard_manager_v1_interface;
  VirtualKeyboard *vkb = static_cast<VirtualKeyboard *>(data);

  if (strcmp(interface, iface.name) == 0 && version == iface.version) {
    vkb->m_iface =
        static_cast<zwp_virtual_keyboard_manager_v1 *>(wl_registry_bind(registry, name, &iface, version));
  }
}

void VirtualKeyboard::globalRemove(void *data, struct wl_registry *registry, uint32_t name) {}

uint32_t VirtualKeyboard::mappedKeyCode(xkb_keysym_t sym) {
  if (auto idx = indexOfKey(sym)) return idx.value() + 1;
  uint32_t code = m_map.size() + 1;
  m_map.emplace_back(sym);
  uploadKeymap(m_map);
  return code;
}

std::optional<size_t> VirtualKeyboard::indexOfKey(xkb_keysym_t sym) const {
  if (auto it = std::ranges::find(m_map, sym); it != m_map.end()) { return std::distance(it, m_map.begin()); }
  return {};
}

bool VirtualKeyboard::isMapped(xkb_keysym_t sym) const {
  // we could speed this up by maintaining a set but I don't think it's worth it
  return std::ranges::contains(m_map, sym);
}

}; // namespace Wayland
