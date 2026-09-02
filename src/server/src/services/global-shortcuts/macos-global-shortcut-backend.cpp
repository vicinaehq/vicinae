#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"
#include "keyboard/keyboard-macos.hpp"

#include <algorithm>
#include <QChar>
#include <QDebug>

namespace {

constexpr OSType HOT_KEY_SIGNATURE = 'vici';
constexpr uint32_t MAX_LAYOUT_KEYCODE = 128;

// On macOS Qt swaps Ctrl/Meta: ControlModifier is the Cmd key, MetaModifier is the Control key.
uint64_t cgFlagsForQtModifiers(Qt::KeyboardModifiers mods) {
  uint64_t flags = 0;
  if (mods.testFlag(Qt::ControlModifier)) { flags |= kCGEventFlagMaskCommand; }
  if (mods.testFlag(Qt::MetaModifier)) { flags |= kCGEventFlagMaskControl; }
  if (mods.testFlag(Qt::AltModifier)) { flags |= kCGEventFlagMaskAlternate; }
  if (mods.testFlag(Qt::ShiftModifier)) { flags |= kCGEventFlagMaskShift; }
  return flags;
}

uint32_t carbonModifiersFromCGFlags(uint64_t flags) {
  uint32_t carbon = 0;
  if (flags & kCGEventFlagMaskCommand) { carbon |= cmdKey; }
  if (flags & kCGEventFlagMaskControl) { carbon |= controlKey; }
  if (flags & kCGEventFlagMaskAlternate) { carbon |= optionKey; }
  if (flags & kCGEventFlagMaskShift) { carbon |= shiftKey; }
  return carbon;
}

OSStatus hotKeyHandler(EventHandlerCallRef, EventRef event, void *userData) {
  EventHotKeyID hotKeyId{};
  if (GetEventParameter(event, kEventParamDirectObject, typeEventHotKeyID, nullptr, sizeof(hotKeyId), nullptr,
                        &hotKeyId) != noErr) {
    return eventNotHandledErr;
  }
  if (hotKeyId.signature != HOT_KEY_SIGNATURE) { return eventNotHandledErr; }

  const auto timestamp = static_cast<quint64>(GetEventTime(event) * 1000.0);
  static_cast<MacOSGlobalShortcutBackend *>(userData)->handleHotKey(hotKeyId.id, timestamp);
  return noErr;
}

void layoutChangedCallback(CFNotificationCenterRef, void *observer, CFNotificationName, const void *,
                           CFDictionaryRef) {
  static_cast<MacOSGlobalShortcutBackend *>(observer)->refreshLayout();
}

} // namespace

MacOSGlobalShortcutBackend::MacOSGlobalShortcutBackend() { m_bindings.reserve(8); }

MacOSGlobalShortcutBackend::~MacOSGlobalShortcutBackend() {
  CFNotificationCenterRemoveObserver(CFNotificationCenterGetDistributedCenter(), this,
                                     kTISNotifySelectedKeyboardInputSourceChanged, nullptr);

  teardownCarbon();

  if (m_layoutData) {
    CFRelease(m_layoutData);
    m_layoutData = nullptr;
  }
  if (m_qwertyLayoutData) {
    CFRelease(m_qwertyLayoutData);
    m_qwertyLayoutData = nullptr;
  }
}

bool MacOSGlobalShortcutBackend::start() {
  if (m_started) { return true; }

  refreshLayout();
  CFNotificationCenterAddObserver(CFNotificationCenterGetDistributedCenter(), this, &layoutChangedCallback,
                                  kTISNotifySelectedKeyboardInputSourceChanged, nullptr,
                                  CFNotificationSuspensionBehaviorDeliverImmediately);

  startCarbonHandler();

  m_started = true;
  emit ready();
  return true;
}

void MacOSGlobalShortcutBackend::refreshLayout() {
  CFDataRef data = Keyboard::macos::copyCurrentLayoutData();

  if (m_layoutData) { CFRelease(m_layoutData); }
  m_layoutData = data;
  m_kbdType = LMGetKbdType();

  if (!m_qwertyLayoutData) { m_qwertyLayoutData = Keyboard::macos::copyQwertyLayoutData(); }

  if (m_hotKeyHandler) {
    for (auto &binding : m_bindings) {
      if (binding.character.isEmpty()) { continue; }
      unregisterCarbonHotKey(binding);
      if (auto registered = registerCarbonHotKey(binding); !registered) {
        qWarning() << "MacOSGlobalShortcutBackend: failed to rebind" << binding.id
                   << "after layout change:" << registered.error();
      }
    }
  }
}

bool MacOSGlobalShortcutBackend::startCarbonHandler() {
  if (m_hotKeyHandler) { return true; }

  const EventTypeSpec spec{.eventClass = kEventClassKeyboard, .eventKind = kEventHotKeyPressed};
  EventHandlerRef handler = nullptr;
  if (InstallApplicationEventHandler(&hotKeyHandler, 1, &spec, this, &handler) != noErr) {
    qWarning() << "MacOSGlobalShortcutBackend: failed to install Carbon hot key handler";
    return false;
  }

  m_hotKeyHandler = handler;
  return true;
}

void MacOSGlobalShortcutBackend::teardownCarbon() {
  for (auto &binding : m_bindings) {
    unregisterCarbonHotKey(binding);
  }

  if (m_hotKeyHandler) {
    RemoveEventHandler(static_cast<EventHandlerRef>(m_hotKeyHandler));
    m_hotKeyHandler = nullptr;
  }
}

std::optional<uint32_t> MacOSGlobalShortcutBackend::resolveCarbonKeycode(const Binding &binding) const {
  if (binding.keycode) { return binding.keycode; }

  const auto active = static_cast<CFDataRef>(m_layoutData);
  const auto qwerty = static_cast<CFDataRef>(m_qwertyLayoutData);
  const bool shift = (binding.flags & kCGEventFlagMaskShift) != 0;

  const auto findIn = [&](CFDataRef layout, bool shifted) -> std::optional<uint32_t> {
    if (!layout) { return std::nullopt; }
    for (uint32_t keycode = 0; keycode < MAX_LAYOUT_KEYCODE; ++keycode) {
      const QString character =
          Keyboard::macos::translateKeycode(layout, static_cast<uint16_t>(keycode), shifted, m_kbdType);
      if (!character.isEmpty() && character == binding.character) { return keycode; }
    }
    return std::nullopt;
  };

  if (const auto keycode = findIn(active, false)) { return keycode; }
  if (shift) {
    if (const auto keycode = findIn(active, true)) { return keycode; }
  }
  if (const auto keycode = findIn(qwerty, false)) { return keycode; }
  if (shift) {
    if (const auto keycode = findIn(qwerty, true)) { return keycode; }
  }
  return std::nullopt;
}

std::expected<void, QString> MacOSGlobalShortcutBackend::registerCarbonHotKey(Binding &binding) {
  const auto keycode = resolveCarbonKeycode(binding);
  if (!keycode) { return std::unexpected(tr("unsupported or invalid trigger")); }

  const uint32_t carbonId = binding.carbonId != 0 ? binding.carbonId : m_nextCarbonId++;
  const EventHotKeyID hotKeyId{.signature = HOT_KEY_SIGNATURE, .id = carbonId};
  EventHotKeyRef ref = nullptr;
  const OSStatus status = RegisterEventHotKey(*keycode, carbonModifiersFromCGFlags(binding.flags), hotKeyId,
                                              GetApplicationEventTarget(), 0, &ref);

  if (status != noErr || !ref) { return std::unexpected(tr("failed to register hot key (%1)").arg(status)); }

  binding.hotKeyRef = ref;
  binding.carbonId = carbonId;
  return {};
}

void MacOSGlobalShortcutBackend::unregisterCarbonHotKey(Binding &binding) {
  if (!binding.hotKeyRef) { return; }
  UnregisterEventHotKey(static_cast<EventHotKeyRef>(binding.hotKeyRef));
  binding.hotKeyRef = nullptr;
}

void MacOSGlobalShortcutBackend::handleHotKey(uint32_t carbonId, quint64 timestamp) {
  const auto it = std::ranges::find_if(m_bindings, [&](const Binding &binding) {
    return binding.carbonId == carbonId && binding.hotKeyRef != nullptr;
  });
  if (it == m_bindings.end()) { return; }

  emit shortcutActivated(it->id, timestamp);
}

std::expected<void, QString> MacOSGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  Binding binding{.id = request.id, .flags = cgFlagsForQtModifiers(request.trigger.mods())};

  if (const auto keycode = Keyboard::macos::keycodeForNamedKey(request.trigger.key())) {
    binding.keycode = *keycode;
  } else if (const auto character = Keyboard::printableCharForKey(request.trigger.key())) {
    binding.character = QString(character->toLower());
  } else {
    return std::unexpected(tr("unsupported or invalid trigger"));
  }

  if (auto registered = registerCarbonHotKey(binding); !registered) { return registered; }
  m_bindings.emplace_back(std::move(binding));
  return {};
}

void MacOSGlobalShortcutBackend::unbindShortcut(const QString &id) {
  for (auto &binding : m_bindings) {
    if (binding.id == id) { unregisterCarbonHotKey(binding); }
  }
  std::erase_if(m_bindings, [&](const Binding &binding) { return binding.id == id; });
}

void MacOSGlobalShortcutBackend::unbindAll() {
  for (auto &binding : m_bindings) {
    unregisterCarbonHotKey(binding);
  }
  m_bindings.clear();
}
