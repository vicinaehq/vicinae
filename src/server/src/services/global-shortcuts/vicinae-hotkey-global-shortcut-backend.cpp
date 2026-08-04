#include "vicinae-hotkey-global-shortcut-backend.hpp"
#include "internal/wayland/xdg-activation.hpp"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include "services/global-shortcuts/xkb-keysym.hpp"
#include <algorithm>
#include <cstdint>
#include <qguiapplication.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <utility>
#include <wayland-client-core.h>

namespace {
std::uint32_t fromQtMods(Qt::KeyboardModifiers mods) {
  using Manager = QtWayland::vicinae_hotkey_manager_v1;

  std::uint32_t m = 0;
  if (mods.testFlag(Qt::KeyboardModifier::ControlModifier)) m |= Manager::modifiers_ctrl;
  if (mods.testFlag(Qt::KeyboardModifier::AltModifier)) m |= Manager::modifiers_alt;
  if (mods.testFlag(Qt::KeyboardModifier::MetaModifier)) m |= Manager::modifiers_super;
  if (mods.testFlag(Qt::KeyboardModifier::ShiftModifier)) m |= Manager::modifiers_shift;

  return m;
}

void wlRoundtrip() {
  auto dp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->display();
  wl_display_roundtrip(dp);
}
} // namespace

VicinaeHotkeyManagerV1::VicinaeHotkeyManagerV1() : QWaylandClientExtensionTemplate(1) { initialize(); }

VicinaeHotkeyGlobalShortcutBackend::Hotkey::Hotkey(VicinaeHotkeyGlobalShortcutBackend *backend,
                                                   struct ::vicinae_hotkey_v1 *object, QString id)
    : QtWayland::vicinae_hotkey_v1(object), m_id(std::move(id)), m_backend(backend) {}

VicinaeHotkeyGlobalShortcutBackend::Hotkey::~Hotkey() {
  if (isInitialized()) destroy();
}

void VicinaeHotkeyGlobalShortcutBackend::Hotkey::vicinae_hotkey_v1_denied(uint32_t reason,
                                                                          const QString &message) {
  if (this == m_backend->m_pendingBind) {
    m_backend->m_pendingDeny = !message.isEmpty() ? message
                                                  : QStringLiteral("Compositor denied the bind. Try "
                                                                   "another key combination.");
    return;
  }

  m_backend->dropHotkey(this);
}

void VicinaeHotkeyGlobalShortcutBackend::Hotkey::vicinae_hotkey_v1_revoked(uint32_t reason,
                                                                           const QString &message) {
  m_backend->dropHotkey(this);
}

void VicinaeHotkeyGlobalShortcutBackend::Hotkey::vicinae_hotkey_v1_pressed(uint32_t serial, uint32_t time) {
  Wayland::XdgActivation::setPendingSerial(serial);
  emit m_backend->shortcutActivated(m_id, time);
}

VicinaeHotkeyGlobalShortcutBackend::~VicinaeHotkeyGlobalShortcutBackend() { unbindAll(); }

QString VicinaeHotkeyGlobalShortcutBackend::id() const { return "vicinae-hotkey"; }

bool VicinaeHotkeyGlobalShortcutBackend::start() {
  emit ready();
  return true;
}

std::expected<void, QString>
VicinaeHotkeyGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  if (!m_manager.isActive()) { return std::unexpected(tr("Compositor does not support global hotkeys")); }

  auto keysym = global_shortcuts::xkbKeysymForQtKey(request.trigger.key());

  if (!keysym) {
    qWarning() << "no xkb keysym matching qt key code" << request.trigger.key();
    return std::unexpected(tr("Unsupported trigger key"));
  }

  auto *handle = m_manager.bind(keysym.value(), fromQtMods(request.trigger.mods()), nullptr,
                                QStringLiteral("vicinae"), request.description);

  m_pendingBind = m_binds.emplace_back(std::make_unique<Hotkey>(this, handle, request.id)).get();
  m_pendingDeny.reset();
  wlRoundtrip();

  auto *bound = std::exchange(m_pendingBind, nullptr);

  if (m_pendingDeny) {
    dropHotkey(bound);
    return std::unexpected(std::move(*m_pendingDeny));
  }

  if (!isTracked(bound)) { return std::unexpected(tr("Hotkey binding was lost")); }

  return {};
}

void VicinaeHotkeyGlobalShortcutBackend::unbindShortcut(const QString &id) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b->m_id == id; }); it != m_binds.end()) {
    m_binds.erase(it);
  }

  wlRoundtrip();
}

void VicinaeHotkeyGlobalShortcutBackend::unbindAll() {
  m_binds.clear();
  wlRoundtrip();
}

bool VicinaeHotkeyGlobalShortcutBackend::isTracked(const Hotkey *hotkey) const {
  return std::ranges::any_of(m_binds, [&](auto &&b) { return b.get() == hotkey; });
}

void VicinaeHotkeyGlobalShortcutBackend::dropHotkey(Hotkey *hotkey) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.get() == hotkey; });
      it != m_binds.end()) {
    m_binds.erase(it);
  }
}
