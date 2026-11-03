#include "trash.hpp"

#if !defined(Q_OS_MACOS)

#include <qcoreapplication.h>
#include <qdesktopservices.h>
#include <qurl.h>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <shellapi.h>

namespace SystemTrash {

bool emptyTrash(std::string &error) {
  HRESULT hr = SHEmptyRecycleBinW(nullptr, nullptr, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI);
  if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) &&
      hr != static_cast<HRESULT>(0x80004005)) {
    error = "Failed to empty recycle bin";
    return false;
  }
  return true;
}

bool openTrash(std::string &error) {
  if (!QDesktopServices::openUrl(QUrl("shell:RecycleBinFolder"))) {
    error = "Failed to open recycle bin";
    return false;
  }
  return true;
}

} // namespace SystemTrash

#elif defined(Q_OS_LINUX)
#include <cstdlib>
#include <filesystem>
#include <qdir.h>
#include <qprocess.h>

namespace fs = std::filesystem;

namespace SystemTrash {

bool emptyTrash(std::string &error) {
  QProcess process;
  process.start("gio", {"trash", "--empty"});
  if (process.waitForFinished(5000) && process.exitCode() == 0) { return true; }

  const char *dataHomeEnv = std::getenv("XDG_DATA_HOME");
  fs::path trashPath = dataHomeEnv ? fs::path(dataHomeEnv) / "Trash"
                                   : fs::path(QDir::homePath().toStdString()) / ".local/share/Trash";

  std::error_code ec;
  bool cleared = false;
  for (const auto *subdir : {"files", "info"}) {
    fs::path target = trashPath / subdir;
    if (fs::exists(target, ec)) {
      for (const auto &entry : fs::directory_iterator(target, ec)) {
        fs::remove_all(entry.path(), ec);
        cleared = true;
      }
    }
  }

  if (ec && !cleared) {
    error = ec.message();
    return false;
  }
  return true;
}

bool openTrash(std::string &error) {
  if (!QDesktopServices::openUrl(QUrl("trash:///"))) {
    error = "Failed to open trash";
    return false;
  }
  return true;
}

} // namespace SystemTrash

#else

namespace SystemTrash {

bool emptyTrash(std::string &error) {
  error = "Not supported on this platform";
  return false;
}

bool openTrash(std::string &error) {
  error = "Not supported on this platform";
  return false;
}

} // namespace SystemTrash

#endif

#endif
