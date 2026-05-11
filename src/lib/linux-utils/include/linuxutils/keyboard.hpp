#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <fcntl.h>
#include <linux/uinput.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

namespace linuxutils {

/**
 * Virtual keyboard device using /dev/uinput.
 * Operates at the scan code level so it works on every linux environment.
 * Supports both raw scancode injection and character-level typing via xkbcommon.
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
  static constexpr int DEFAULT_KEY_DELAY_US = 2000;

  UInputKeyboard();
  ~UInputKeyboard();

  void sendKey(int code, int mods);
  void repeatKey(int code, int n);
  void typeText(std::string_view text);
  void setKeyDelay(int us) { m_keyDelayUs = us; }

  int fd() const { return m_fd; }
  std::optional<std::string> error() const { return m_error; }

private:
  struct KeyRecord {
    uint32_t code = 0;
    int mods = 0;
  };

  static constexpr size_t CHARMAP_SIZE = 128;

  void buildCharMap();
  void sync();
  void keyup(int code);
  void keydown(int code);
  void sendKey(int code);
  void applyMods(int mods);
  void clearMods(int mods);

  std::optional<std::string> m_error;
  int m_fd = -1;
  int m_keyDelayUs = DEFAULT_KEY_DELAY_US;

  xkb_context *m_xkbCtx = nullptr;
  xkb_keymap *m_xkbKeymap = nullptr;
  std::array<KeyRecord, CHARMAP_SIZE> m_charMap{};
};
} // namespace linuxutils
