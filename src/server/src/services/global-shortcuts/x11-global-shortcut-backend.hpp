#pragma once
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include <cstdint>
#include <expected>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

class QSocketNotifier;

/**
 * X11 global shortcut backend built on XGrabKey (via XCB). Owns its own XCB connection and grabs
 * each shortcut on the root window, so combos fire regardless of the focused window.
 *
 * Only viable on a real X11 session (`platformName() == "xcb"`); under XWayland the root grab does
 * not see native Wayland windows, so the factory never selects it there.
 */
class X11GlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  ~X11GlobalShortcutBackend() override;

  QString id() const override;
  bool start() override;

  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;

private slots:
  void drainEvents();

private:
  struct Binding {
    QString id;
    xcb_keysym_t keysym;
    xcb_keycode_t keycode;
    uint16_t mods; // base modifier mask, without lock-key bits
  };

  std::expected<void, QString> grab(xcb_keycode_t keycode, uint16_t mods);
  void ungrab(xcb_keycode_t keycode, uint16_t mods);
  void regrabAll();
  void computeLockMasks();
  uint16_t modMaskForKeysym(xcb_keysym_t keysym) const;

  xcb_connection_t *m_connection = nullptr;
  xcb_screen_t *m_screen = nullptr;
  xcb_window_t m_root = XCB_WINDOW_NONE;
  xcb_key_symbols_t *m_keysyms = nullptr;
  QSocketNotifier *m_notifier = nullptr;

  uint16_t m_numLockMask = 0;
  uint16_t m_scrollLockMask = 0;
  std::vector<uint16_t> m_lockCombos; // every on/off combination of the detected lock modifiers

  std::vector<Binding> m_binds;
};
