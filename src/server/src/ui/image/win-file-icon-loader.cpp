#include "win-file-icon-loader.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <wrl/client.h>

#include <string>

namespace {

// Some icons come back with an all-zero alpha channel (stored without alpha); force those opaque.
QImage imageFromHBITMAP(HBITMAP bitmap) {
  BITMAP bm{};
  if (GetObject(bitmap, sizeof(bm), &bm) == 0) return {};

  const int w = bm.bmWidth;
  const int h = bm.bmHeight;
  if (w <= 0 || h <= 0) return {};

  BITMAPINFO bmi{};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = w;
  bmi.bmiHeader.biHeight = -h; // negative: top-down
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  QImage image(w, h, QImage::Format_ARGB32);
  if (image.isNull()) return {};

  HDC hdc = GetDC(nullptr);
  if (!hdc) return {};
  const int lines = GetDIBits(hdc, bitmap, 0, static_cast<UINT>(h), image.bits(), &bmi, DIB_RGB_COLORS);
  ReleaseDC(nullptr, hdc);
  if (lines == 0) return {};

  bool hasAlpha = false;
  for (int y = 0; y < h && !hasAlpha; ++y) {
    const auto *row = reinterpret_cast<const QRgb *>(image.constScanLine(y));
    for (int x = 0; x < w; ++x) {
      if (qAlpha(row[x]) != 0) {
        hasAlpha = true;
        break;
      }
    }
  }

  if (!hasAlpha) {
    for (int y = 0; y < h; ++y) {
      auto *row = reinterpret_cast<QRgb *>(image.scanLine(y));
      for (int x = 0; x < w; ++x) {
        row[x] |= 0xFF000000;
      }
    }
  }

  return image;
}

} // namespace

QImage renderWinShellIcon(const QString &parsingName, const QSize &size) {
  if (parsingName.isEmpty() || size.isEmpty()) return {};

  // STA required: from an MTA thread GetImage returns E_PENDING for icons not yet in the shell cache
  const HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  const bool ownsCom = coInit == S_OK || coInit == S_FALSE;

  QImage result;

  // scoped so the factory is released before CoUninitialize
  {
    Microsoft::WRL::ComPtr<IShellItemImageFactory> factory;
    const std::wstring wname = parsingName.toStdWString();
    HRESULT hr = SHCreateItemFromParsingName(wname.c_str(), nullptr, IID_PPV_ARGS(&factory));
    if (SUCCEEDED(hr) && factory) {
      const SIZE requested{size.width(), size.height()};
      HBITMAP bitmap = nullptr;
      // ICONONLY: never a document thumbnail. BIGGERSIZEOK: allow upscaling for sharpness.
      hr = factory->GetImage(requested, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap);
      if (SUCCEEDED(hr) && bitmap) {
        result = imageFromHBITMAP(bitmap);
        DeleteObject(bitmap);
      }
    }
  }

  if (ownsCom) CoUninitialize();

  return result;
}
