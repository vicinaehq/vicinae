#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"
#include "keyboard/keyboard-macos.hpp"

#include <algorithm>
#include <array>
#include <QChar>
#include <QDebug>

namespace {

constexpr uint64_t MODIFIER_MASK =
    kCGEventFlagMaskCommand | kCGEventFlagMaskControl | kCGEventFlagMaskAlternate | kCGEventFlagMaskShift;
constexpr int PERMISSION_RETRY_INTERVAL_MS = 2000;

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

  if (void *loop = m_runLoop.load()) { CFRunLoopStop(static_cast<CFRunLoopRef>(loop)); }
  if (m_thread.joinable()) { m_thread.join(); }

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
  startTapThread();

  if (!AXIsProcessTrusted()) {
    m_permissionRetryTimer.setInterval(PERMISSION_RETRY_INTERVAL_MS);
    connect(&m_permissionRetryTimer, &QTimer::timeout, this, [this]() {
      if (!AXIsProcessTrusted()) return;
      m_permissionRetryTimer.stop();
      ensureTapRunning();
    });
    m_permissionRetryTimer.start();
  }

  m_started = true;
  emit ready();
  return true;
}

// The tap gets its own run loop thread so a busy Qt main loop can't get it disabled by timeout.
void MacOSGlobalShortcutBackend::startTapThread() {
  m_tapThreadDone = false;
  m_thread = std::thread([this]() {
    runTap();
    m_tapThreadDone = true;
  });
}

void MacOSGlobalShortcutBackend::ensureTapRunning() {
  if (!m_tapThreadDone) return;
  if (m_thread.joinable()) m_thread.join();
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
}

std::expected<void, QString> MacOSGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  Binding binding{.id = request.id, .flags = cgFlagsForQtModifiers(request.trigger.mods())};

  if (const auto keycode = staticKeycodeForQtKey(request.trigger.key())) {
    binding.keycode = *keycode;
  } else if (const auto character = Keyboard::printableCharForKey(request.trigger.key())) {
    binding.character = QString(character->toLower());
  } else {
    return std::unexpected(QStringLiteral("unsupported or invalid trigger"));
  }

  std::lock_guard lock(m_mutex);
  m_bindings.emplace_back(std::move(binding));
  return {};
}

void MacOSGlobalShortcutBackend::unbindShortcut(const QString &id) {
  std::lock_guard lock(m_mutex);
  std::erase_if(m_bindings, [&](const Binding &binding) { return binding.id == id; });
}

void MacOSGlobalShortcutBackend::unbindAll() {
  std::lock_guard lock(m_mutex);
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
