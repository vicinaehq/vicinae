#include <linux/input-event-codes.h>
#include <QDebug>
#include "linux-keyboard-service.hpp"

LinuxKeyboardService::LinuxKeyboardService() {
  if (const auto err = m_keyboard.error()) {
    qWarning().noquote() << "Keyboard injection disabled:" << err->c_str()
                         << "- ensure your user has write access to /dev/uinput.";
  }
}

bool LinuxKeyboardService::supportsKeyInjection() const { return !m_keyboard.error().has_value(); }

void LinuxKeyboardService::paste(bool terminal) {
  using Mod = linuxutils::UInputKeyboard::Modifier;

  if (terminal) {
    m_keyboard.sendKey(KEY_V, static_cast<int>(Mod::Ctrl | Mod::Shift));
  } else {
    m_keyboard.sendKey(KEY_V, static_cast<int>(Mod::Ctrl));
  }
}

void LinuxKeyboardService::backspace(int n) { m_keyboard.repeatKey(KEY_BACKSPACE, n); }

void LinuxKeyboardService::moveCursorLeft(int n) { m_keyboard.repeatKey(KEY_LEFT, n); }

void LinuxKeyboardService::space() { m_keyboard.sendKey(KEY_SPACE, 0); }
