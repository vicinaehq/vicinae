#include "xx-hotkey-global-shortcut-backend.hpp"
#include "internal/wayland/xdg-activation.hpp"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include "services/global-shortcuts/xkb-keysym.hpp"
#include "vicinae.hpp"
#include <algorithm>
#include <cstdint>
#include <qguiapplication.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <utility>
#include <variant>
#include <wayland-client-core.h>

namespace {
std::uint32_t fromQtMods(Qt::KeyboardModifiers mods) {
  using Manager = QtWayland::xx_hotkey_manager_v1;

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

XxHotkeyManagerV1::XxHotkeyManagerV1() : QWaylandClientExtensionTemplate(1) { initialize(); }

XxHotkeyGlobalShortcutBackend::Hotkey::Hotkey(XxHotkeyGlobalShortcutBackend *backend,
                                              struct ::xx_hotkey_v1 *object, QString id)
    : QtWayland::xx_hotkey_v1(object), m_id(std::move(id)), m_backend(backend) {}

XxHotkeyGlobalShortcutBackend::Hotkey::~Hotkey() {
  if (isInitialized()) destroy();
}

void XxHotkeyGlobalShortcutBackend::Hotkey::apply(const Trigger &trigger, const QString &description) {
  if (!description.isEmpty()) set_description(description);

  std::visit(
      [&](auto &&t) {
        using T = std::decay_t<decltype(t)>;
        if constexpr (std::is_same_v<T, KeyTrigger>) {
          set_key_trigger(t.keysym, t.modifiers);
        } else {
          set_button_trigger(t.button, t.modifiers);
        }
      },
      trigger);

  m_state = State::Pending;
  m_message.clear();
  commit();
}

void XxHotkeyGlobalShortcutBackend::Hotkey::xx_hotkey_v1_bound() { m_state = State::Bound; }

void XxHotkeyGlobalShortcutBackend::Hotkey::xx_hotkey_v1_denied(uint32_t reason, const QString &message) {
  m_state = State::Denied;
  m_message = message;

  if (this != m_backend->m_pendingBind) {
    qWarning() << "global hotkey" << m_id << "denied by compositor after bind:" << message;
    m_backend->dropHotkey(this);
  }
}

void XxHotkeyGlobalShortcutBackend::Hotkey::xx_hotkey_v1_revoked(const QString &message) {
  qWarning() << "global hotkey" << m_id << "revoked by compositor:" << message;
  m_backend->dropHotkey(this);
}

void XxHotkeyGlobalShortcutBackend::Hotkey::xx_hotkey_v1_triggered(uint32_t serial, uint32_t time) {
  Wayland::XdgActivation::setPendingSerial(serial);
  emit m_backend->shortcutActivated(m_id, time);
}

XxHotkeyGlobalShortcutBackend::XxHotkeyGlobalShortcutBackend() {
  if (m_manager.isActive()) m_manager.set_app_id(Omnicast::APP_ID);
}

XxHotkeyGlobalShortcutBackend::~XxHotkeyGlobalShortcutBackend() { unbindAll(); }

QString XxHotkeyGlobalShortcutBackend::id() const { return "xx-hotkey"; }

bool XxHotkeyGlobalShortcutBackend::start() {
  emit ready();
  return true;
}

std::expected<void, QString>
XxHotkeyGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  auto keysym = global_shortcuts::xkbKeysymForQtKey(request.trigger.key());

  if (!keysym) {
    qWarning() << "no xkb keysym matching qt key code" << request.trigger.key();
    return std::unexpected(tr("Unsupported trigger key"));
  }

  return bind(request.id, KeyTrigger{.keysym = *keysym, .modifiers = fromQtMods(request.trigger.mods())},
              request.description);
}

std::expected<void, QString> XxHotkeyGlobalShortcutBackend::bind(const QString &id, const Trigger &trigger,
                                                                 const QString &description) {
  if (!m_manager.isActive()) { return std::unexpected(tr("Compositor does not support global hotkeys")); }

  auto *hotkey = m_binds.emplace_back(std::make_unique<Hotkey>(this, m_manager.create_hotkey(), id)).get();

  m_pendingBind = hotkey;
  hotkey->apply(trigger, description);
  wlRoundtrip();
  m_pendingBind = nullptr;

  if (!isTracked(hotkey)) { return std::unexpected(tr("Hotkey binding was lost")); }

  if (hotkey->m_state == Hotkey::State::Denied) {
    auto message = !hotkey->m_message.isEmpty()
                       ? hotkey->m_message
                       : tr("Compositor denied the bind. Try another key combination.");
    dropHotkey(hotkey);
    return std::unexpected(std::move(message));
  }

  if (hotkey->m_state == Hotkey::State::Pending) {
    qWarning() << "compositor did not answer the commit for global hotkey" << id << "within a roundtrip";
  }

  return {};
}

void XxHotkeyGlobalShortcutBackend::unbindShortcut(const QString &id) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b->m_id == id; }); it != m_binds.end()) {
    m_binds.erase(it);
  }

  wlRoundtrip();
}

void XxHotkeyGlobalShortcutBackend::unbindAll() {
  m_binds.clear();
  wlRoundtrip();
}

bool XxHotkeyGlobalShortcutBackend::isTracked(const Hotkey *hotkey) const {
  return std::ranges::any_of(m_binds, [&](auto &&b) { return b.get() == hotkey; });
}

void XxHotkeyGlobalShortcutBackend::dropHotkey(Hotkey *hotkey) {
  if (auto it = std::ranges::find_if(m_binds, [&](auto &&b) { return b.get() == hotkey; });
      it != m_binds.end()) {
    m_binds.erase(it);
  }
}
