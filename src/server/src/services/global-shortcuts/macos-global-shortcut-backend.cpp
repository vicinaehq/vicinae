#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"

// Keep AssertMacros from defining bare check()/verify()/require(), which collide with Qt and the STL.
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <Carbon/Carbon.h>

namespace {

constexpr OSType HOT_KEY_SIGNATURE = 'vici';

std::optional<uint32_t> macVirtualKeyForQtKey(Qt::Key key) {
  switch (key) {
  case Qt::Key_A:
    return kVK_ANSI_A;
  case Qt::Key_B:
    return kVK_ANSI_B;
  case Qt::Key_C:
    return kVK_ANSI_C;
  case Qt::Key_D:
    return kVK_ANSI_D;
  case Qt::Key_E:
    return kVK_ANSI_E;
  case Qt::Key_F:
    return kVK_ANSI_F;
  case Qt::Key_G:
    return kVK_ANSI_G;
  case Qt::Key_H:
    return kVK_ANSI_H;
  case Qt::Key_I:
    return kVK_ANSI_I;
  case Qt::Key_J:
    return kVK_ANSI_J;
  case Qt::Key_K:
    return kVK_ANSI_K;
  case Qt::Key_L:
    return kVK_ANSI_L;
  case Qt::Key_M:
    return kVK_ANSI_M;
  case Qt::Key_N:
    return kVK_ANSI_N;
  case Qt::Key_O:
    return kVK_ANSI_O;
  case Qt::Key_P:
    return kVK_ANSI_P;
  case Qt::Key_Q:
    return kVK_ANSI_Q;
  case Qt::Key_R:
    return kVK_ANSI_R;
  case Qt::Key_S:
    return kVK_ANSI_S;
  case Qt::Key_T:
    return kVK_ANSI_T;
  case Qt::Key_U:
    return kVK_ANSI_U;
  case Qt::Key_V:
    return kVK_ANSI_V;
  case Qt::Key_W:
    return kVK_ANSI_W;
  case Qt::Key_X:
    return kVK_ANSI_X;
  case Qt::Key_Y:
    return kVK_ANSI_Y;
  case Qt::Key_Z:
    return kVK_ANSI_Z;
  case Qt::Key_0:
    return kVK_ANSI_0;
  case Qt::Key_1:
    return kVK_ANSI_1;
  case Qt::Key_2:
    return kVK_ANSI_2;
  case Qt::Key_3:
    return kVK_ANSI_3;
  case Qt::Key_4:
    return kVK_ANSI_4;
  case Qt::Key_5:
    return kVK_ANSI_5;
  case Qt::Key_6:
    return kVK_ANSI_6;
  case Qt::Key_7:
    return kVK_ANSI_7;
  case Qt::Key_8:
    return kVK_ANSI_8;
  case Qt::Key_9:
    return kVK_ANSI_9;
  case Qt::Key_Space:
    return kVK_Space;
  case Qt::Key_Return:
    return kVK_Return;
  case Qt::Key_Enter:
    return kVK_ANSI_KeypadEnter;
  case Qt::Key_Escape:
    return kVK_Escape;
  case Qt::Key_Tab:
    return kVK_Tab;
  case Qt::Key_Backspace:
    return kVK_Delete;
  case Qt::Key_Delete:
    return kVK_ForwardDelete;
  case Qt::Key_Home:
    return kVK_Home;
  case Qt::Key_End:
    return kVK_End;
  case Qt::Key_PageUp:
    return kVK_PageUp;
  case Qt::Key_PageDown:
    return kVK_PageDown;
  case Qt::Key_Left:
    return kVK_LeftArrow;
  case Qt::Key_Right:
    return kVK_RightArrow;
  case Qt::Key_Up:
    return kVK_UpArrow;
  case Qt::Key_Down:
    return kVK_DownArrow;
  case Qt::Key_Minus:
    return kVK_ANSI_Minus;
  case Qt::Key_Equal:
    return kVK_ANSI_Equal;
  case Qt::Key_BracketLeft:
    return kVK_ANSI_LeftBracket;
  case Qt::Key_BracketRight:
    return kVK_ANSI_RightBracket;
  case Qt::Key_Backslash:
    return kVK_ANSI_Backslash;
  case Qt::Key_Semicolon:
    return kVK_ANSI_Semicolon;
  case Qt::Key_Apostrophe:
    return kVK_ANSI_Quote;
  case Qt::Key_Comma:
    return kVK_ANSI_Comma;
  case Qt::Key_Period:
    return kVK_ANSI_Period;
  case Qt::Key_Slash:
    return kVK_ANSI_Slash;
  case Qt::Key_QuoteLeft:
    return kVK_ANSI_Grave;
  case Qt::Key_F1:
    return kVK_F1;
  case Qt::Key_F2:
    return kVK_F2;
  case Qt::Key_F3:
    return kVK_F3;
  case Qt::Key_F4:
    return kVK_F4;
  case Qt::Key_F5:
    return kVK_F5;
  case Qt::Key_F6:
    return kVK_F6;
  case Qt::Key_F7:
    return kVK_F7;
  case Qt::Key_F8:
    return kVK_F8;
  case Qt::Key_F9:
    return kVK_F9;
  case Qt::Key_F10:
    return kVK_F10;
  case Qt::Key_F11:
    return kVK_F11;
  case Qt::Key_F12:
    return kVK_F12;
  default:
    return std::nullopt;
  }
}

// On macOS Qt swaps Ctrl/Meta: ControlModifier is the Cmd key, MetaModifier is the Control key.
uint32_t carbonModifiers(Qt::KeyboardModifiers mods) {
  uint32_t carbon = 0;
  if (mods.testFlag(Qt::ControlModifier)) { carbon |= cmdKey; }
  if (mods.testFlag(Qt::MetaModifier)) { carbon |= controlKey; }
  if (mods.testFlag(Qt::AltModifier)) { carbon |= optionKey; }
  if (mods.testFlag(Qt::ShiftModifier)) { carbon |= shiftKey; }
  return carbon;
}

OSStatus hotKeyHandler(EventHandlerCallRef, EventRef event, void *userData) {
  EventHotKeyID hotKeyId{};
  if (GetEventParameter(event, kEventParamDirectObject, typeEventHotKeyID, nullptr, sizeof(hotKeyId), nullptr,
                        &hotKeyId) != noErr) {
    return eventNotHandledErr;
  }

  const auto timestamp = static_cast<quint64>(GetEventTime(event) * 1000.0);
  static_cast<MacOSGlobalShortcutBackend *>(userData)->handleHotKey(hotKeyId.id, timestamp);
  return noErr;
}

} // namespace

MacOSGlobalShortcutBackend::MacOSGlobalShortcutBackend() = default;

MacOSGlobalShortcutBackend::~MacOSGlobalShortcutBackend() {
  for (auto &[id, binding] : m_bindings) {
    if (binding.ref) { UnregisterEventHotKey(static_cast<EventHotKeyRef>(binding.ref)); }
  }
  if (m_handler) { RemoveEventHandler(static_cast<EventHandlerRef>(m_handler)); }
}

bool MacOSGlobalShortcutBackend::start() {
  if (m_started) { return true; }

  const EventTypeSpec spec{kEventClassKeyboard, kEventHotKeyPressed};
  EventHandlerRef handlerRef = nullptr;
  if (InstallApplicationEventHandler(&hotKeyHandler, 1, &spec, this, &handlerRef) != noErr) { return false; }

  m_handler = handlerRef;
  m_started = true;
  emit ready();
  return true;
}

std::expected<void, QString> MacOSGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  const auto keyCode = macVirtualKeyForQtKey(request.trigger.key());
  if (!keyCode) { return std::unexpected(QStringLiteral("unsupported or invalid trigger")); }

  const uint32_t carbonId = m_nextCarbonId++;
  const EventHotKeyID hotKeyId{.signature = HOT_KEY_SIGNATURE, .id = carbonId};
  EventHotKeyRef ref = nullptr;
  const OSStatus status = RegisterEventHotKey(*keyCode, carbonModifiers(request.trigger.mods()), hotKeyId,
                                              GetApplicationEventTarget(), 0, &ref);

  if (status != noErr || !ref) {
    return std::unexpected(QStringLiteral("RegisterEventHotKey failed (%1)").arg(status));
  }

  m_bindings.emplace(request.id, Binding{.ref = ref, .carbonId = carbonId});
  m_idByCarbonId.emplace(carbonId, request.id);
  return {};
}

void MacOSGlobalShortcutBackend::unbindShortcut(const QString &id) {
  const auto it = m_bindings.find(id);
  if (it == m_bindings.end()) { return; }

  if (it->second.ref) {
    UnregisterEventHotKey(static_cast<EventHotKeyRef>(it->second.ref));
    m_idByCarbonId.erase(it->second.carbonId);
  }

  m_bindings.erase(it);
}

void MacOSGlobalShortcutBackend::unbindAll() {
  for (auto &[id, binding] : m_bindings) {
    if (binding.ref) { UnregisterEventHotKey(static_cast<EventHotKeyRef>(binding.ref)); }
  }
  m_bindings.clear();
  m_idByCarbonId.clear();
}

void MacOSGlobalShortcutBackend::handleHotKey(uint32_t carbonId, quint64 timestamp) {
  if (const auto it = m_idByCarbonId.find(carbonId); it != m_idByCarbonId.end()) {
    emit shortcutActivated(it->second, timestamp);
  }
}
