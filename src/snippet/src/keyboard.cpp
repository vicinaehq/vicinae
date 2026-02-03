#include <cstring>
#include <format>
#include <unistd.h>
#include "snippet/keyboard.hpp"
#include "linux/uinput.h"

static constexpr const uinput_setup KB_ID = {
    .id = {.bustype = BUS_VIRTUAL, .vendor = 0x1234, .product = 0x5678, .version = 1},
    .name = "vicinae-snippet-virtual-keyboard",
};
static constexpr const auto KEY_DELAY_US = 2000;

UInputKeyboard::UInputKeyboard() {
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    m_error = std::format("Failed to open /dev/uinput: {}", strerror(errno));
    return;
  }

  ioctl(fd, UI_SET_EVBIT, EV_KEY);

  // KEY_ESC=1 through most keys
  for (int i = KEY_ESC; i < 256; i++) {
    ioctl(fd, UI_SET_KEYBIT, i);
  }

  ioctl(fd, UI_DEV_SETUP, &KB_ID);
  ioctl(fd, UI_DEV_CREATE);
  m_fd = fd;
}

UInputKeyboard::~UInputKeyboard() {
  ioctl(m_fd, UI_DEV_DESTROY);
  close(m_fd);
}

void UInputKeyboard::sendKey(int code, int mods) {
  applyMods(mods);
  usleep(KEY_DELAY_US);
  sendKey(code);
  usleep(KEY_DELAY_US);
  sync();
  usleep(KEY_DELAY_US);
  clearMods(mods);
  sync();
}

void UInputKeyboard::repeatKey(int code, int n) {
  for (int i = 0; i != n; ++i) {
    sendKey(code);
    usleep(KEY_DELAY_US);
  }
}

void UInputKeyboard::sync() {
  struct input_event ev{};
  ev.type = EV_SYN;
  ev.code = SYN_REPORT;
  ev.value = 0;
  write(m_fd, &ev, sizeof(ev));
}

void UInputKeyboard::keyup(int code) {
  struct input_event ev{};
  ev.type = EV_KEY;
  ev.code = code;
  ev.value = 0;
  write(m_fd, &ev, sizeof(ev));
}

void UInputKeyboard::keydown(int code) {
  struct input_event ev{};
  ev.type = EV_KEY;
  ev.code = code;
  ev.value = 1;
  write(m_fd, &ev, sizeof(ev));
}

void UInputKeyboard::sendKey(int code) {
  keydown(code);
  sync();
  keyup(code);
  sync();
}

void UInputKeyboard::applyMods(int mods) {
  if (mods & MOD_CTRL) { keydown(KEY_LEFTCTRL); }
  if (mods & MOD_SHIFT) { keydown(KEY_LEFTSHIFT); }
}

void UInputKeyboard::clearMods(int mods) {
  if (mods & MOD_CTRL) { keyup(KEY_LEFTCTRL); }
  if (mods & MOD_SHIFT) { keyup(KEY_LEFTSHIFT); }
}
