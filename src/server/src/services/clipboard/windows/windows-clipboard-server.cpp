#include <QTimer>
#include <qendian.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <appmodel.h>

#include "windows-clipboard-server.hpp"
#include "services/clipboard/clipboard-mime.hpp"

namespace {
constexpr const char *EXCLUDE_FMT =
    R"(application/x-qt-windows-mime;value="ExcludeClipboardContentFromMonitorProcessing")";
constexpr const char *HISTORY_FMT = R"(application/x-qt-windows-mime;value="CanIncludeInClipboardHistory")";
constexpr const char *CLOUD_FMT = R"(application/x-qt-windows-mime;value="CanUploadToCloudClipboard")";
constexpr const char *LEGACY_IGNORE_FMT = R"(application/x-qt-windows-mime;value="Clipboard Viewer Ignore")";

bool isDwordZero(const QByteArray &data) {
  return data.size() >= 4 && qFromLittleEndian<quint32>(data.constData()) == 0;
}
} // namespace

bool WindowsClipboardServer::start() {
  connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, [this]() { handleChange(false); });
  return true;
}

bool WindowsClipboardServer::stop() {
  disconnect(QGuiApplication::clipboard(), nullptr, this, nullptr);
  return true;
}

void WindowsClipboardServer::handleChange(bool isRetry) {
  const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
  // each formats()/hasFormat() call re-enumerates the clipboard
  const QStringList formats = mime ? mime->formats() : QStringList();

  // another process may still hold the clipboard open
  if (formats.isEmpty()) {
    if (!isRetry) {
      QTimer::singleShot(RETRY_DELAY_MS, this, [this]() { handleChange(true); });
    } else {
      qWarning() << "Windows: clipboard still unreadable after retry, selection lost";
    }
    return;
  }

  if (formats.contains(EXCLUDE_FMT) || formats.contains(LEGACY_IGNORE_FMT)) {
    qInfo() << "Windows: dropping selection excluded from monitoring";
    return;
  }
  // CanUploadToCloudClipboard=0 only opts out of cloud sync
  if (formats.contains(HISTORY_FMT) && isDwordZero(mime->data(HISTORY_FMT))) {
    qInfo() << "Windows: dropping selection excluded from clipboard history";
    return;
  }

  auto selection = Clipboard::selectionFromMimeData(mime);
  if (!selection) return;

  selection->sourceApp = clipboardOwnerApp();
  emit selectionAdded(*selection);
}

bool WindowsClipboardServer::writeClipboard(QMimeData *data, const Clipboard::CopyOptions &options) {
  if (options.concealed || options.transient) {
    QByteArray const zero(4, '\0');
    data->setData(EXCLUDE_FMT, zero);
    data->setData(HISTORY_FMT, zero);
    data->setData(CLOUD_FMT, zero);
  }
  return AbstractClipboardServer::writeClipboard(data, options);
}

std::optional<QString> WindowsClipboardServer::clipboardOwnerApp() const {
  // OLE clipboard writes often have no owner window
  HWND hwnd = GetClipboardOwner();
  if (!hwnd) hwnd = GetForegroundWindow();
  if (!hwnd) return std::nullopt;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (!pid) return std::nullopt;

  HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process) return std::nullopt;

  std::optional<QString> result;
  wchar_t aumid[APPLICATION_USER_MODEL_ID_MAX_LENGTH];
  UINT32 aumidSize = APPLICATION_USER_MODEL_ID_MAX_LENGTH;

  if (GetApplicationUserModelId(process, &aumidSize, aumid) == ERROR_SUCCESS && aumidSize > 1) {
    result = QString::fromWCharArray(aumid, aumidSize - 1);
  } else {
    constexpr DWORD PATH_BUFFER_LEN = 8192;
    wchar_t path[PATH_BUFFER_LEN];
    DWORD size = PATH_BUFFER_LEN;
    if (QueryFullProcessImageNameW(process, 0, path, &size) && size > 0) {
      result = QString::fromWCharArray(path, size);
    }
  }

  CloseHandle(process);
  return result;
}
