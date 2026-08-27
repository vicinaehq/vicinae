#include <algorithm>
#include <cwchar>
#include <iterator>

#include <QDebug>

#include <windows.h>
#include <shellapi.h>

#include "services/tray/windows/tray-service-windows.hpp"

namespace {
constexpr wchar_t WINDOW_CLASS_NAME[] = L"VicinaeTrayWindow";
constexpr wchar_t ICON_RESOURCE_NAME[] = L"IDI_ICON1";
constexpr UINT TRAY_CALLBACK_MESSAGE = WM_APP + 1;
constexpr UINT TRAY_ICON_ID = 1;

constexpr UINT COMMAND_TOGGLE = 1;
constexpr UINT COMMAND_ABOUT = 2;
constexpr UINT COMMAND_CHECK_FOR_UPDATES = 3;
constexpr UINT COMMAND_SETTINGS = 4;
constexpr UINT COMMAND_QUIT = 5;

constexpr GUID TRAY_ICON_GUID = {
    0x58c36f83, 0x24c6, 0x4ed9, {0x91, 0x7c, 0x6b, 0x1f, 0x89, 0xa4, 0xe6, 0x32}};

void copyString(wchar_t *destination, std::size_t capacity, const QString &source) {
  const auto length = std::min<std::size_t>(source.size(), capacity - 1);
  std::wmemcpy(destination, reinterpret_cast<const wchar_t *>(source.utf16()), length);
  destination[length] = L'\0';
}

void appendMenuItem(HMENU menu, UINT flags, UINT_PTR id, const QString &title) {
  AppendMenuW(menu, flags, id, reinterpret_cast<const wchar_t *>(title.utf16()));
}
} // namespace

struct TrayServiceWindows::Impl {
  explicit Impl(TrayServiceWindows &owner) : owner(owner) {
    instance = GetModuleHandleW(nullptr);
    taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW windowClass{
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = windowProc,
        .hInstance = instance,
        .lpszClassName = WINDOW_CLASS_NAME,
    };

    if (!RegisterClassExW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
      qWarning() << "failed to register tray window class:" << GetLastError();
      return;
    }

    window = CreateWindowExW(0, WINDOW_CLASS_NAME, L"", 0, 0, 0, 0, 0, nullptr, nullptr, instance, this);
    if (!window) {
      qWarning() << "failed to create tray window:" << GetLastError();
      return;
    }

    icon = reinterpret_cast<HICON>(LoadImageW(instance, ICON_RESOURCE_NAME, IMAGE_ICON,
                                              GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                                              LR_DEFAULTCOLOR));
    if (!icon) { qWarning() << "failed to load tray icon:" << GetLastError(); }
  }

  ~Impl() {
    hide();
    if (window) DestroyWindow(window);
    if (icon) DestroyIcon(icon);
  }

  static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto *impl = reinterpret_cast<Impl *>(GetWindowLongPtrW(window, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
      const auto *create = reinterpret_cast<const CREATESTRUCTW *>(lParam);
      impl = static_cast<Impl *>(create->lpCreateParams);
      SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(impl));
    }

    if (!impl) return DefWindowProcW(window, message, wParam, lParam);
    return impl->handleMessage(window, message, wParam, lParam);
  }

  LRESULT handleMessage(HWND messageWindow, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == taskbarCreatedMessage) {
      if (visible) addIcon();
      return 0;
    }

    if (message == TRAY_CALLBACK_MESSAGE) {
      switch (LOWORD(lParam)) {
      case NIN_SELECT:
        emit owner.toggleRequested();
        return 0;
      case WM_CONTEXTMENU:
        showMenu();
        return 0;
      default:
        break;
      }
    }

    return DefWindowProcW(messageWindow, message, wParam, lParam);
  }

  NOTIFYICONDATAW notificationData() const {
    NOTIFYICONDATAW data{
        .cbSize = sizeof(NOTIFYICONDATAW),
        .hWnd = window,
        .uID = TRAY_ICON_ID,
        .uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP,
        .uCallbackMessage = TRAY_CALLBACK_MESSAGE,
        .hIcon = icon,
        .guidItem = TRAY_ICON_GUID,
    };
    copyString(data.szTip, std::size(data.szTip), QStringLiteral("Vicinae"));
    return data;
  }

  void addIcon() {
    if (!window || !icon) return;

    auto data = notificationData();
    if (!Shell_NotifyIconW(NIM_ADD, &data)) {
      qWarning() << "failed to add tray icon:" << GetLastError();
      return;
    }

    data.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &data);
  }

  void show() {
    if (visible) return;
    visible = true;
    addIcon();
  }

  void hide() {
    if (!visible) return;
    visible = false;
    if (!window) return;

    auto data = notificationData();
    Shell_NotifyIconW(NIM_DELETE, &data);
  }

  void showMenu() {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
      qWarning() << "failed to create tray menu:" << GetLastError();
      return;
    }

    appendMenuItem(menu, MF_STRING, COMMAND_TOGGLE, owner.tr("Toggle Vicinae"));
    appendMenuItem(menu, MF_STRING | MF_DISABLED, 0,
                   version.isEmpty() ? QStringLiteral("Vicinae") : QStringLiteral("Vicinae %1").arg(version));
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    appendMenuItem(menu, MF_STRING, COMMAND_ABOUT, owner.tr("About Vicinae"));
    if (checkForUpdatesVisible) {
      appendMenuItem(menu, MF_STRING, COMMAND_CHECK_FOR_UPDATES,
                     availableUpdate.isEmpty() ? owner.tr("Check for Updates…")
                                               : owner.tr("Update Available: %1").arg(availableUpdate));
    }
    appendMenuItem(menu, MF_STRING, COMMAND_SETTINGS, owner.tr("Settings…"));
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    appendMenuItem(menu, MF_STRING, COMMAND_QUIT, owner.tr("Quit Vicinae"));

    POINT cursor{};
    GetCursorPos(&cursor);
    SetForegroundWindow(window);
    const UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, cursor.x,
                                        cursor.y, 0, window, nullptr);
    PostMessageW(window, WM_NULL, 0, 0);
    DestroyMenu(menu);

    switch (command) {
    case COMMAND_TOGGLE:
      emit owner.toggleRequested();
      break;
    case COMMAND_ABOUT:
      emit owner.openSettingsRequested(QStringLiteral("about"));
      break;
    case COMMAND_CHECK_FOR_UPDATES:
      emit owner.checkForUpdatesRequested();
      break;
    case COMMAND_SETTINGS:
      emit owner.openSettingsRequested(QString());
      break;
    case COMMAND_QUIT:
      emit owner.quitRequested();
      break;
    default:
      break;
    }
  }

  TrayServiceWindows &owner;
  HINSTANCE instance = nullptr;
  HWND window = nullptr;
  HICON icon = nullptr;
  UINT taskbarCreatedMessage = 0;
  QString version;
  QString availableUpdate;
  bool checkForUpdatesVisible = false;
  bool visible = false;
};

TrayServiceWindows::TrayServiceWindows(QObject *parent)
    : TrayService(parent), m_impl(std::make_unique<Impl>(*this)) {}

TrayServiceWindows::~TrayServiceWindows() = default;

void TrayServiceWindows::setVersion(const QString &version) { m_impl->version = version; }

void TrayServiceWindows::setCheckForUpdatesVisible(bool visible) { m_impl->checkForUpdatesVisible = visible; }

void TrayServiceWindows::setAvailableUpdate(const QString &tag) { m_impl->availableUpdate = tag; }

void TrayServiceWindows::show() { m_impl->show(); }

void TrayServiceWindows::hide() { m_impl->hide(); }
