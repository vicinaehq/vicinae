#include "ext-hotkey-global-shortcut-backend.hpp"
#include "ext-hotkey-v1-client-protocol.h"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include "services/global-shortcuts/xkb-keysym.hpp"
#include "wayland/globals.hpp"
#include <algorithm>
#include <cstdint>
#include <qguiapplication.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <vector>
#include <wayland-client-core.h>

namespace {
std::uint32_t fromQtMods(Qt::KeyboardModifiers mods) {
  std::uint32_t m = 0;
  if (mods.testFlag(Qt::KeyboardModifier::ControlModifier)) m |= EXT_HOTKEY_MANAGER_V1_MODIFIERS_CTRL;
  if (mods.testFlag(Qt::KeyboardModifier::AltModifier)) m |= EXT_HOTKEY_MANAGER_V1_MODIFIERS_ALT;
  if (mods.testFlag(Qt::KeyboardModifier::MetaModifier)) m |= EXT_HOTKEY_MANAGER_V1_MODIFIERS_SUPER;
  if (mods.testFlag(Qt::KeyboardModifier::ShiftModifier)) m |= EXT_HOTKEY_MANAGER_V1_MODIFIERS_SHIFT;

  return m;
}

void wlRoundtrip() {
  auto dp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->display();
  wl_display_roundtrip(dp);
}
} // namespace

ExtHotkeyGlobalShortcutBackend::~ExtHotkeyGlobalShortcutBackend() { unbindAll(); }

QString ExtHotkeyGlobalShortcutBackend::id() const { return "ext-hotkey"; }

bool ExtHotkeyGlobalShortcutBackend::start() {
  emit ready();
  return true;
}

std::expected<void, QString>
ExtHotkeyGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  auto manager = Wayland::Globals::hotkey();
  auto keysym = global_shortcuts::xkbKeysymForQtKey(request.trigger.key());

  if (!keysym) {
    qWarning() << "no xkb keysym matching qt key code" << request.trigger.key();
    return std::unexpected(QStringLiteral("Unsupported trigger key"));
  }

  auto description = request.description.toUtf8();
  auto handle = ext_hotkey_manager_v1_bind(manager, keysym.value(), fromQtMods(request.trigger.mods()),
                                           nullptr, "vicinae", description.constData());

  ext_hotkey_v1_add_listener(handle, &listener, this);
  m_binds.emplace_back(HotkeyInfo{.handle = handle, .id = request.id});

  m_pendingBind = handle;
  m_pendingDeny.reset();
  wlRoundtrip();
  m_pendingBind = nullptr;

  if (m_pendingDeny) {
    dropHotkey(handle);
    return std::unexpected(std::move(*m_pendingDeny));
  }

  if (!findHotkey(handle)) { return std::unexpected(QStringLiteral("Hotkey binding was lost")); }

  return {};
}

void ExtHotkeyGlobalShortcutBackend::unbindShortcut(const QString &id) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.id == id; }); it != m_binds.end()) {
    ext_hotkey_v1_destroy(it->handle);
    m_binds.erase(it);
  }

  wlRoundtrip();
}

void ExtHotkeyGlobalShortcutBackend::unbindAll() {
  for (const auto &bind : m_binds) {
    ext_hotkey_v1_destroy(bind.handle);
  }
  m_binds.clear();
  wlRoundtrip();
}

ExtHotkeyGlobalShortcutBackend::HotkeyInfo *
ExtHotkeyGlobalShortcutBackend::findHotkey(ext_hotkey_v1 *hotkey) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.handle == hotkey; });
      it != m_binds.end()) {
    return &*it;
  }
  return nullptr;
}

void ExtHotkeyGlobalShortcutBackend::dropHotkey(ext_hotkey_v1 *hotkey) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.handle == hotkey; });
      it != m_binds.end()) {
    ext_hotkey_v1_destroy(it->handle);
    m_binds.erase(it);
  }
}

void ExtHotkeyGlobalShortcutBackend::denied(void *data, ext_hotkey_v1 *hotkey, uint32_t reason,
                                            const char *msg) {
  auto self = static_cast<ExtHotkeyGlobalShortcutBackend *>(data);

  if (hotkey == self->m_pendingBind) {
    self->m_pendingDeny = *msg ? QString::fromUtf8(msg)
                               : QStringLiteral("Compositor denied the bind. Try another key "
                                                "combination.");
    return;
  }

  self->dropHotkey(hotkey);
}

void ExtHotkeyGlobalShortcutBackend::revoked(void *data, ext_hotkey_v1 *hotkey, uint32_t reason,
                                             const char *msg) {
  auto self = static_cast<ExtHotkeyGlobalShortcutBackend *>(data);
  self->dropHotkey(hotkey);
}

void ExtHotkeyGlobalShortcutBackend::bound(void *data, ext_hotkey_v1 *hotkey) {
  auto self = static_cast<ExtHotkeyGlobalShortcutBackend *>(data);

  if (!self->findHotkey(hotkey)) { qWarning() << "ext-hotkey: bound event for an untracked hotkey"; }
}

void ExtHotkeyGlobalShortcutBackend::pressed(void *data, struct ext_hotkey_v1 *hotkey, uint32_t serial,
                                             uint32_t time) {
  auto self = static_cast<ExtHotkeyGlobalShortcutBackend *>(data);

  if (auto bind = self->findHotkey(hotkey)) { emit self->shortcutActivated(bind->id, time); }
}

void ExtHotkeyGlobalShortcutBackend::released(void *data, struct ext_hotkey_v1 *hotkey, uint32_t serial,
                                              uint32_t time) {
  // no-op, we don't care about release
}
