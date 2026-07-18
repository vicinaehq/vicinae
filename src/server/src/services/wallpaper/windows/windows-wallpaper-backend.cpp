#include "windows-wallpaper-backend.hpp"
#include "utils/scoped-com.hpp"
#include <QtConcurrent>
#include <filesystem>
#include <optional>
#include <shobjidl_core.h>
#include <string>
#include <system_error>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace {

DESKTOP_WALLPAPER_POSITION positionForFit(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Cover:
    return DWPOS_FILL;
  case WallpaperFit::Contain:
    return DWPOS_FIT;
  case WallpaperFit::Stretch:
    return DWPOS_STRETCH;
  case WallpaperFit::Center:
    return DWPOS_CENTER;
  case WallpaperFit::Tile:
    return DWPOS_TILE;
  }
  return DWPOS_FILL;
}

std::string hresultMessage(HRESULT hr) { return std::system_category().message(hr); }

// Resolves a QScreen::name() GDI adapter name (e.g. \\.\DISPLAY1) into the monitor device
// interface path IDesktopWallpaper expects.
std::optional<std::wstring> monitorIdForScreen(IDesktopWallpaper *wallpaper, const std::string &screen) {
  DISPLAY_DEVICEW device{};
  device.cb = sizeof(device);
  const std::wstring adapter = QString::fromStdString(screen).toStdWString();
  if (!EnumDisplayDevicesW(adapter.c_str(), 0, &device, EDD_GET_DEVICE_INTERFACE_NAME)) {
    return std::nullopt;
  }

  UINT count = 0;
  if (FAILED(wallpaper->GetMonitorDevicePathCount(&count))) return std::nullopt;

  for (UINT i = 0; i < count; ++i) {
    LPWSTR pathAt = nullptr;
    if (FAILED(wallpaper->GetMonitorDevicePathAt(i, &pathAt))) continue;
    std::wstring id = pathAt;
    CoTaskMemFree(pathAt);
    if (_wcsicmp(id.c_str(), device.DeviceID) == 0) return id;
  }

  return std::nullopt;
}

std::expected<void, std::string> apply(const WallpaperRequest &request) {
  ScopedCom com;
  ComPtr<IDesktopWallpaper> wallpaper;

  HRESULT hr = CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&wallpaper));
  if (FAILED(hr)) {
    return std::unexpected("failed to reach the desktop wallpaper service: " + hresultMessage(hr));
  }

  std::optional<std::wstring> monitorId;
  if (request.screen) monitorId = monitorIdForScreen(wallpaper.Get(), *request.screen);

  const std::wstring path = std::filesystem::path(request.path).wstring();

  hr = wallpaper->SetWallpaper(monitorId ? monitorId->c_str() : nullptr, path.c_str());
  if (FAILED(hr)) return std::unexpected("failed to set wallpaper: " + hresultMessage(hr));

  hr = wallpaper->SetPosition(positionForFit(request.fit));
  if (FAILED(hr)) return std::unexpected("failed to set wallpaper position: " + hresultMessage(hr));

  return {};
}

} // namespace

QFuture<std::expected<void, std::string>>
WindowsWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  return QtConcurrent::run(apply, request);
}
