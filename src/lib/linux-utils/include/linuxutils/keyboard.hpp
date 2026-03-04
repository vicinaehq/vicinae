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
  enum class Modifier : std::uint8_t {
    None = 0,
    Shift = 1,
    Capslock = 1 << 1,
    Ctrl = 1 << 2,
    Alt = 1 << 3,
    Logo = 1 << 4,
    Altgr = 1 << 5
  };

  friend constexpr Modifier operator|(Modifier a, Modifier b) {
    return static_cast<Modifier>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
  }

  friend constexpr Modifier operator&(Modifier a, Modifier b) {
    return static_cast<Modifier>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
  }

  friend constexpr Modifier &operator|=(Modifier &a, Modifier b) {
    a = a | b;
    return a;
  }

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
