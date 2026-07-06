#include "macos-snippet-server.hpp"
#include <ApplicationServices/ApplicationServices.h>
#include <QString>
#include <QTimer>
#include <cctype>
#include <ranges>
#include <unistd.h>

namespace {

constexpr size_t MAX_BUFFER_SIZE = 32;

// Virtual keycodes (Carbon HIToolbox).
constexpr CGKeyCode VK_DELETE = 0x33;
constexpr CGKeyCode VK_LEFT_ARROW = 0x7B;

// Tag stamped on synthetic events so the tap can skip its own injection.
constexpr int64_t VICINAE_EVENT_TAG = 0x7669636e; // 'vicn'

constexpr CGEventFlags BLOCKING_MODS =
    kCGEventFlagMaskCommand | kCGEventFlagMaskControl | kCGEventFlagMaskAlternate;

bool isWordSeparator(char c) {
  return std::isspace(static_cast<unsigned char>(c)) || std::ispunct(static_cast<unsigned char>(c));
}

void postKey(CGKeyCode key, bool down, CGEventFlags flags, int delayUs) {
  CGEventRef event = CGEventCreateKeyboardEvent(nullptr, key, down);
  if (!event) { return; }
  CGEventSetFlags(event, flags);
  CGEventSetIntegerValueField(event, kCGEventSourceUserData, VICINAE_EVENT_TAG);
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);
  if (delayUs > 0) { usleep(delayUs); }
}

void injectText(std::string_view text, int delayUs) {
  const QString qtext = QString::fromUtf8(text.data(), static_cast<int>(text.size()));
  const auto *units = reinterpret_cast<const UniChar *>(qtext.utf16());
  const int total = static_cast<int>(qtext.size());
  constexpr int CHUNK = 20;

  for (int off = 0; off < total;) {
    int count = total - off < CHUNK ? total - off : CHUNK;
    // never end a chunk on a high surrogate whose low half follows, or we split the code point in two
    if (off + count < total && QChar::isHighSurrogate(units[off + count - 1])) { --count; }

    for (const bool down : {true, false}) {
      CGEventRef event = CGEventCreateKeyboardEvent(nullptr, 0, down);
      if (!event) { continue; }
      CGEventKeyboardSetUnicodeString(event, static_cast<UniCharCount>(count), units + off);
      CGEventSetIntegerValueField(event, kCGEventSourceUserData, VICINAE_EVENT_TAG);
      CGEventPost(kCGHIDEventTap, event);
      CFRelease(event);
    }
    if (delayUs > 0) { usleep(delayUs); }
    off += count;
  }
}

CGEventRef tapCallback(CGEventTapProxy, CGEventType type, CGEventRef event, void *refcon) {
  auto *self = static_cast<MacosSnippetServer *>(refcon);

  if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
    self->reenableTap();
    return event;
  }

  // skip events we injected ourselves to avoid feedback loops
  if (CGEventGetIntegerValueField(event, kCGEventSourceUserData) == VICINAE_EVENT_TAG) { return event; }

  const bool blocking = (CGEventGetFlags(event) & BLOCKING_MODS) != 0;
  const auto keycode = static_cast<int>(CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode));

  UniChar chars[8];
  UniCharCount len = 0;
  CGEventKeyboardGetUnicodeString(event, std::size(chars), &len, chars);
  const std::string utf8 =
      QString::fromUtf16(reinterpret_cast<const char16_t *>(chars), static_cast<int>(len)).toStdString();

  self->onKey(keycode, utf8, blocking);
  return event;
}

} // namespace

MacosSnippetServer::MacosSnippetServer() {
  startTapThread();

  if (!AXIsProcessTrusted()) {
    m_permissionRetryTimer.setInterval(2000);
    connect(&m_permissionRetryTimer, &QTimer::timeout, this, [this]() {
      if (!AXIsProcessTrusted()) return;
      m_permissionRetryTimer.stop();
      ensureTapRunning();
    });
    m_permissionRetryTimer.start();
  }

  QTimer::singleShot(0, this, [this]() { emit ready(); });
}

// we run a separate event loop for the tap instead of using the QT event loop directly
// because macOS can disable the tap if our main thread times out, which would force us to
// handle retrying and all that annoying stuff.
void MacosSnippetServer::startTapThread() {
  m_tapThreadDone = false;
  m_thread = std::thread([this]() {
    runTap();
    m_tapThreadDone = true;
  });
}

void MacosSnippetServer::ensureTapRunning() {
  if (!m_tapThreadDone) return;
  if (m_thread.joinable()) m_thread.join();
  startTapThread();
}

MacosSnippetServer::~MacosSnippetServer() {
  if (void *loop = m_runLoop.load()) { CFRunLoopStop(static_cast<CFRunLoopRef>(loop)); }
  if (m_thread.joinable()) { m_thread.join(); }
}

void MacosSnippetServer::runTap() {
  const CGEventMask mask = CGEventMaskBit(kCGEventKeyDown);

  CFMachPortRef tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionListenOnly,
                                       mask, tapCallback, this);

  if (!tap) {
    qWarning() << "MacosSnippetServer: failed to create event tap (accessibility permission missing?)";
    return;
  }

  CFRunLoopSourceRef source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, tap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
  CGEventTapEnable(tap, true);

  m_tap = tap;
  m_source = source;
  m_running = true;
  m_runLoop = CFRunLoopGetCurrent();

  CFRunLoopRun();

  m_running = false;
  m_runLoop = nullptr;
  CFRunLoopRemoveSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
  CFRelease(source);
  CFRelease(tap);
  m_tap = nullptr;
  m_source = nullptr;
}

void MacosSnippetServer::registerSnippet(snippet_gen::CreateSnippetRequest payload) {
  std::lock_guard lock(m_mutex);
  std::erase_if(m_snippets, [&](auto &&s) { return s.trigger == payload.trigger; });
  m_snippets.emplace_back(Snippet{.trigger = payload.trigger, .mode = payload.mode});
  std::ranges::sort(m_snippets, [](auto &&a, auto &&b) { return a.trigger.size() > b.trigger.size(); });
}

void MacosSnippetServer::unregisterSnippet(std::string_view keyword) {
  std::lock_guard lock(m_mutex);
  std::erase_if(m_snippets, [&](auto &&s) { return s.trigger == keyword; });
}

void MacosSnippetServer::setKeymap(snippet_gen::LayoutInfo) {
  // TODO: the tap is automatically using the right keymap, this is mostly a Linux problem
  // Maybe we want to remove this from the abstract interface
}

void MacosSnippetServer::resetContext() {
  std::lock_guard lock(m_mutex);
  m_text.clear();
  m_undoTrigger.reset();
}

void MacosSnippetServer::injectExpand(const std::string &text, unsigned charsToDelete, unsigned, bool,
                                      unsigned cursorLeftMoves) {
  const int delay = m_keyDelayUs.load();

  // delete the trigger with backspaces, then type the expansion as unicode key events. typing avoids
  // the clipboard entirely and behaves consistently across terminals and editors (unlike paste +
  // shift-select, which breaks in terminals)
  for (unsigned i = 0; i < charsToDelete; ++i) {
    postKey(VK_DELETE, true, 0, 0);
    postKey(VK_DELETE, false, 0, delay);
  }

  injectText(text, delay);

  for (unsigned i = 0; i < cursorLeftMoves; ++i) {
    postKey(VK_LEFT_ARROW, true, 0, 0);
    postKey(VK_LEFT_ARROW, false, 0, delay);
  }
}

void MacosSnippetServer::injectUndo(unsigned backspaceCount, const std::string &trigger) {
  const int delay = m_keyDelayUs.load();

  for (unsigned i = 0; i < backspaceCount; ++i) {
    postKey(VK_DELETE, true, 0, 0);
    postKey(VK_DELETE, false, 0, delay);
  }

  injectText(trigger, delay);
}

void MacosSnippetServer::setKeyDelay(int us) { m_keyDelayUs = us; }

bool MacosSnippetServer::supportsKeyInjection() const { return AXIsProcessTrusted(); }

bool MacosSnippetServer::isRunning() const { return m_running.load(); }

void MacosSnippetServer::reenableTap() {
  if (m_tap) { CGEventTapEnable(static_cast<CFMachPortRef>(m_tap), true); }
}

void MacosSnippetServer::onKey(int keycode, const std::string &utf8, bool blockingMods) {
  std::lock_guard lock(m_mutex);

  if (m_undoTrigger) {
    if (keycode == VK_DELETE) {
      const std::string trigger = *m_undoTrigger;
      m_undoTrigger.reset();
      QMetaObject::invokeMethod(
          this, [this, trigger]() { emit undoTriggered(trigger); }, Qt::QueuedConnection);
      return;
    }
    m_undoTrigger.reset();
  }

  if (keycode == VK_DELETE) {
    if (!m_text.empty()) { m_text.pop_back(); }
  } else if (!blockingMods && !utf8.empty() && std::isprint(static_cast<unsigned char>(utf8.front()))) {
    m_text.append(utf8);
    if (m_text.size() > MAX_BUFFER_SIZE) { m_text.erase(0, m_text.size() - MAX_BUFFER_SIZE); }
  }

  const bool wordSep = !blockingMods && !utf8.empty() && isWordSeparator(utf8.front());

  for (const auto &snippet : m_snippets) {
    if (snippet.mode == snippet_gen::ExpansionMode::Keydown) {
      if (snippet.trigger.size() > m_text.size()) { continue; }
      if (m_text.ends_with(snippet.trigger)) {
        emitExpansionLocked(snippet);
        break;
      }
    } else if (wordSep) {
      if (snippet.trigger.size() + 1 > m_text.size()) { continue; }
      if (std::string_view(m_text).substr(0, m_text.size() - 1).ends_with(snippet.trigger)) {
        emitExpansionLocked(snippet);
        break;
      }
    }
  }
}

void MacosSnippetServer::emitExpansionLocked(const Snippet &snippet) {
  m_text.clear();

  const std::string trigger = snippet.trigger;
  QMetaObject::invokeMethod(
      this, [this, trigger]() { emit keywordTriggered(trigger); }, Qt::QueuedConnection);
  m_undoTrigger = trigger;
}
