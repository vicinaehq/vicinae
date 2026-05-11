#include <array>
#include <cstring>
#include <format>
#include <print>
#include <unistd.h>
#include "linuxutils/keyboard.hpp"
#include "linux/uinput.h"

namespace linuxutils {

static constexpr const uinput_setup KB_ID = {
    .id = {.bustype = BUS_VIRTUAL, .vendor = 0x1234, .product = 0x5678, .version = 1},
    .name = "vicinae-snippet-virtual-keyboard",
};
static constexpr int MODIFIER_DELAY_US = 10000;
static constexpr const uint32_t EVDEV_OFFSET = 8;

static void emit(int fd, const input_event &ev) {
  if (write(fd, &ev, sizeof(ev)) < 0) {
    std::println(stderr, "UInputKeyboard: write failed: {}", strerror(errno));
  }
}

UInputKeyboard::UInputKeyboard() {
  const int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    m_error = std::format("Failed to open /dev/uinput: {}", strerror(errno));
    return;
  }

  ioctl(fd, UI_SET_EVBIT, EV_KEY);

  for (int i = KEY_ESC; i < 256; i++) {
    ioctl(fd, UI_SET_KEYBIT, i);
  }

  ioctl(fd, UI_DEV_SETUP, &KB_ID);
  ioctl(fd, UI_DEV_CREATE);
  m_fd = fd;

  buildCharMap();
}

UInputKeyboard::~UInputKeyboard() {
  ioctl(m_fd, UI_DEV_DESTROY);
  close(m_fd);
  if (m_xkbKeymap) xkb_keymap_unref(m_xkbKeymap);
  if (m_xkbCtx) xkb_context_unref(m_xkbCtx);
}

void UInputKeyboard::buildCharMap() {
  m_xkbCtx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!m_xkbCtx) return;

  m_xkbKeymap = xkb_keymap_new_from_names(m_xkbCtx, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!m_xkbKeymap) return;

  auto *state = xkb_state_new(m_xkbKeymap);
  if (!state) return;

  std::array<char, 8> buf{};

  for (uint32_t keycode = EVDEV_OFFSET; keycode < 256 + EVDEV_OFFSET; ++keycode) {
    const uint32_t evdevCode = keycode - EVDEV_OFFSET;

    xkb_state_update_key(state, keycode, XKB_KEY_UP);
    int len = xkb_state_key_get_utf8(state, keycode, buf.data(), buf.size());
    if (len == 1) {
      const auto idx = static_cast<unsigned char>(buf[0]);
      if (idx < CHARMAP_SIZE && m_charMap[idx].code == 0) { m_charMap[idx] = {.code = evdevCode, .mods = 0}; }
    }

    xkb_state_update_key(state, KEY_LEFTSHIFT + EVDEV_OFFSET, XKB_KEY_DOWN);
    len = xkb_state_key_get_utf8(state, keycode, buf.data(), buf.size());
    if (len == 1) {
      const auto idx = static_cast<unsigned char>(buf[0]);
      if (idx < CHARMAP_SIZE && m_charMap[idx].code == 0) {
        m_charMap[idx] = {.code = evdevCode, .mods = static_cast<int>(Modifier::Shift)};
      }
    }
    xkb_state_update_key(state, KEY_LEFTSHIFT + EVDEV_OFFSET, XKB_KEY_UP);
  }

  xkb_state_unref(state);
}

void UInputKeyboard::sendKey(int code, int mods) {
  applyMods(mods);
  usleep(mods ? MODIFIER_DELAY_US : m_keyDelayUs);
  sendKey(code);
  usleep(m_keyDelayUs);
  sync();
  usleep(mods ? MODIFIER_DELAY_US : m_keyDelayUs);
  clearMods(mods);
  sync();
}

void UInputKeyboard::repeatKey(int code, int n) {
  for (int i = 0; i != n; ++i) {
    sendKey(code);
    usleep(m_keyDelayUs);
  }
}

void UInputKeyboard::sync() {
  struct input_event ev{};
  ev.type = EV_SYN;
  ev.code = SYN_REPORT;
  ev.value = 0;
  emit(m_fd, ev);
}

void UInputKeyboard::keyup(int code) {
  struct input_event ev{};
  ev.type = EV_KEY;
  ev.code = code;
  ev.value = 0;
  emit(m_fd, ev);
}

void UInputKeyboard::keydown(int code) {
  struct input_event ev{};
  ev.type = EV_KEY;
  ev.code = code;
  ev.value = 1;
  emit(m_fd, ev);
}

void UInputKeyboard::sendKey(int code) {
  keydown(code);
  sync();
  keyup(code);
  sync();
}

void UInputKeyboard::typeText(std::string_view text) {
  for (char c : text) {
    const auto idx = static_cast<unsigned char>(c);
    if (idx >= CHARMAP_SIZE || m_charMap[idx].code == 0) continue;
    sendKey(static_cast<int>(m_charMap[idx].code), m_charMap[idx].mods);
  }
}

void UInputKeyboard::applyMods(int mods) {
  const auto m = static_cast<Modifier>(mods);
  if ((m & Modifier::Ctrl) != Modifier::None) { keydown(KEY_LEFTCTRL); }
  if ((m & Modifier::Shift) != Modifier::None) { keydown(KEY_LEFTSHIFT); }
}

void UInputKeyboard::clearMods(int mods) {
  const auto m = static_cast<Modifier>(mods);
  if ((m & Modifier::Ctrl) != Modifier::None) { keyup(KEY_LEFTCTRL); }
  if ((m & Modifier::Shift) != Modifier::None) { keyup(KEY_LEFTSHIFT); }
}
}; // namespace linuxutils
