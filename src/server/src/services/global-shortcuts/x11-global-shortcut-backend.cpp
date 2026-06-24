#include "x11-global-shortcut-backend.hpp"
#include "common/c-ptr.hpp"
#include "services/global-shortcuts/xkb-keysym.hpp"
#include <QSocketNotifier>
#include <qlogging.h>
#include <xcb/xproto.h>
#include <xkbcommon/xkbcommon-keysyms.h>

namespace {
uint16_t fromQtMods(Qt::KeyboardModifiers mods) {
  uint16_t m = 0;
  if (mods.testFlag(Qt::ControlModifier)) m |= XCB_MOD_MASK_CONTROL;
  if (mods.testFlag(Qt::ShiftModifier)) m |= XCB_MOD_MASK_SHIFT;
  if (mods.testFlag(Qt::AltModifier)) m |= XCB_MOD_MASK_1;
  if (mods.testFlag(Qt::MetaModifier)) m |= XCB_MOD_MASK_4;
  return m;
}

constexpr uint16_t MODIFIER_BITS =
    XCB_MOD_MASK_SHIFT | XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_1 | XCB_MOD_MASK_4;
} // namespace

X11GlobalShortcutBackend::~X11GlobalShortcutBackend() {
  if (m_notifier) { m_notifier->setEnabled(false); }
  if (m_connection) {
    unbindAll();
    if (m_keysyms) { xcb_key_symbols_free(m_keysyms); }
    xcb_disconnect(m_connection);
  }
}

QString X11GlobalShortcutBackend::id() const { return "x11"; }

bool X11GlobalShortcutBackend::start() {
  m_connection = xcb_connect(nullptr, nullptr);
  if (xcb_connection_has_error(m_connection)) {
    qWarning() << "X11GlobalShortcutBackend: xcb_connect failed";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    return false;
  }

  const xcb_setup_t *setup = xcb_get_setup(m_connection);
  xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
  if (it.rem == 0) {
    qWarning() << "X11GlobalShortcutBackend: no screens available";
    xcb_disconnect(m_connection);
    m_connection = nullptr;
    return false;
  }

  m_screen = it.data;
  m_root = m_screen->root;
  m_keysyms = xcb_key_symbols_alloc(m_connection);
  computeLockMasks();

  int const fd = xcb_get_file_descriptor(m_connection);
  m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
  connect(m_notifier, &QSocketNotifier::activated, this, &X11GlobalShortcutBackend::drainEvents);

  emit ready();
  return true;
}

void X11GlobalShortcutBackend::computeLockMasks() {
  m_numLockMask = modMaskForKeysym(XKB_KEY_Num_Lock);
  m_scrollLockMask = modMaskForKeysym(XKB_KEY_Scroll_Lock);

  // XGrabKey matches modifiers exactly, so to fire regardless of lock state we grab every on/off
  // combination of the active lock modifiers (CapsLock is always the dedicated Lock modifier).
  std::vector<uint16_t> active;
  active.reserve(3);
  active.emplace_back(XCB_MOD_MASK_LOCK);
  if (m_numLockMask) { active.emplace_back(m_numLockMask); }
  if (m_scrollLockMask) { active.emplace_back(m_scrollLockMask); }

  m_lockCombos.clear();
  m_lockCombos.reserve(1u << active.size());
  for (size_t subset = 0; subset < (1u << active.size()); ++subset) {
    uint16_t mask = 0;
    for (size_t i = 0; i < active.size(); ++i) {
      if (subset & (1u << i)) { mask |= active[i]; }
    }
    // Dedupe: grabbing the same combo twice on one connection raises BadAccess.
    if (std::ranges::find(m_lockCombos, mask) == m_lockCombos.end()) { m_lockCombos.emplace_back(mask); }
  }
}

uint16_t X11GlobalShortcutBackend::modMaskForKeysym(xcb_keysym_t keysym) const {
  CPtr<xcb_keycode_t> keycodes(xcb_key_symbols_get_keycode(m_keysyms, keysym));
  if (!keycodes) { return 0; }

  xcb_get_modifier_mapping_cookie_t const cookie = xcb_get_modifier_mapping(m_connection);
  CPtr<xcb_get_modifier_mapping_reply_t> reply(xcb_get_modifier_mapping_reply(m_connection, cookie, nullptr));
  if (!reply) { return 0; }

  const xcb_keycode_t *codes = xcb_get_modifier_mapping_keycodes(reply.get());
  int const perMod = reply->keycodes_per_modifier;

  for (int mod = 0; mod < 8; ++mod) {
    for (int j = 0; j < perMod; ++j) {
      xcb_keycode_t const code = codes[(mod * perMod) + j];
      if (code == XCB_NO_SYMBOL) { continue; }
      for (xcb_keycode_t *kc = keycodes.get(); *kc != XCB_NO_SYMBOL; ++kc) {
        if (*kc == code) { return static_cast<uint16_t>(1u << mod); }
      }
    }
  }

  return 0;
}

std::expected<void, QString> X11GlobalShortcutBackend::grab(xcb_keycode_t keycode, uint16_t mods) {
  for (size_t i = 0; i < m_lockCombos.size(); ++i) {
    xcb_void_cookie_t const cookie = xcb_grab_key_checked(m_connection, 0, m_root, mods | m_lockCombos[i],
                                                          keycode, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    if (CPtr<xcb_generic_error_t> err{xcb_request_check(m_connection, cookie)}) {
      for (size_t j = 0; j < i; ++j) {
        xcb_ungrab_key(m_connection, keycode, m_root, mods | m_lockCombos[j]);
      }
      xcb_flush(m_connection);
      return std::unexpected(QStringLiteral("This shortcut is already in use by another application"));
    }
  }

  return {};
}

void X11GlobalShortcutBackend::ungrab(xcb_keycode_t keycode, uint16_t mods) {
  for (uint16_t const lock : m_lockCombos) {
    xcb_ungrab_key(m_connection, keycode, m_root, mods | lock);
  }
}

std::expected<void, QString> X11GlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  auto keysym = global_shortcuts::xkbKeysymForQtKey(request.trigger.key());
  if (!keysym) {
    qWarning() << "X11GlobalShortcutBackend: no xkb keysym for qt key" << request.trigger.key();
    return std::unexpected(QStringLiteral("Unsupported trigger key"));
  }

  CPtr<xcb_keycode_t> keycodes(xcb_key_symbols_get_keycode(m_keysyms, *keysym));
  if (!keycodes || keycodes.get()[0] == XCB_NO_SYMBOL) {
    return std::unexpected(QStringLiteral("Trigger key is not present on this keyboard"));
  }

  xcb_keycode_t const keycode = keycodes.get()[0];
  uint16_t const mods = fromQtMods(request.trigger.mods());

  if (auto grabbed = grab(keycode, mods); !grabbed) { return grabbed; }

  xcb_flush(m_connection);
  m_binds.emplace_back(Binding{.id = request.id, .keysym = *keysym, .keycode = keycode, .mods = mods});
  return {};
}

void X11GlobalShortcutBackend::unbindShortcut(const QString &id) {
  auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.id == id; });
  if (it == m_binds.end()) { return; }

  ungrab(it->keycode, it->mods);
  xcb_flush(m_connection);
  m_binds.erase(it);
}

void X11GlobalShortcutBackend::unbindAll() {
  for (const auto &bind : m_binds) {
    ungrab(bind.keycode, bind.mods);
  }
  xcb_flush(m_connection);
  m_binds.clear();
}

void X11GlobalShortcutBackend::regrabAll() {
  xcb_ungrab_key(m_connection, XCB_GRAB_ANY, m_root, XCB_MOD_MASK_ANY);
  computeLockMasks();

  for (auto &bind : m_binds) {
    CPtr<xcb_keycode_t> keycodes(xcb_key_symbols_get_keycode(m_keysyms, bind.keysym));
    if (keycodes && keycodes.get()[0] != XCB_NO_SYMBOL) { bind.keycode = keycodes.get()[0]; }
    std::ignore = (grab(bind.keycode, bind.mods));
  }

  xcb_flush(m_connection);
}

void X11GlobalShortcutBackend::drainEvents() {
  m_notifier->setEnabled(false);

  for (CPtr<xcb_generic_event_t> event{xcb_poll_for_event(m_connection)}; event;
       event.reset(xcb_poll_for_event(m_connection))) {
    switch (event->response_type & ~0x80) {
    case XCB_KEY_PRESS: {
      auto *key = reinterpret_cast<xcb_key_press_event_t *>(event.get());
      // Auto-repeat arrives as a KeyRelease/KeyPress pair sharing a timestamp; skip the repeat press.
      bool const isRepeat =
          m_lastReleaseValid && m_lastReleaseCode == key->detail && m_lastReleaseTime == key->time;
      m_lastReleaseValid = false;
      if (isRepeat) { break; }

      uint16_t const mods = key->state & MODIFIER_BITS;
      auto it =
          std::ranges::find_if(m_binds, [&](auto &&b) { return b.keycode == key->detail && b.mods == mods; });
      if (it != m_binds.end()) { emit shortcutActivated(it->id, key->time); }
      break;
    }
    case XCB_KEY_RELEASE: {
      auto *key = reinterpret_cast<xcb_key_release_event_t *>(event.get());
      m_lastReleaseCode = key->detail;
      m_lastReleaseTime = key->time;
      m_lastReleaseValid = true;
      break;
    }
    case XCB_MAPPING_NOTIFY: {
      auto *mapping = reinterpret_cast<xcb_mapping_notify_event_t *>(event.get());
      xcb_refresh_keyboard_mapping(m_keysyms, mapping);
      if (mapping->request != XCB_MAPPING_POINTER) { regrabAll(); }
      break;
    }
    default:
      break;
    }
  }

  m_notifier->setEnabled(true);
}
