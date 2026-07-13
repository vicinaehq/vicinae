#include "services/global-shortcuts/windows-global-shortcut-backend.hpp"
#include <QGuiApplication>
#include <windows.h>

namespace {

std::optional<UINT> vkForQtKey(Qt::Key key) {
  if (key >= Qt::Key_A && key <= Qt::Key_Z) { return 'A' + (key - Qt::Key_A); }
  if (key >= Qt::Key_0 && key <= Qt::Key_9) { return '0' + (key - Qt::Key_0); }
  if (key >= Qt::Key_F1 && key <= Qt::Key_F12) { return VK_F1 + (key - Qt::Key_F1); }

  switch (key) {
  case Qt::Key_Space:
    return VK_SPACE;
  case Qt::Key_Return:
  case Qt::Key_Enter:
    return VK_RETURN;
  case Qt::Key_Escape:
    return VK_ESCAPE;
  case Qt::Key_Tab:
    return VK_TAB;
  case Qt::Key_Backspace:
    return VK_BACK;
  case Qt::Key_Delete:
    return VK_DELETE;
  case Qt::Key_Home:
    return VK_HOME;
  case Qt::Key_End:
    return VK_END;
  case Qt::Key_PageUp:
    return VK_PRIOR;
  case Qt::Key_PageDown:
    return VK_NEXT;
  case Qt::Key_Left:
    return VK_LEFT;
  case Qt::Key_Right:
    return VK_RIGHT;
  case Qt::Key_Up:
    return VK_UP;
  case Qt::Key_Down:
    return VK_DOWN;
  case Qt::Key_Minus:
    return VK_OEM_MINUS;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
    return VK_OEM_PLUS;
  case Qt::Key_BracketLeft:
    return VK_OEM_4;
  case Qt::Key_BracketRight:
    return VK_OEM_6;
  case Qt::Key_Backslash:
    return VK_OEM_5;
  case Qt::Key_Semicolon:
    return VK_OEM_1;
  case Qt::Key_Apostrophe:
    return VK_OEM_7;
  case Qt::Key_Comma:
    return VK_OEM_COMMA;
  case Qt::Key_Period:
    return VK_OEM_PERIOD;
  case Qt::Key_Slash:
    return VK_OEM_2;
  case Qt::Key_QuoteLeft:
    return VK_OEM_3;
  default:
    return std::nullopt;
  }
}

UINT winModifiers(Qt::KeyboardModifiers mods) {
  UINT win = MOD_NOREPEAT;
  if (mods.testFlag(Qt::ControlModifier)) { win |= MOD_CONTROL; }
  if (mods.testFlag(Qt::AltModifier)) { win |= MOD_ALT; }
  if (mods.testFlag(Qt::ShiftModifier)) { win |= MOD_SHIFT; }
  if (mods.testFlag(Qt::MetaModifier)) { win |= MOD_WIN; }
  return win;
}

} // namespace

WindowsGlobalShortcutBackend::~WindowsGlobalShortcutBackend() {
  unbindAll();
  if (m_started) { qGuiApp->removeNativeEventFilter(this); }
}

bool WindowsGlobalShortcutBackend::start() {
  if (m_started) { return true; }

  qGuiApp->installNativeEventFilter(this);
  m_started = true;
  emit ready();
  return true;
}

std::expected<void, QString> WindowsGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  const auto vk = vkForQtKey(request.trigger.key());
  if (!vk) { return std::unexpected(QStringLiteral("unsupported or invalid trigger")); }

  const int hotkeyId = m_nextHotkeyId++;
  if (!RegisterHotKey(nullptr, hotkeyId, winModifiers(request.trigger.mods()), *vk)) {
    const DWORD error = GetLastError();
    if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {
      return std::unexpected(QStringLiteral("already registered by another application"));
    }
    return std::unexpected(QStringLiteral("RegisterHotKey failed (%1)").arg(error));
  }

  m_hotkeyIds.emplace(request.id, hotkeyId);
  m_idByHotkeyId.emplace(hotkeyId, request.id);
  return {};
}

void WindowsGlobalShortcutBackend::unbindShortcut(const QString &id) {
  const auto it = m_hotkeyIds.find(id);
  if (it == m_hotkeyIds.end()) { return; }

  UnregisterHotKey(nullptr, it->second);
  m_idByHotkeyId.erase(it->second);
  m_hotkeyIds.erase(it);
}

void WindowsGlobalShortcutBackend::unbindAll() {
  for (const auto &[id, hotkeyId] : m_hotkeyIds) {
    UnregisterHotKey(nullptr, hotkeyId);
  }
  m_hotkeyIds.clear();
  m_idByHotkeyId.clear();
}

bool WindowsGlobalShortcutBackend::nativeEventFilter(const QByteArray &eventType, void *message,
                                                     qintptr *result) {
  Q_UNUSED(eventType)
  Q_UNUSED(result)

  const MSG *msg = static_cast<MSG *>(message);
  if (msg->message != WM_HOTKEY) { return false; }

  if (const auto it = m_idByHotkeyId.find(static_cast<int>(msg->wParam)); it != m_idByHotkeyId.end()) {
    emit shortcutActivated(it->second, GetTickCount64());
  }

  return false;
}
