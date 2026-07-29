#include "vicinae-hotkey-global-shortcut-backend.hpp"
#include "vicinae-hotkey-v1-client-protocol.h"
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
  if (mods.testFlag(Qt::KeyboardModifier::ControlModifier)) m |= VICINAE_HOTKEY_MANAGER_V1_MODIFIERS_CTRL;
  if (mods.testFlag(Qt::KeyboardModifier::AltModifier)) m |= VICINAE_HOTKEY_MANAGER_V1_MODIFIERS_ALT;
  if (mods.testFlag(Qt::KeyboardModifier::MetaModifier)) m |= VICINAE_HOTKEY_MANAGER_V1_MODIFIERS_SUPER;
  if (mods.testFlag(Qt::KeyboardModifier::ShiftModifier)) m |= VICINAE_HOTKEY_MANAGER_V1_MODIFIERS_SHIFT;

  return m;
}

void wlRoundtrip() {
  auto dp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->display();
  wl_display_roundtrip(dp);
}
} // namespace

VicinaeHotkeyGlobalShortcutBackend::~VicinaeHotkeyGlobalShortcutBackend() { unbindAll(); }

QString VicinaeHotkeyGlobalShortcutBackend::id() const { return "vicinae-hotkey"; }

bool VicinaeHotkeyGlobalShortcutBackend::start() {
  emit ready();
  return true;
}

std::expected<void, QString>
VicinaeHotkeyGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  auto manager = Wayland::Globals::hotkey();
  auto keysym = global_shortcuts::xkbKeysymForQtKey(request.trigger.key());

  if (!keysym) {
    qWarning() << "no xkb keysym matching qt key code" << request.trigger.key();
    return std::unexpected(tr("Unsupported trigger key"));
  }

  auto description = request.description.toUtf8();
  auto handle = vicinae_hotkey_manager_v1_bind(manager, keysym.value(), fromQtMods(request.trigger.mods()),
                                               nullptr, "vicinae", description.constData());

  vicinae_hotkey_v1_add_listener(handle, &listener, this);
  m_binds.emplace_back(HotkeyInfo{.handle = handle, .id = request.id});

  m_pendingBind = handle;
  m_pendingDeny.reset();
  wlRoundtrip();
  m_pendingBind = nullptr;

  if (m_pendingDeny) {
    dropHotkey(handle);
    return std::unexpected(std::move(*m_pendingDeny));
  }

  if (!findHotkey(handle)) { return std::unexpected(tr("Hotkey binding was lost")); }

  return {};
}

void VicinaeHotkeyGlobalShortcutBackend::unbindShortcut(const QString &id) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.id == id; }); it != m_binds.end()) {
    vicinae_hotkey_v1_destroy(it->handle);
    m_binds.erase(it);
  }

  wlRoundtrip();
}

void VicinaeHotkeyGlobalShortcutBackend::unbindAll() {
  for (const auto &bind : m_binds) {
    vicinae_hotkey_v1_destroy(bind.handle);
  }
  m_binds.clear();
  wlRoundtrip();
}

VicinaeHotkeyGlobalShortcutBackend::HotkeyInfo *
VicinaeHotkeyGlobalShortcutBackend::findHotkey(vicinae_hotkey_v1 *hotkey) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.handle == hotkey; });
      it != m_binds.end()) {
    return &*it;
  }
  return nullptr;
}

void VicinaeHotkeyGlobalShortcutBackend::dropHotkey(vicinae_hotkey_v1 *hotkey) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.handle == hotkey; });
      it != m_binds.end()) {
    vicinae_hotkey_v1_destroy(it->handle);
    m_binds.erase(it);
  }
}

void VicinaeHotkeyGlobalShortcutBackend::denied(void *data, vicinae_hotkey_v1 *hotkey, uint32_t reason,
                                                const char *msg) {
  auto self = static_cast<VicinaeHotkeyGlobalShortcutBackend *>(data);

  if (hotkey == self->m_pendingBind) {
    self->m_pendingDeny = *msg ? QString::fromUtf8(msg)
                               : QStringLiteral("Compositor denied the bind. Try another key "
                                                "combination.");
    return;
  }

  self->dropHotkey(hotkey);
}

void VicinaeHotkeyGlobalShortcutBackend::revoked(void *data, vicinae_hotkey_v1 *hotkey, uint32_t reason,
                                                 const char *msg) {
  auto self = static_cast<VicinaeHotkeyGlobalShortcutBackend *>(data);
  self->dropHotkey(hotkey);
}

void VicinaeHotkeyGlobalShortcutBackend::bound(void *data, vicinae_hotkey_v1 *hotkey) {
  auto self = static_cast<VicinaeHotkeyGlobalShortcutBackend *>(data);

  if (!self->findHotkey(hotkey)) { qWarning() << "vicinae-hotkey: bound event for an untracked hotkey"; }
}

void VicinaeHotkeyGlobalShortcutBackend::pressed(void *data, struct vicinae_hotkey_v1 *hotkey,
                                                 uint32_t serial, uint32_t time) {
  auto self = static_cast<VicinaeHotkeyGlobalShortcutBackend *>(data);

  if (auto bind = self->findHotkey(hotkey)) { emit self->shortcutActivated(bind->id, time); }
}

void VicinaeHotkeyGlobalShortcutBackend::released(void *data, struct vicinae_hotkey_v1 *hotkey,
                                                  uint32_t serial, uint32_t time) {
  // no-op, we don't care about release
}
