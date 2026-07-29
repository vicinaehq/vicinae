#include "win-file-icon-loader.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shobjidl_core.h>
#include <wrl/client.h>

#include "utils/scoped-com.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>

namespace {

// Items of the "All Tasks" folder cannot be re-created from a parsing name (the folder does not
// implement ParseDisplayName); they are resolved by enumerating the folder and matching the
// in-folder parsing name ("{guid}").
constexpr const wchar_t *ALL_TASKS_FOLDER = L"shell:::{ED7BA470-8E54-465E-825C-99712043E01C}";
constexpr QStringView ALL_TASKS_PREFIX = u"::{ED7BA470-8E54-465E-825C-99712043E01C}\\";

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

// Enumerating the folder is expensive (it instantiates every task provider), so it is done once
// and the PIDLs are kept as raw bytes, which are safe to rebind from any COM-initialized thread.
struct AllTasksCache {
  QByteArray parentPidl;
  std::unordered_map<QString, QByteArray> children; // lowercase "{guid}" -> child PIDL bytes
};

const AllTasksCache &allTasksCache() {
  static const AllTasksCache cache = [] {
    AllTasksCache c;

    PIDLIST_ABSOLUTE folderPidl = nullptr;
    if (FAILED(SHParseDisplayName(ALL_TASKS_FOLDER, nullptr, &folderPidl, 0, nullptr))) return c;
    c.parentPidl = QByteArray(reinterpret_cast<const char *>(folderPidl), ILGetSize(folderPidl));

    Microsoft::WRL::ComPtr<IShellFolder> folder;
    const HRESULT bindHr = SHBindToObject(nullptr, folderPidl, nullptr, IID_PPV_ARGS(&folder));
    ILFree(folderPidl);
    if (FAILED(bindHr)) return c;

    Microsoft::WRL::ComPtr<IEnumIDList> ids;
    if (folder->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &ids) != S_OK || !ids) return c;

    PITEMID_CHILD child = nullptr;
    while (ids->Next(1, &child, nullptr) == S_OK) {
      STRRET ret;
      wchar_t buf[128];
      if (SUCCEEDED(folder->GetDisplayNameOf(child, SHGDN_FORPARSING | SHGDN_INFOLDER, &ret)) &&
          SUCCEEDED(StrRetToBufW(&ret, child, buf, static_cast<UINT>(std::size(buf))))) {
        c.children.emplace(QString::fromWCharArray(buf).toLower(),
                           QByteArray(reinterpret_cast<const char *>(child), ILGetSize(child)));
      }
      ILFree(child);
    }

    return c;
  }();
  return cache;
}

Microsoft::WRL::ComPtr<IShellItemImageFactory> allTasksItemFactory(const QString &taskId) {
  Microsoft::WRL::ComPtr<IShellItemImageFactory> factory;

  const AllTasksCache &cache = allTasksCache();
  const auto it = cache.children.find(taskId.toLower());
  if (it == cache.children.end() || cache.parentPidl.isEmpty()) return factory;

  Microsoft::WRL::ComPtr<IShellFolder> folder;
  const auto *parent = reinterpret_cast<PCIDLIST_ABSOLUTE>(cache.parentPidl.constData());
  if (FAILED(SHBindToObject(nullptr, parent, nullptr, IID_PPV_ARGS(&folder)))) return factory;

  const auto *child = reinterpret_cast<PCUITEMID_CHILD>(it->second.constData());
  SHCreateItemWithParent(nullptr, folder.Get(), child, IID_PPV_ARGS(&factory));
  return factory;
}

} // namespace

QImage renderWinShellIcon(const QString &parsingName, const QSize &size) {
  if (parsingName.isEmpty() || size.isEmpty()) return {};

  // STA required: from an MTA thread GetImage returns E_PENDING for icons not yet in the shell cache
  ScopedCom com;

  QImage result;

  Microsoft::WRL::ComPtr<IShellItemImageFactory> factory;
  HRESULT hr = E_FAIL;
  if (parsingName.startsWith(ALL_TASKS_PREFIX)) {
    factory = allTasksItemFactory(parsingName.mid(ALL_TASKS_PREFIX.size()));
    if (factory) hr = S_OK;
  } else {
    const std::wstring wname = parsingName.toStdWString();
    hr = SHCreateItemFromParsingName(wname.c_str(), nullptr, IID_PPV_ARGS(&factory));
  }
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

  return result;
}

QImage renderWinStockIcon(int stockIconId, const QSize &size) {
  if (size.isEmpty()) return {};

  SHSTOCKICONINFO info{};
  info.cbSize = sizeof(info);
  if (FAILED(SHGetStockIconInfo(static_cast<SHSTOCKICONID>(stockIconId), SHGSI_ICONLOCATION, &info))) {
    return {};
  }

  const UINT dim = static_cast<UINT>(std::max(size.width(), size.height()));
  HICON icon = nullptr;
  if (FAILED(SHDefExtractIconW(info.szPath, info.iIcon, 0, &icon, nullptr, dim)) || !icon) return {};

  QImage result = QImage::fromHICON(icon);
  DestroyIcon(icon);
  return result;
}
