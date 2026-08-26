#include "virtual-desktops.hpp"
#include <QCoreApplication>
#include <QWinEventNotifier>
#include <QDebug>

namespace {

constexpr const wchar_t *VD_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";

// documented coclass for IVirtualDesktopManager; not declared by all SDK header configurations
constexpr CLSID VIRTUAL_DESKTOP_MANAGER_CLSID = {
    0xaa509086, 0x5ca9, 0x4c25, {0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a}};

QString guidToString(const GUID &guid) {
  wchar_t buf[64] = {};
  StringFromGUID2(guid, buf, static_cast<int>(std::size(buf)));
  return QString::fromWCharArray(buf);
}

std::optional<GUID> readRegistryGuid(const wchar_t *key, const wchar_t *value) {
  GUID guid{};
  DWORD size = sizeof(guid);
  if (RegGetValueW(HKEY_CURRENT_USER, key, value, RRF_RT_REG_BINARY, nullptr, &guid, &size) !=
          ERROR_SUCCESS ||
      size != sizeof(guid)) {
    return std::nullopt;
  }
  return guid;
}

std::vector<GUID> readDesktopIds() {
  DWORD size = 0;
  if (RegGetValueW(HKEY_CURRENT_USER, VD_KEY, L"VirtualDesktopIDs", RRF_RT_REG_BINARY, nullptr, nullptr,
                   &size) != ERROR_SUCCESS ||
      size == 0 || size % sizeof(GUID) != 0) {
    return {};
  }

  std::vector<GUID> ids(size / sizeof(GUID));
  if (RegGetValueW(HKEY_CURRENT_USER, VD_KEY, L"VirtualDesktopIDs", RRF_RT_REG_BINARY, nullptr, ids.data(),
                   &size) != ERROR_SUCCESS) {
    return {};
  }
  return ids;
}

std::optional<GUID> readCurrentDesktopGuid() {
  if (auto guid = readRegistryGuid(VD_KEY, L"CurrentVirtualDesktop")) return guid;

  // Windows 10 keeps the current desktop under a per-session key instead
  DWORD session = 0;
  if (!ProcessIdToSessionId(GetCurrentProcessId(), &session)) return std::nullopt;
  const std::wstring sessionKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\" +
                                  std::to_wstring(session) + L"\\VirtualDesktops";
  return readRegistryGuid(sessionKey.c_str(), L"CurrentVirtualDesktop");
}

QString readDesktopName(const QString &id, size_t position) {
  const std::wstring key = std::wstring(VD_KEY) + L"\\Desktops\\" + id.toStdWString();
  wchar_t name[256] = {};
  DWORD size = sizeof(name);
  if (RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Name", RRF_RT_REG_SZ, nullptr, name, &size) ==
          ERROR_SUCCESS &&
      name[0]) {
    return QString::fromWCharArray(name);
  }
  return QCoreApplication::translate("virtual-desktops", "Desktop %1").arg(position + 1);
}

} // namespace

namespace Win {

VirtualDesktops::VirtualDesktops(QObject *parent) : QObject(parent) {
  if (HRESULT hr = CoCreateInstance(VIRTUAL_DESKTOP_MANAGER_CLSID, nullptr, CLSCTX_ALL,
                                    IID_PPV_ARGS(m_manager.GetAddressOf()));
      FAILED(hr)) {
    qWarning() << "VirtualDesktops: failed to create IVirtualDesktopManager" << Qt::hex << hr;
  }

  reload();

  if (RegOpenKeyExW(HKEY_CURRENT_USER, VD_KEY, 0, KEY_NOTIFY, &m_watchKey) == ERROR_SUCCESS) {
    m_watchEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    m_notifier = new QWinEventNotifier(m_watchEvent, this);
    connect(m_notifier, &QWinEventNotifier::activated, this, [this]() {
      reload();
      armWatcher();
      emit changed();
    });
    armWatcher();
  }
}

VirtualDesktops::~VirtualDesktops() {
  if (m_watchKey) RegCloseKey(m_watchKey);
  if (m_watchEvent) CloseHandle(m_watchEvent);
}

void VirtualDesktops::armWatcher() {
  RegNotifyChangeKeyValue(m_watchKey, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, m_watchEvent,
                          TRUE);
}

void VirtualDesktops::reload() {
  std::vector<Desktop> desktops;
  const auto ids = readDesktopIds();
  desktops.reserve(ids.size());

  for (size_t i = 0; i < ids.size(); ++i) {
    const QString id = guidToString(ids[i]);
    desktops.emplace_back(Desktop{.id = id, .name = readDesktopName(id, i)});
  }

  m_desktops = std::move(desktops);
}

QString VirtualDesktops::activeDesktopId() const {
  if (auto guid = readCurrentDesktopGuid()) return guidToString(*guid);

  // the registry value only appears once the user has switched desktops at least once
  if (HWND foreground = GetForegroundWindow()) {
    if (auto id = windowDesktopId(foreground)) return *id;
  }
  if (!m_desktops.empty()) return m_desktops.front().id;
  return {};
}

std::optional<QString> VirtualDesktops::windowDesktopId(HWND hwnd) const {
  if (!m_manager) return std::nullopt;

  GUID guid{};
  if (FAILED(m_manager->GetWindowDesktopId(hwnd, &guid)) || guid == GUID_NULL) return std::nullopt;
  return guidToString(guid);
}

bool VirtualDesktops::isWindowOnCurrentDesktop(HWND hwnd) const {
  if (!m_manager) return true;

  BOOL onCurrent = TRUE;
  if (FAILED(m_manager->IsWindowOnCurrentVirtualDesktop(hwnd, &onCurrent))) return true;
  return onCurrent;
}

bool VirtualDesktops::moveOwnWindowToDesktop(HWND hwnd, const QString &desktopId) const {
  if (!m_manager) return false;

  GUID guid{};
  if (FAILED(CLSIDFromString(desktopId.toStdWString().c_str(), &guid))) return false;
  return SUCCEEDED(m_manager->MoveWindowToDesktop(hwnd, guid));
}

} // namespace Win
