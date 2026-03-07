#pragma once
#include "services/app-service/app-service.hpp"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QUrl>
#include <filesystem>
#include <optional>
#include <set>

namespace FileBrowser {

inline bool revealInFileManager(const std::filesystem::path &path) {
  auto const fileUrl = QUrl::fromLocalFile(path.c_str()).toString();
  QDBusInterface iface("org.freedesktop.FileManager1", "/org/freedesktop/FileManager1",
                       "org.freedesktop.FileManager1", QDBusConnection::sessionBus());

  if (!iface.isValid()) { return false; }

  QDBusReply<void> const reply = iface.call("ShowItems", QStringList{fileUrl}, QString{});
  return reply.isValid();
}

inline std::optional<std::filesystem::path> containingFolderTarget(const std::filesystem::path &path) {
  std::error_code ec;

  if (!std::filesystem::exists(path, ec)) {
    auto parentPath = path.parent_path();
    if (parentPath.empty()) { return std::nullopt; }
    return parentPath;
  }

  if (std::filesystem::is_directory(path, ec)) {
    auto parentPath = path.parent_path();
    return parentPath.empty() ? std::optional(path) : std::optional(parentPath);
  }

  return path;
}

inline bool openContainingFolderFallback(const std::filesystem::path &path, AppService *appDb) {
  auto const browser = appDb->fileBrowser();
  if (!browser) { return false; }

  auto target = containingFolderTarget(path);
  if (!target) { return false; }

  // These file managers don't work correctly when they're passed a file path
  // We work around this by passing the parent folder path instead.
  static const std::set<QString> exceptions = {"org.kde.dolphin.desktop", "ranger.desktop"};

  if (exceptions.contains(browser->id())) {
    if (auto parentPath = target->parent_path(); !parentPath.empty() && *target == path) {
      return appDb->launch(*browser, {parentPath.c_str()});
    }
  }

  return appDb->launch(*browser, {target->c_str()});
}

inline bool openInFileBrowser(const std::filesystem::path &path, AppService *appDb) {
  auto const browser = appDb->fileBrowser();
  return browser && appDb->launch(*browser, {path.c_str()});
}

inline bool showInFileBrowser(const std::filesystem::path &path, AppService *appDb, bool select) {
  if (!select) { return openInFileBrowser(path, appDb); }

  std::error_code ec;
  if (std::filesystem::exists(path, ec) && revealInFileManager(path)) { return true; }

  return openContainingFolderFallback(path, appDb);
}

} // namespace FileBrowser
