#include "win-recent-files-provider.hpp"
#include <windows.h>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <wrl/client.h>
#include "utils/scoped-com.hpp"
#include "services/files-service/windows/win-file-candidates.hpp"
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <cwctype>
#include <optional>
#include <string>
#include <system_error>
#include <unordered_set>

namespace fs = std::filesystem;
using Microsoft::WRL::ComPtr;

namespace {

std::optional<fs::path> recentFolder() {
  PWSTR raw = nullptr;
  const HRESULT hr = SHGetKnownFolderPath(FOLDERID_Recent, KF_FLAG_DEFAULT, nullptr, &raw);
  std::optional<fs::path> result;
  if (SUCCEEDED(hr) && raw) result = fs::path(raw);
  if (raw) CoTaskMemFree(raw);
  return result;
}

// Reads the stored target without IShellLink::Resolve, which can block on the network.
std::optional<fs::path> shortcutTarget(const fs::path &lnk) {
  ComPtr<IShellLinkW> link;
  if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) {
    return std::nullopt;
  }

  ComPtr<IPersistFile> persist;
  if (FAILED(link.As(&persist)) || FAILED(persist->Load(lnk.wstring().c_str(), STGM_READ))) {
    return std::nullopt;
  }

  wchar_t buf[MAX_PATH] = {};
  if (FAILED(link->GetPath(buf, MAX_PATH, nullptr, SLGP_RAWPATH)) || buf[0] == L'\0') return std::nullopt;

  return fs::path(buf);
}

std::wstring caseFoldedKey(const fs::path &path) {
  auto key = path.wstring();
  std::ranges::transform(key, key.begin(), [](wchar_t c) { return std::towlower(c); });
  return key;
}

bool matchesCategory(const fs::path &path, const RecentFilesParams &params) {
  if (!params.category) return true;
  std::error_code ec;
  return winFileCategory({.path = path, .isDirectory = fs::is_directory(path, ec)}) == *params.category;
}

std::vector<fs::path> listRecent(const RecentFilesParams &params) {
  std::vector<fs::path> result;
  if (params.limit <= 0) return result;

  auto folder = recentFolder();
  if (!folder) return result;

  struct Entry {
    fs::file_time_type lastWrite;
    fs::path lnk;
  };

  std::vector<Entry> entries;
  std::error_code ec;

  for (auto const &entry : fs::directory_iterator(*folder, ec)) {
    if (!entry.is_regular_file(ec) || entry.path().extension() != L".lnk") continue;
    entries.emplace_back(Entry{.lastWrite = entry.last_write_time(ec), .lnk = entry.path()});
  }

  std::ranges::sort(entries, [](const Entry &a, const Entry &b) { return a.lastWrite > b.lastWrite; });

  ScopedCom com;
  std::unordered_set<std::wstring> seen;

  for (auto const &entry : entries) {
    if (std::ssize(result) >= params.limit) break;

    auto target = shortcutTarget(entry.lnk);
    if (!target || !fs::exists(*target, ec)) continue;
    if (!seen.insert(caseFoldedKey(*target)).second) continue;
    if (!matchesCategory(*target, params)) continue;

    result.emplace_back(std::move(*target));
  }

  return result;
}

} // namespace

QFuture<std::vector<fs::path>> WinRecentFilesProvider::listAsync(const RecentFilesParams &params) {
  return QtConcurrent::run([params] { return listRecent(params); });
}

void WinRecentFilesProvider::recordAccess(const fs::path &path) {
  SHAddToRecentDocs(SHARD_PATHW, path.wstring().c_str());
}
