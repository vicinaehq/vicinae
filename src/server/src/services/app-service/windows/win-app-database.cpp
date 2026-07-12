#include "win-app-database.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl_core.h>
#include <propsys.h>
#include <initguid.h> // defines the BHID_*/PKEY_* GUIDs below in this TU
#include <shlguid.h>
#include <propkey.h>
#include <wrl/client.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>

#include <QDebug>
#include <array>
#include <string>

namespace fs = std::filesystem;
using Microsoft::WRL::ComPtr;

namespace {

// Tolerates Qt's existing COM init (S_FALSE) and a foreign apartment mode (RPC_E_CHANGED_MODE).
class ScopedCom {
public:
  ScopedCom() {
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    m_owns = hr == S_OK || hr == S_FALSE;
  }
  ~ScopedCom() {
    if (m_owns) CoUninitialize();
  }
  ScopedCom(const ScopedCom &) = delete;
  ScopedCom &operator=(const ScopedCom &) = delete;

private:
  bool m_owns = false;
};

std::optional<fs::path> knownFolder(REFKNOWNFOLDERID id) {
  PWSTR raw = nullptr;
  const HRESULT hr = SHGetKnownFolderPath(id, KF_FLAG_DEFAULT, nullptr, &raw);
  std::optional<fs::path> result;
  if (SUCCEEDED(hr) && raw) result = fs::path(raw);
  if (raw) CoTaskMemFree(raw);
  return result;
}

std::vector<fs::path> desktopRoots() {
  std::vector<fs::path> roots;
  if (auto user = knownFolder(FOLDERID_Desktop)) roots.emplace_back(std::move(*user));
  if (auto common = knownFolder(FOLDERID_PublicDesktop)) roots.emplace_back(std::move(*common));
  return roots;
}

struct ShortcutInfo {
  std::optional<fs::path> target; // nullopt for PIDL-backed shortcuts we cannot resolve
  QString arguments;
  QString workingDirectory;
  QString aumid;
};

// Reads the stored link data without IShellLink::Resolve, which can block on the network.
ShortcutInfo readShortcutInfo(const fs::path &lnk) {
  ShortcutInfo info;

  ComPtr<IShellLinkW> link;
  if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) {
    return info;
  }

  ComPtr<IPersistFile> persist;
  if (FAILED(link.As(&persist)) || FAILED(persist->Load(lnk.wstring().c_str(), STGM_READ))) {
    return info;
  }

  wchar_t buf[MAX_PATH] = {};
  WIN32_FIND_DATAW find = {};
  if (SUCCEEDED(link->GetPath(buf, MAX_PATH, &find, 0)) && buf[0] != L'\0') {
    info.target = fs::path(buf);
  }

  if (!info.target) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(link->GetIDList(&pidl)) && pidl) {
      PWSTR name = nullptr;
      if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_FILESYSPATH, &name)) && name) {
        info.target = fs::path(name);
      } else if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &name)) && name) {
        // the File Explorer CLSID
        if (_wcsicmp(name, L"::{52205FD8-5DFB-447D-801A-D0B52F2E83E1}") == 0) {
          if (const wchar_t *windir = _wgetenv(L"WINDIR"); windir && *windir) {
            info.target = fs::path(windir) / L"explorer.exe";
          }
        }
      }
      if (name) CoTaskMemFree(name);
      CoTaskMemFree(pidl);
    }
  }

  wchar_t args[1024] = {};
  if (SUCCEEDED(link->GetArguments(args, 1024)) && args[0] != L'\0') {
    info.arguments = QString::fromWCharArray(args).trimmed();
  }

  wchar_t dir[MAX_PATH] = {};
  if (SUCCEEDED(link->GetWorkingDirectory(dir, MAX_PATH)) && dir[0] != L'\0') {
    wchar_t expanded[MAX_PATH] = {};
    const DWORD n = ExpandEnvironmentStringsW(dir, expanded, MAX_PATH);
    info.workingDirectory = QString::fromWCharArray((n > 0 && n <= MAX_PATH) ? expanded : dir);
  }

  ComPtr<IPropertyStore> store;
  if (SUCCEEDED(link.As(&store))) {
    PROPVARIANT value;
    PropVariantInit(&value);
    if (SUCCEEDED(store->GetValue(PKEY_AppUserModel_ID, &value)) && value.vt == VT_LPWSTR &&
        value.pwszVal) {
      info.aumid = QString::fromWCharArray(value.pwszVal);
    }
    PropVariantClear(&value);
  }

  return info;
}

// AUMID if present, else target + args, else the shortcut file itself
QString win32AppId(const QString &program, const QString &arguments = {}, const QString &aumid = {},
                   const fs::path &shortcut = {}) {
  if (!aumid.isEmpty()) return QStringLiteral("win32:aumid:") + aumid.toLower();
  if (!program.isEmpty()) {
    QString id = QStringLiteral("win32:") + program.toLower();
    if (!arguments.isEmpty()) id += QLatin1Char(' ') + arguments.toLower();
    return id;
  }
  return QStringLiteral("win32:") + QString::fromStdWString(shortcut.wstring()).toLower();
}

QString uwpAppId(const QString &aumid) { return QStringLiteral("uwp:") + aumid.toLower(); }

QString readUrlTarget(const fs::path &url) {
  wchar_t buf[2048] = {};
  GetPrivateProfileStringW(L"InternetShortcut", L"URL", L"", buf, 2048, url.wstring().c_str());
  return QString::fromWCharArray(buf);
}

bool isGameLauncherUrl(const QString &url) {
  static constexpr std::array schemes = {"steam:",   "com.epicgames.launcher:",
                                         "goggalaxy:", "uplay:",
                                         "ubisoft:",  "battlenet:",
                                         "origin:",   "link2ea:",
                                         "amazon-games:"};
  const QString lower = url.toLower();
  return std::ranges::any_of(schemes,
                             [&](const char *s) { return lower.startsWith(QLatin1String(s)); });
}

bool looksLikeUninstaller(const fs::path &target) {
  auto name = target.filename().wstring();
  for (auto &c : name)
    c = static_cast<wchar_t>(::towlower(c));
  return name.find(L"unins") != std::wstring::npos;
}

bool isMsiUninstall(const std::optional<fs::path> &target, const QString &arguments) {
  if (!target) return false;
  auto name = target->filename().wstring();
  for (auto &c : name)
    c = static_cast<wchar_t>(::towlower(c));
  return name == L"msiexec.exe" && arguments.contains(QLatin1String("/x"), Qt::CaseInsensitive);
}

std::wstring lowerExtension(const fs::path &p) {
  auto ext = p.extension().wstring();
  for (auto &c : ext)
    c = static_cast<wchar_t>(::towlower(c));
  return ext;
}

bool isUnderDirectory(const fs::path &p, const fs::path &dir) {
  std::wstring a = p.wstring();
  std::wstring b = dir.wstring();
  for (auto &c : a)
    c = static_cast<wchar_t>(::towlower(c));
  for (auto &c : b)
    c = static_cast<wchar_t>(::towlower(c));
  if (!b.empty() && b.back() != L'\\') b += L'\\';
  return a.starts_with(b);
}

QString exeFileDescription(const fs::path &exe) {
  DWORD ignored = 0;
  const DWORD size = GetFileVersionInfoSizeW(exe.c_str(), &ignored);
  if (!size) return {};

  std::vector<BYTE> info(size);
  if (!GetFileVersionInfoW(exe.c_str(), 0, size, info.data())) return {};

  struct LangCp {
    WORD lang;
    WORD codepage;
  };
  std::vector<LangCp> translations;
  LangCp *table = nullptr;
  UINT cb = 0;
  if (VerQueryValueW(info.data(), L"\\VarFileInfo\\Translation", reinterpret_cast<void **>(&table),
                     &cb) &&
      table) {
    translations.assign(table, table + cb / sizeof(LangCp));
  }
  translations.push_back({0x0409, 0x04B0}); // en-US Unicode, common even without a table

  for (const wchar_t *field : {L"FileDescription", L"ProductName"}) {
    for (const auto &t : translations) {
      wchar_t query[64];
      swprintf_s(query, L"\\StringFileInfo\\%04x%04x\\%s", t.lang, t.codepage, field);
      wchar_t *value = nullptr;
      UINT len = 0;
      if (VerQueryValueW(info.data(), query, reinterpret_cast<void **>(&value), &len) && value &&
          len > 1) {
        const QString name = QString::fromWCharArray(value).trimmed();
        if (!name.isEmpty()) return name;
      }
    }
  }
  return {};
}

std::optional<fs::path> readAppPathValue(HKEY appPathsKey, const std::wstring &subName) {
  HKEY sub = nullptr;
  if (RegOpenKeyExW(appPathsKey, subName.c_str(), 0, KEY_READ, &sub) != ERROR_SUCCESS) {
    return std::nullopt;
  }

  std::optional<fs::path> result;
  wchar_t buf[1024] = {};
  DWORD cb = sizeof(buf);
  DWORD type = 0;
  if (RegQueryValueExW(sub, nullptr, nullptr, &type, reinterpret_cast<LPBYTE>(buf), &cb) ==
          ERROR_SUCCESS &&
      (type == REG_SZ || type == REG_EXPAND_SZ)) {
    std::wstring value(buf, cb / sizeof(wchar_t));
    while (!value.empty() && (value.back() == L'\0')) value.pop_back();
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"') {
      value = value.substr(1, value.size() - 2);
    }
    if (!value.empty()) {
      wchar_t expanded[1024] = {};
      const DWORD n = ExpandEnvironmentStringsW(value.c_str(), expanded, 1024);
      result = fs::path((n > 0 && n <= 1024) ? std::wstring(expanded, n - 1) : value);
    }
  }

  RegCloseKey(sub);
  return result;
}

std::vector<fs::path> enumerateAppPaths() {
  std::vector<fs::path> result;
  static constexpr const wchar_t *kSubKey =
      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths";

  auto readHive = [&](HKEY root, REGSAM extraSam) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, kSubKey, 0, KEY_READ | extraSam, &key) != ERROR_SUCCESS) return;

    for (DWORD i = 0;; ++i) {
      wchar_t nameBuf[256];
      DWORD nameLen = 256;
      const LONG r = RegEnumKeyExW(key, i, nameBuf, &nameLen, nullptr, nullptr, nullptr, nullptr);
      if (r != ERROR_SUCCESS) break;
      if (auto exe = readAppPathValue(key, std::wstring(nameBuf, nameLen))) {
        result.emplace_back(std::move(*exe));
      }
    }

    RegCloseKey(key);
  };

  readHive(HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY);
  readHive(HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY);
  readHive(HKEY_CURRENT_USER, 0);
  return result;
}

bool shellExecuteOpen(const std::wstring &target, const wchar_t *params = nullptr,
                      const wchar_t *workdir = nullptr) {
  const HINSTANCE ret =
      ShellExecuteW(nullptr, L"open", target.c_str(), params, workdir, SW_SHOWNORMAL);
  return reinterpret_cast<INT_PTR>(ret) > 32;
}

QString fromHString(const winrt::hstring &s) {
  return QString::fromWCharArray(s.c_str(), static_cast<int>(s.size()));
}

std::wstring quoteArg(const QString &arg) {
  const std::wstring w = arg.toStdWString();
  if (!w.empty() && w.find_first_of(L" \t\"") == std::wstring::npos) return w;

  // argv rules: backslashes are literal except before a quote
  std::wstring out = L"\"";
  size_t backslashes = 0;
  for (const wchar_t c : w) {
    if (c == L'\\') {
      ++backslashes;
      continue;
    }
    if (c == L'"') {
      out.append(backslashes * 2 + 1, L'\\');
    } else {
      out.append(backslashes, L'\\');
    }
    backslashes = 0;
    out += c;
  }
  out.append(backslashes * 2, L'\\');
  out += L'"';
  return out;
}

// quotes included, so cmd never enters quote mode
std::wstring cmdEscape(const std::wstring &s) {
  std::wstring out;
  out.reserve(s.size());
  for (const wchar_t c : s) {
    if (wcschr(L"()%!^\"<>&|", c)) out += L'^';
    out += c;
  }
  return out;
}

std::wstring joinArgs(const std::vector<QString> &args) {
  std::wstring out;
  for (size_t i = 0; i < args.size(); ++i) {
    if (i) out += L' ';
    out += quoteArg(args[i]);
  }
  return out;
}

enum class AssocKind { None, Protocol, Extension, Directory };
struct Assoc {
  AssocKind kind = AssocKind::None;
  std::wstring value; // "http" for a protocol, ".txt" for an extension, "Directory" for a folder
};

Assoc classifyTarget(const QString &target) {
  if (target.isEmpty()) return {};

  const int schemeSep = target.indexOf(QStringLiteral("://"));
  const bool driveLetter =
      target.size() >= 2 && target[1] == QLatin1Char(':') && target[0].isLetter();

  if (schemeSep > 0) {
    QString scheme = target.left(schemeSep).toLower();
    if (scheme != QStringLiteral("file")) return {AssocKind::Protocol, scheme.toStdWString()};
  } else if (!driveLetter) {
    if (const int colon = target.indexOf(QLatin1Char(':')); colon > 1) {
      return {AssocKind::Protocol, target.left(colon).toLower().toStdWString()};
    }
  }

  // a dead UNC host can block for seconds
  std::error_code ec;
  if (!target.startsWith(QLatin1String("\\\\")) &&
      fs::is_directory(fs::path(target.toStdWString()), ec)) {
    return {AssocKind::Directory, L"Directory"};
  }

  std::wstring ext = fs::path(target.toStdWString()).extension().wstring();
  if (ext.empty()) return {};
  for (auto &c : ext)
    c = static_cast<wchar_t>(::towlower(c));
  return {AssocKind::Extension, std::move(ext)};
}

QString friendlyAppName(const std::wstring &assoc) {
  wchar_t buf[512];
  DWORD len = 512;
  if (SUCCEEDED(AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_FRIENDLYAPPNAME, assoc.c_str(), nullptr, buf,
                                  &len)) &&
      len > 1) {
    return QString::fromWCharArray(buf, len - 1);
  }
  return {};
}

std::optional<fs::path> defaultHandlerExe(const std::wstring &assoc) {
  wchar_t buf[1024];
  DWORD len = 1024;
  const HRESULT hr =
      AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE, assoc.c_str(), nullptr, buf, &len);
  if (SUCCEEDED(hr) && len > 1) return fs::path(std::wstring(buf, len - 1));
  return std::nullopt;
}

std::vector<std::pair<fs::path, QString>> enumExtensionHandlers(const std::wstring &ext,
                                                                ASSOC_FILTER filter) {
  std::vector<std::pair<fs::path, QString>> result;

  ComPtr<IEnumAssocHandlers> handlers;
  if (FAILED(SHAssocEnumHandlers(ext.c_str(), filter, &handlers)) || !handlers) {
    return result;
  }

  ComPtr<IAssocHandler> handler;
  ULONG fetched = 0;
  while (handlers->Next(1, &handler, &fetched) == S_OK && fetched == 1) {
    LPWSTR path = nullptr;
    LPWSTR ui = nullptr;
    if (SUCCEEDED(handler->GetName(&path)) && path) {
      QString display;
      if (SUCCEEDED(handler->GetUIName(&ui)) && ui) display = QString::fromWCharArray(ui);
      result.emplace_back(fs::path(path), display);
    }
    if (path) CoTaskMemFree(path);
    if (ui) CoTaskMemFree(ui);
  }

  return result;
}

ComPtr<IShellItemArray> shellItemArrayFromParsingNames(const std::vector<QString> &names) {
  std::vector<PIDLIST_ABSOLUTE> pidls;
  for (const auto &name : names) {
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(name.toStdWString().c_str(), nullptr,
                                              IID_PPV_ARGS(&item))) &&
        item) {
      PIDLIST_ABSOLUTE pidl = nullptr;
      if (SUCCEEDED(SHGetIDListFromObject(item.Get(), &pidl))) pidls.push_back(pidl);
    }
  }

  ComPtr<IShellItemArray> array;
  if (!pidls.empty()) {
    SHCreateShellItemArrayFromIDLists(static_cast<UINT>(pidls.size()),
                                      const_cast<PCIDLIST_ABSOLUTE *>(pidls.data()), &array);
  }
  for (auto *pidl : pidls)
    CoTaskMemFree(pidl);
  return array;
}

// Re-resolves the handler by name and invokes it; works for packaged handlers, whose exe under
// WindowsApps cannot be spawned directly.
bool invokeAssocHandler(const std::wstring &ext, const QString &handlerName,
                        const std::vector<QString> &files) {
  // "Directory" handlers are not in the recommended set
  ComPtr<IEnumAssocHandlers> handlers;
  if (FAILED(SHAssocEnumHandlers(ext.c_str(), ASSOC_FILTER_NONE, &handlers)) || !handlers) {
    return false;
  }

  ComPtr<IAssocHandler> handler;
  ULONG fetched = 0;
  while (handlers->Next(1, &handler, &fetched) == S_OK && fetched == 1) {
    LPWSTR name = nullptr;
    const bool match = SUCCEEDED(handler->GetName(&name)) && name &&
                       handlerName.compare(QString::fromWCharArray(name), Qt::CaseInsensitive) == 0;
    if (name) CoTaskMemFree(name);
    if (!match) continue;

    const auto array = shellItemArrayFromParsingNames(files);
    if (!array) return false;

    ComPtr<IDataObject> dataObject;
    if (FAILED(array->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&dataObject))) ||
        !dataObject) {
      return false;
    }

    ComPtr<IAssocHandlerInvoker> invoker;
    if (FAILED(handler->CreateInvoker(dataObject.Get(), &invoker)) || !invoker) return false;

    return SUCCEEDED(invoker->Invoke());
  }

  return false;
}

// Packaged apps never receive argv; documents and URIs are handed over through activation.
bool activatePackaged(const std::wstring &aumid, const std::vector<QString> &files,
                      const std::vector<QString> &uris) {
  ComPtr<IApplicationActivationManager> manager;
  if (FAILED(CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&manager))) ||
      !manager) {
    return false;
  }

  bool ok = true;
  DWORD pid = 0;

  if (!files.empty()) {
    const auto array = shellItemArrayFromParsingNames(files);
    ok = array && SUCCEEDED(manager->ActivateForFile(aumid.c_str(), array.Get(), L"open", &pid));
  }

  // Protocol activation takes a single URI at a time.
  for (const auto &uri : uris) {
    const auto array = shellItemArrayFromParsingNames({uri});
    ok = array && SUCCEEDED(manager->ActivateForProtocol(aumid.c_str(), array.Get(), &pid)) && ok;
  }

  return ok;
}

bool defaultTerminalIsConhost() {
  wchar_t buf[64] = {};
  DWORD cb = sizeof(buf);
  if (RegGetValueW(HKEY_CURRENT_USER, L"Console\\%%Startup", L"DelegationTerminal", RRF_RT_REG_SZ,
                   nullptr, buf, &cb) != ERROR_SUCCESS) {
    return false;
  }
  // the classic conhost delegation CLSID
  return _wcsicmp(buf, L"{B23D10C0-E52E-411E-9D5B-C09FDF709C7D}") == 0;
}

// SearchPathW also resolves App Execution Aliases such as wt.exe.
std::optional<fs::path> searchExecutable(const wchar_t *name) {
  wchar_t buf[MAX_PATH];
  const DWORD n = SearchPathW(nullptr, name, nullptr, MAX_PATH, buf, nullptr);
  if (n > 0 && n < MAX_PATH) return fs::path(std::wstring(buf, n));
  return std::nullopt;
}

} // namespace

struct UwpPackageWatcher {
  winrt::Windows::ApplicationModel::PackageCatalog catalog{nullptr};
  winrt::Windows::ApplicationModel::PackageCatalog::PackageInstalling_revoker installing;
  winrt::Windows::ApplicationModel::PackageCatalog::PackageUninstalling_revoker uninstalling;
  winrt::Windows::ApplicationModel::PackageCatalog::PackageUpdating_revoker updating;
};

WindowsAppDatabase::WindowsAppDatabase() {
  connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this] { Q_EMIT changed(); });

  scan(defaultSearchPaths());

  m_rescanTimer.setInterval(std::chrono::minutes(5));
  connect(&m_rescanTimer, &QTimer::timeout, this, [this] { Q_EMIT changed(); });
  m_rescanTimer.start();

  using namespace winrt::Windows::ApplicationModel;
  try {
    auto watcher = std::make_unique<UwpPackageWatcher>();
    watcher->catalog = PackageCatalog::OpenForCurrentUser();
    // Progress events fire repeatedly; only rescan on completion. Cross-thread emit is queued.
    auto handler = [this](const auto &, const auto &args) {
      if (args.IsComplete()) {
        m_uwpDirty.store(true);
        Q_EMIT changed();
      }
    };
    watcher->installing = watcher->catalog.PackageInstalling(winrt::auto_revoke, handler);
    watcher->uninstalling = watcher->catalog.PackageUninstalling(winrt::auto_revoke, handler);
    watcher->updating = watcher->catalog.PackageUpdating(winrt::auto_revoke, handler);
    m_uwpWatcher = std::move(watcher);
  } catch (const winrt::hresult_error &e) {
    qWarning() << "[win-app-db] package watcher unavailable:" << fromHString(e.message());
  }
}

WindowsAppDatabase::~WindowsAppDatabase() { m_uwpWatcher.reset(); }

std::vector<fs::path> WindowsAppDatabase::defaultSearchPaths() const {
  std::vector<fs::path> paths;
  if (auto common = knownFolder(FOLDERID_CommonPrograms)) paths.emplace_back(std::move(*common));
  if (auto user = knownFolder(FOLDERID_Programs)) paths.emplace_back(std::move(*user));
  return paths;
}

void WindowsAppDatabase::addApp(std::shared_ptr<WindowsApplication> app) {
  if (!app || m_appsById.contains(app->id())) return;
  m_appsById.emplace(app->id(), app);
  m_apps.emplace_back(std::move(app));
}

void WindowsAppDatabase::addShortcut(const fs::path &file) {
  const std::wstring ext = lowerExtension(file);
  const bool isLnk = ext == L".lnk";
  const bool isUrl = ext == L".url";
  const bool isAppRef = ext == L".appref-ms";
  if (!isLnk && !isUrl && !isAppRef) return;

  if (looksLikeUninstaller(file)) return;

  ShortcutInfo info;
  QString program; // .lnk: target exe. .url: target URL.
  if (isLnk) {
    info = readShortcutInfo(file);
    if (info.target && looksLikeUninstaller(*info.target)) return;
    if (isMsiUninstall(info.target, info.arguments)) return;
    if (!info.aumid.isEmpty() && m_appsById.contains(uwpAppId(info.aumid))) return;
    if (info.target) program = QString::fromStdWString(info.target->wstring());
  } else if (isUrl) {
    program = readUrlTarget(file);
  }

  WindowsApplication::Data data;
  data.id = win32AppId(program, info.arguments, info.aumid, file);
  data.displayName = QString::fromStdWString(file.stem().wstring());
  data.program = program;
  data.arguments = info.arguments;
  data.workingDirectory = info.workingDirectory;
  data.path = file;
  data.shellParsingName = QString::fromStdWString(file.wstring());
  data.packaged = false;
  if (isUrl) {
    if (isGameLauncherUrl(program)) {
      data.category = QStringLiteral("Game");
    } else if (program.startsWith(QLatin1String("http://"), Qt::CaseInsensitive) ||
               program.startsWith(QLatin1String("https://"), Qt::CaseInsensitive)) {
      data.category = QStringLiteral("Link");
    }
  }

  addApp(std::make_shared<WindowsApplication>(std::move(data)));
}

void WindowsAppDatabase::scanWin32(const std::vector<fs::path> &paths) {
  for (const auto &root : paths) {
    std::error_code ec;
    if (!fs::is_directory(root, ec)) continue;

    m_watchDirs.insert(QString::fromStdWString(root.wstring()));

    fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec);
    const fs::recursive_directory_iterator end;
    if (ec) continue;

    for (; it != end; it.increment(ec)) {
      if (ec) {
        ec.clear();
        continue;
      }
      if (it->is_directory(ec)) {
        m_watchDirs.insert(QString::fromStdWString(it->path().wstring()));
      } else if (it->is_regular_file(ec)) {
        addShortcut(it->path());
      }
    }
  }
}

void WindowsAppDatabase::scanDesktop() {
  for (const auto &root : desktopRoots()) {
    std::error_code ec;
    if (!fs::is_directory(root, ec)) continue;

    m_watchDirs.insert(QString::fromStdWString(root.wstring()));

    fs::directory_iterator it(root, fs::directory_options::skip_permission_denied, ec);
    const fs::directory_iterator end;
    if (ec) continue;

    for (; it != end; it.increment(ec)) {
      if (ec) {
        ec.clear();
        continue;
      }
      if (it->is_regular_file(ec)) addShortcut(it->path());
    }
  }
}

void WindowsAppDatabase::scanAppPaths() {
  fs::path systemRoot;
  if (const wchar_t *env = _wgetenv(L"SystemRoot"); env && *env) systemRoot = env;

  fs::path windowsApps; // packaged apps register execution aliases here; scanUwp already lists them
  if (const wchar_t *env = _wgetenv(L"ProgramW6432"); env && *env) {
    windowsApps = fs::path(env) / L"WindowsApps";
  }

  for (const auto &exe : enumerateAppPaths()) {
    std::error_code ec;
    if (!fs::exists(exe, ec) || looksLikeUninstaller(exe)) continue;
    if (!systemRoot.empty() && isUnderDirectory(exe, systemRoot)) continue;
    if (!windowsApps.empty() && isUnderDirectory(exe, windowsApps)) continue;

    // nameless exes are almost always helper binaries
    const QString description = exeFileDescription(exe);
    if (description.isEmpty()) continue;

    WindowsApplication::Data data;
    data.id = win32AppId(QString::fromStdWString(exe.wstring()));
    data.displayName = description;
    data.program = QString::fromStdWString(exe.wstring());
    data.path = exe;
    data.shellParsingName = QString::fromStdWString(exe.wstring());
    data.packaged = false;

    // Deduped by id, so this only contributes apps without a Start Menu / Desktop shortcut.
    addApp(std::make_shared<WindowsApplication>(std::move(data)));
  }
}

void WindowsAppDatabase::scanUwp() {
  if (m_uwpDirty.load()) refreshUwpCache();
  for (const auto &app : m_uwpCache)
    addApp(app);
}

void WindowsAppDatabase::refreshUwpCache() {
  using namespace winrt::Windows::Management::Deployment;
  using namespace winrt::Windows::ApplicationModel;

  PackageManager manager{nullptr};
  winrt::Windows::Foundation::Collections::IIterable<Package> packages{nullptr};

  try {
    manager = PackageManager();
    packages = manager.FindPackagesForUser(winrt::hstring{}); // empty SID = current user
  } catch (const winrt::hresult_error &e) {
    qWarning() << "[win-app-db] PackageManager query failed:" << fromHString(e.message());
    return;
  }

  m_uwpCache.clear();
  m_uwpDirty.store(false);

  for (const auto &pkg : packages) {
    try {
      if (pkg.IsFramework() || pkg.IsResourcePackage()) continue;

      fs::path installPath;
      try {
        installPath = fs::path(std::wstring(pkg.InstalledLocation().Path()));
      } catch (const winrt::hresult_error &) {
        // no accessible install location; not fatal
      }

      // GetAppListEntries yields exactly the launchable, user-visible entries.
      for (const auto &entry : pkg.GetAppListEntries()) {
        const QString aumid = fromHString(entry.AppUserModelId());
        if (aumid.isEmpty()) continue;

        QString name;
        try {
          name = fromHString(entry.DisplayInfo().DisplayName());
        } catch (const winrt::hresult_error &) {
        }
        if (name.isEmpty()) continue;

        WindowsApplication::Data data;
        data.id = uwpAppId(aumid);
        data.displayName = name;
        data.program = aumid;
        data.path = installPath;
        data.shellParsingName = QStringLiteral("shell:AppsFolder\\") + aumid;
        data.packaged = true;

        m_uwpCache.push_back(std::make_shared<WindowsApplication>(std::move(data)));
      }
    } catch (const winrt::hresult_error &) {
      // skip a single bad package rather than aborting the scan
    }
  }
}

bool WindowsAppDatabase::scan(const std::vector<fs::path> &paths) {
  m_apps.clear();
  m_appsById.clear();
  m_watchDirs.clear();

  ScopedCom com;
  scanUwp(); // first: addShortcut drops shortcuts whose AUMID is already listed
  scanWin32(paths);
  scanDesktop();
  scanAppPaths();

  installWatches();
  return !m_apps.empty();
}

void WindowsAppDatabase::installWatches() {
  if (const QStringList old = m_watcher.directories(); !old.isEmpty()) m_watcher.removePaths(old);
  if (!m_watchDirs.empty()) m_watcher.addPaths(QStringList(m_watchDirs.begin(), m_watchDirs.end()));
}

bool WindowsAppDatabase::launch(const AbstractApplication &app, const std::vector<QString> &args) const {
  const auto *winApp = dynamic_cast<const WindowsApplication *>(&app);
  if (!winApp) return false;

  ScopedCom com;

  // only way to open a target whose default handler is a UWP app with no exe
  if (winApp->opensViaShell()) {
    if (args.empty()) return false;
    bool ok = true;
    for (const auto &target : args) {
      qInfo() << "Shell-opening" << target;
      ok = shellExecuteOpen(target.toStdWString()) && ok;
    }
    return ok;
  }

  if (winApp->isPackaged() && !args.empty()) {
    std::vector<QString> files;
    std::vector<QString> uris;
    for (const auto &arg : args) {
      (classifyTarget(arg).kind == AssocKind::Protocol ? uris : files).emplace_back(arg);
    }
    qInfo() << "Activating packaged app" << winApp->program() << "files" << files << "uris" << uris;
    return activatePackaged(winApp->program().toStdWString(), files, uris);
  }

  if (!args.empty() && !winApp->openerExtension().isEmpty() &&
      invokeAssocHandler(winApp->openerExtension().toStdWString(), winApp->program(), args)) {
    qInfo() << "Invoked assoc handler" << winApp->program() << "with" << args;
    return true;
  }

  const std::wstring params = joinArgs(args);

  // ShellExecute ignores lpParameters for .lnk targets
  if (!args.empty() && !winApp->program().isEmpty() &&
      classifyTarget(winApp->program()).kind != AssocKind::Protocol) {
    std::wstring combined = winApp->arguments().toStdWString();
    if (!combined.empty()) combined += L' ';
    combined += params;
    const std::wstring workdir = winApp->workingDirectory().toStdWString();
    qInfo() << "Launching" << winApp->program() << "args" << QString::fromStdWString(combined);
    return shellExecuteOpen(winApp->program().toStdWString(), combined.c_str(),
                            workdir.empty() ? nullptr : workdir.c_str());
  }

  qInfo() << "Launching" << winApp->shellParsingName() << "args" << QString::fromStdWString(params);
  if (!shellExecuteOpen(winApp->shellParsingName().toStdWString(),
                        params.empty() ? nullptr : params.c_str())) {
    qWarning() << "Failed to launch" << winApp->displayName();
    return false;
  }
  return true;
}

bool WindowsAppDatabase::launchTerminalCommand(const std::vector<QString> &cmdline,
                                               const LaunchTerminalCommandOptions &opts) const {
  if (cmdline.empty()) return false;

  // cmd.exe is always present and gives us /k (hold) vs /c plus a working directory.
  std::wstring comspec = L"cmd.exe";
  if (const wchar_t *env = _wgetenv(L"ComSpec"); env && *env) comspec = env;

  std::wstring inner;
  if (opts.title) inner += L"title " + cmdEscape(opts.title->toStdWString()) + L" & ";
  inner += cmdEscape(joinArgs(cmdline));

  const std::wstring params = (opts.hold ? L"/k " : L"/c ") + inner;
  const std::wstring workdir =
      opts.workingDirectory ? opts.workingDirectory->toStdWString() : std::wstring();

  ScopedCom com;
  const HINSTANCE ret = ShellExecuteW(nullptr, L"open", comspec.c_str(), params.c_str(),
                                      workdir.empty() ? nullptr : workdir.c_str(), SW_SHOWNORMAL);
  return reinterpret_cast<INT_PTR>(ret) > 32;
}

// Prefers the scanned app for the same executable, like the XDG provider; synthesizes one only for
// executables we never scanned (packaged handlers, apps without shortcuts).
WindowsAppDatabase::AppPtr WindowsAppDatabase::resolveExecutable(const fs::path &exe, const QString &name,
                                                                const QString &openerExtension) const {
  // registry handler paths can contain doubled separators
  const fs::path normalized = exe.lexically_normal();
  const auto it = m_appsById.find(win32AppId(QString::fromStdWString(normalized.wstring())));
  if (it != m_appsById.end()) return it->second;
  return appForExecutable(normalized, name, openerExtension);
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::appForExecutable(const fs::path &exe,
                                                               const QString &name,
                                                               const QString &openerExtension) const {
  const QString exeStr = QString::fromStdWString(exe.wstring());

  WindowsApplication::Data data;
  data.id = win32AppId(exeStr);
  data.displayName = name.isEmpty() ? QString::fromStdWString(exe.stem().wstring()) : name;
  data.program = exeStr;
  data.path = exe;
  data.shellParsingName = exeStr;
  data.openerExtension = openerExtension;
  data.packaged = false;
  return std::make_shared<WindowsApplication>(std::move(data));
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::makeShellOpenApp(const QString &target) const {
  const Assoc assoc = classifyTarget(target);

  WindowsApplication::Data data;
  const QString friendly = assoc.kind == AssocKind::None ? QString() : friendlyAppName(assoc.value);
  data.id = QStringLiteral("shell-open:") + QString::fromStdWString(assoc.value);
  data.displayName = friendly.isEmpty() ? QStringLiteral("Open") : friendly;
  data.shellParsingName = target; // icon resolves to the target's type icon
  data.shellOpen = true;
  return std::make_shared<WindowsApplication>(std::move(data));
}

std::vector<WindowsAppDatabase::AppPtr> WindowsAppDatabase::findOpeners(const Target &target) const {
  const Assoc assoc = classifyTarget(target);
  if (assoc.kind == AssocKind::None) return {};

  ScopedCom com;
  std::vector<AppPtr> result;

  if (assoc.kind == AssocKind::Extension) {
    const QString ext = QString::fromStdWString(assoc.value);
    for (auto &[exe, display] : enumExtensionHandlers(assoc.value, ASSOC_FILTER_RECOMMENDED)) {
      result.emplace_back(resolveExecutable(exe, display, ext));
    }
  }

  if (assoc.kind == AssocKind::Directory) {
    if (auto browser = fileBrowser()) result.emplace_back(std::move(browser));
    for (auto &[exe, display] : enumExtensionHandlers(assoc.value, ASSOC_FILTER_NONE)) {
      auto app = resolveExecutable(exe, display, QStringLiteral("Directory"));
      const bool dup =
          std::ranges::any_of(result, [&](const AppPtr &r) { return r->id() == app->id(); });
      if (!dup) result.emplace_back(std::move(app));
    }
  }

  // Protocols have no handler enumeration, and UWP handlers expose no exe.
  if (result.empty()) {
    if (auto exe = defaultHandlerExe(assoc.value))
      result.emplace_back(resolveExecutable(*exe, friendlyAppName(assoc.value)));
    else
      result.emplace_back(makeShellOpenApp(target));
  }

  return result;
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::findDefaultOpener(const Target &target) const {
  const Assoc assoc = classifyTarget(target);
  if (assoc.kind == AssocKind::None) return nullptr;
  if (assoc.kind == AssocKind::Directory) return fileBrowser();

  ScopedCom com;
  if (auto exe = defaultHandlerExe(assoc.value))
    return resolveExecutable(*exe, friendlyAppName(assoc.value));
  return makeShellOpenApp(target); // UWP handler: defer to the shell
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::findById(const QString &id) const {
  if (auto it = m_appsById.find(id); it != m_appsById.end()) return it->second;
  return nullptr;
}

std::vector<WindowsAppDatabase::AppPtr> WindowsAppDatabase::list() const {
  return {m_apps.begin(), m_apps.end()};
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::findByClass(const QString &) const { return nullptr; }

WindowsAppDatabase::AppPtr WindowsAppDatabase::fileBrowser() const {
  ScopedCom com;
  fs::path explorer;
  if (const wchar_t *windir = _wgetenv(L"WINDIR"); windir && *windir) {
    explorer = fs::path(windir) / L"explorer.exe";
  } else if (auto found = searchExecutable(L"explorer.exe")) {
    explorer = *found;
  } else {
    return nullptr;
  }
  return appForExecutable(explorer, QStringLiteral("File Explorer"));
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::genericTextEditor() const {
  ScopedCom com;
  if (auto exe = defaultHandlerExe(L".txt")) return resolveExecutable(*exe, friendlyAppName(L".txt"));
  if (auto notepad = searchExecutable(L"notepad.exe")) return appForExecutable(*notepad, {});
  return nullptr;
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::webBrowser() const {
  ScopedCom com;
  if (auto exe = defaultHandlerExe(L"http")) return resolveExecutable(*exe, friendlyAppName(L"http"));
  return nullptr;
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::terminalEmulator() const {
  ScopedCom com;
  if (!defaultTerminalIsConhost()) {
    if (auto wt = searchExecutable(L"wt.exe")) return appForExecutable(*wt, QStringLiteral("Terminal"));
  }
  fs::path comspec = L"cmd.exe";
  if (const wchar_t *env = _wgetenv(L"ComSpec"); env && *env) comspec = env;
  return appForExecutable(comspec, QStringLiteral("Command Prompt"));
}

bool WindowsAppDatabase::showInFileBrowser(const fs::path &path, bool select) const {
  std::error_code ec;
  fs::path target = path;
  bool reveal = select;

  if (!fs::exists(target, ec)) {
    target = target.parent_path();
    reveal = false;
    if (target.empty()) return false;
  }

  ScopedCom com;

  if (reveal) {
    const std::wstring params = L"/select,\"" + target.wstring() + L"\"";
    return shellExecuteOpen(L"explorer.exe", params.c_str());
  }

  return shellExecuteOpen(target.wstring());
}

bool WindowsAppDatabase::openLocation(const AbstractApplication &app) const {
  return showInFileBrowser(app.path(), true);
}

WindowsAppDatabase::AppPtr WindowsAppDatabase::locationOpener(const AbstractApplication &app) const {
  if (const auto *winApp = dynamic_cast<const WindowsApplication *>(&app);
      winApp && winApp->isPackaged()) {
    return nullptr; // packaged apps live under the ACL-locked WindowsApps folder
  }
  return fileBrowser();
}
