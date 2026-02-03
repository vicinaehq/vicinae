#pragma once
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <fcntl.h>
#include <linux/uinput.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <unistd.h>

/**
 * Virtual keyboard device using /dev/uinput.
 * This is a very low level keyboard device that operates at the scan code level,
 * The main pro of this is that it will work on every linux environment.
 * We mostly use it to send ctrl+v and ctrl+shift+v in order to implement proper paste
 * functionnality.
 * It's not possible to type UTF-8 text with this alone. The way to do this is to use the clipboard
 * which is better suited for data transfer anyways.
 */
class UInputKeyboard {

public:
  enum Modifier : std::uint8_t {
    MOD_NONE = 0,
    MOD_SHIFT = 1,
    MOD_CAPSLOCK = 1 << 1,
    MOD_CTRL = 1 << 2,
    MOD_ALT = 1 << 3,
    MOD_LOGO = 1 << 4,
    MOD_ALTGR = 1 << 5
  };

public:
  UInputKeyboard();
  ~UInputKeyboard();

  void sendKey(int code, int mods);
  void repeatKey(int code, int n);

  int fd() const { return m_fd; }
  std::optional<std::string> error() const { return m_error; }

private:
  void sync();
  void keyup(int code);
  void keydown(int code);
  void sendKey(int code);
  void applyMods(int mods);
  void clearMods(int mods);

  std::optional<std::string> m_error;
  int m_fd = -1;
};
