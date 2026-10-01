#include "xbel-recent-files-provider.hpp"
#include <QMimeDatabase>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <functional>
#include <qlogging.h>
#include <system_error>
#include <xdgpp/bookmark.hpp>

namespace fs = std::filesystem;

namespace {

constexpr auto APP_NAME = "vicinae";
constexpr auto APP_EXEC = "'vicinae %u'";

bool matchesCategory(const fs::path &path, const RecentFilesParams &params) {
  if (!params.category) return true;
  std::error_code ec;
  return vicinae::fileCategoryFor(path, fs::is_directory(path, ec)) == *params.category;
}

std::vector<fs::path> listRecent(const fs::path &xbelPath, const RecentFilesParams &params) {
  std::vector<fs::path> result;
  if (params.limit <= 0) return result;

  auto file = xdgpp::BookmarkFile::fromFile(xbelPath);
  if (!file) return result;

  auto bookmarks = file->bookmarks();
  std::ranges::stable_sort(bookmarks, std::ranges::greater{}, &xdgpp::Bookmark::lastUsed);

  std::error_code ec;
  for (auto const &bookmark : bookmarks) {
    if (std::ssize(result) >= params.limit) break;
    if (bookmark.isPrivate) continue;
    auto path = xdgpp::fromFileUri(bookmark.href);
    if (!path || !fs::exists(*path, ec) || !matchesCategory(*path, params)) continue;
    result.emplace_back(std::move(*path));
  }

  return result;
}

} // namespace

XbelRecentFilesProvider::XbelRecentFilesProvider() : m_xbelPath(xdgpp::recentlyUsedFilePath()) {}

bool XbelRecentFilesProvider::isAvailable() const { return true; }

QFuture<std::vector<fs::path>> XbelRecentFilesProvider::listAsync(const RecentFilesParams &params) {
  return QtConcurrent::run([path = m_xbelPath, params] { return listRecent(path, params); });
}

void XbelRecentFilesProvider::recordAccess(const fs::path &path) {
  auto file = xdgpp::BookmarkFile::fromFile(m_xbelPath);

  if (!file) {
    qWarning() << "Not recording recent file access, failed to parse" << m_xbelPath.c_str()
               << file.error().c_str();
    return;
  }

  auto const href = xdgpp::toFileUri(path);
  auto const mime = QMimeDatabase().mimeTypeForFile(QString::fromStdString(path.string())).name();

  file->addApplication(href, APP_NAME, APP_EXEC);
  file->setMimeType(href, mime.toStdString());

  if (!file->save(m_xbelPath)) {
    qWarning() << "Failed to record recent file access in" << m_xbelPath.c_str();
  }
}
