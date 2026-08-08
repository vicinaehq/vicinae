#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"
#include "keyboard/keyboard-macos.hpp"

#include <algorithm>
#include <array>
#include <QChar>
#include <QDebug>

namespace {

constexpr uint64_t MODIFIER_MASK =
    kCGEventFlagMaskCommand | kCGEventFlagMaskControl | kCGEventFlagMaskAlternate | kCGEventFlagMaskShift;
constexpr int PERMISSION_POLL_INTERVAL_MS = 2000;
constexpr OSType HOT_KEY_SIGNATURE = 'vici';
constexpr uint32_t MAX_LAYOUT_KEYCODE = 128;

std::optional<uint32_t> staticKeycodeForQtKey(Qt::Key key) {
  switch (key) {
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
  case Qt::Key_F13:
    return kVK_F13;
  case Qt::Key_F14:
    return kVK_F14;
  case Qt::Key_F15:
    return kVK_F15;
  case Qt::Key_F16:
    return kVK_F16;
  case Qt::Key_F17:
    return kVK_F17;
  case Qt::Key_F18:
    return kVK_F18;
  case Qt::Key_F19:
    return kVK_F19;
  case Qt::Key_F20:
    return kVK_F20;
  default:
    return std::nullopt;
  }
}

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

CGEventRef tapCallback(CGEventTapProxy, CGEventType type, CGEventRef event, void *refcon) {
  auto *self = static_cast<MacOSGlobalShortcutBackend *>(refcon);

  if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
    self->reenableTap();
    return event;
  }
  if (type != kCGEventKeyDown) { return event; }

  const auto keycode = static_cast<uint32_t>(CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode));
  const bool autorepeat = CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat) != 0;
  const quint64 timestamp = CGEventGetTimestamp(event) / 1'000'000; // ns -> ms

  if (self->handleKeyDown(keycode, CGEventGetFlags(event), autorepeat, timestamp)) { return nullptr; }
  return event;
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
  stopTapThread();

  std::lock_guard lock(m_mutex);
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

  m_trusted = AXIsProcessTrusted();
  if (m_trusted) {
    startTapThread();
  } else {
    startCarbonHandler();
  }

  m_permissionTimer.setInterval(PERMISSION_POLL_INTERVAL_MS);
  connect(&m_permissionTimer, &QTimer::timeout, this, &MacOSGlobalShortcutBackend::syncPermissionState);
  m_permissionTimer.start();

  m_started = true;
  emit ready();
  return true;
}

void MacOSGlobalShortcutBackend::syncPermissionState() {
  const bool trusted = AXIsProcessTrusted();

  if (trusted == m_trusted) {
    if (trusted) { ensureTapRunning(); }
    return;
  }

  m_trusted = trusted;
  if (trusted) {
    unbindAll();
    teardownCarbon();
    startTapThread();
  } else {
    stopTapThread();
    unbindAll();
    startCarbonHandler();
  }

  emit ready();
}

// The tap gets its own run loop thread so a busy Qt main loop can't get it disabled by timeout.
void MacOSGlobalShortcutBackend::startTapThread() {
  if (m_thread.joinable()) { m_thread.join(); }
  m_tapThreadDone = false;
  m_thread = std::thread([this]() {
    runTap();
    m_tapThreadDone = true;
  });
}

void MacOSGlobalShortcutBackend::stopTapThread() {
  if (void *loop = m_runLoop.load()) { CFRunLoopStop(static_cast<CFRunLoopRef>(loop)); }
  if (m_thread.joinable()) { m_thread.join(); }
}

void MacOSGlobalShortcutBackend::ensureTapRunning() {
  if (!m_tapThreadDone) return;
  startTapThread();
}

void MacOSGlobalShortcutBackend::runTap() {
  const CGEventMask mask = CGEventMaskBit(kCGEventKeyDown);

  CFMachPortRef tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                       mask, tapCallback, this);

  if (!tap) {
    qWarning()
        << "MacOSGlobalShortcutBackend: failed to create event tap (accessibility permission missing?)";
    return;
  }

  CFRunLoopSourceRef source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, tap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
  CGEventTapEnable(tap, true);

  m_tap = tap;
  m_runLoop = CFRunLoopGetCurrent();

  CFRunLoopRun();

  m_runLoop = nullptr;
  CFRunLoopRemoveSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
  CFRelease(source);
  CFRelease(tap);
  m_tap = nullptr;
}

void MacOSGlobalShortcutBackend::reenableTap() {
  if (m_tap) { CGEventTapEnable(static_cast<CFMachPortRef>(m_tap), true); }
}

void MacOSGlobalShortcutBackend::refreshLayout() {
  CFDataRef data = Keyboard::macos::copyCurrentLayoutData();

  std::lock_guard lock(m_mutex);
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
  {
    std::lock_guard lock(m_mutex);
    for (auto &binding : m_bindings) {
      unregisterCarbonHotKey(binding);
    }
  }

  if (m_hotKeyHandler) {
    RemoveEventHandler(static_cast<EventHandlerRef>(m_hotKeyHandler));
    m_hotKeyHandler = nullptr;
  }
}

// requires m_mutex to be held
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

// requires m_mutex to be held
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
  QString id;

  {
    std::lock_guard lock(m_mutex);
    const auto it = std::ranges::find_if(m_bindings, [&](const Binding &binding) {
      return binding.carbonId == carbonId && binding.hotKeyRef != nullptr;
    });
    if (it == m_bindings.end()) { return; }
    id = it->id;
  }

  emit shortcutActivated(id, timestamp);
}

std::expected<void, QString> MacOSGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  Binding binding{.id = request.id, .flags = cgFlagsForQtModifiers(request.trigger.mods())};

  if (const auto keycode = staticKeycodeForQtKey(request.trigger.key())) {
    binding.keycode = *keycode;
  } else if (const auto character = Keyboard::printableCharForKey(request.trigger.key())) {
    binding.character = QString(character->toLower());
  } else {
    return std::unexpected(tr("unsupported or invalid trigger"));
  }

  std::lock_guard lock(m_mutex);
  if (m_hotKeyHandler) {
    if (auto registered = registerCarbonHotKey(binding); !registered) { return registered; }
  }
  m_bindings.emplace_back(std::move(binding));
  return {};
}

void MacOSGlobalShortcutBackend::unbindShortcut(const QString &id) {
  std::lock_guard lock(m_mutex);
  for (auto &binding : m_bindings) {
    if (binding.id == id) { unregisterCarbonHotKey(binding); }
  }
  std::erase_if(m_bindings, [&](const Binding &binding) { return binding.id == id; });
}

void MacOSGlobalShortcutBackend::unbindAll() {
  std::lock_guard lock(m_mutex);
  for (auto &binding : m_bindings) {
    unregisterCarbonHotKey(binding);
  }
  m_bindings.clear();
}

bool MacOSGlobalShortcutBackend::handleKeyDown(uint32_t keycode, uint64_t rawFlags, bool autorepeat,
                                               quint64 timestamp) {
  const uint64_t flags = rawFlags & MODIFIER_MASK;

  std::lock_guard lock(m_mutex);
  if (m_bindings.empty()) { return false; }

  const Binding *match = nullptr;
  std::array<QString, 4> candidates;
  bool translated = false;

  for (const auto &binding : m_bindings) {
    if (binding.flags != flags) { continue; }

    if (binding.keycode) {
      if (*binding.keycode == keycode) {
        match = &binding;
        break;
      }
      continue;
    }

    if (!translated) {
      translated = true;
      const bool shift = (flags & kCGEventFlagMaskShift) != 0;
      const auto active = static_cast<CFDataRef>(m_layoutData);
      const auto qwerty = static_cast<CFDataRef>(m_qwertyLayoutData);
      const auto code = static_cast<uint16_t>(keycode);
      candidates[0] = Keyboard::macos::translateKeycode(active, code, false, m_kbdType);
      // shortcuts can hold a shifted char (e.g ':' recorded on layouts where it is Shift+';')
      if (shift) { candidates[1] = Keyboard::macos::translateKeycode(active, code, true, m_kbdType); }
      // QWERTY fallback keeps Latin shortcuts firing while a non-Latin layout is active
      candidates[2] = Keyboard::macos::translateKeycode(qwerty, code, false, m_kbdType);
      if (shift) { candidates[3] = Keyboard::macos::translateKeycode(qwerty, code, true, m_kbdType); }
    }

    const bool matches = std::ranges::any_of(candidates, [&](const QString &candidate) {
      return !candidate.isEmpty() && binding.character == candidate;
    });

    if (matches) {
      match = &binding;
      break;
    }
  }

  if (!match) { return false; }

  if (!autorepeat) {
    const QString id = match->id;
    QMetaObject::invokeMethod(
        this, [this, id, timestamp]() { emit shortcutActivated(id, timestamp); }, Qt::QueuedConnection);
  }
  return true;
}
