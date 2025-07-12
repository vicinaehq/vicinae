#include "utils.hpp"
#include <cstdlib>
#include <filesystem>
#include <qmimedatabase.h>
#include <qmimetype.h>

namespace fs = std::filesystem;

std::filesystem::path homeDir() {
  const char *env = getenv("HOME");

  if (!env) return {};

  return env;
}

fs::path compressPath(const fs::path &path) {
  auto homeStr = homeDir().string();
  auto str = path.string();

  if (str.starts_with(homeStr)) { return "~" + str.substr(homeStr.size()); }

  return path;
}

QString getRelativeTimeString(const QDateTime &pastTime) {
  QDateTime now = QDateTime::currentDateTime();
  qint64 secondsDiff = pastTime.secsTo(now);

  if (secondsDiff < 0) { return QObject::tr("in the future"); }

  qint64 days = secondsDiff / (24 * 3600);
  qint64 hours = secondsDiff / 3600;
  qint64 minutes = secondsDiff / 60;

  if (days >= 365) {
    int years = days / 365;
    return QString("%1 year%2 ago").arg(years).arg(years > 1 ? "s" : "");
  } else if (days >= 30) {
    int months = days / 30;
    return QString("%1 month%2 ago").arg(months).arg(months > 1 ? "s" : "");
  } else if (days >= 1) {
    return QString("%1 day%2 ago").arg(days).arg(days > 1 ? "s" : "");
  } else if (hours >= 1) {
    return QString("%1 hour%2 ago").arg(hours).arg(hours > 1 ? "s" : "");
  } else if (minutes >= 1) {
    return QString("%1 minute%2 ago").arg(minutes).arg(minutes > 1 ? "s" : "");
  } else {
    return "just now";
  }
}

QString qStringFromStdView(std::string_view view) { return QString::fromUtf8(view.data(), view.size()); }

bool isTextMimeType(const QMimeType &mime) {
  QMimeDatabase db;
  QMimeType textPlain = db.mimeTypeForName("text/plain");

  return mime.inherits(textPlain.name());
}

bool isHiddenPath(const std::filesystem::path &path) {
  return std::ranges::any_of(path, [](auto &&path) { return path.string().starts_with('.'); });
}

bool isInHomeDirectory(const std::filesystem::path &path) {
  return path.string().starts_with(homeDir().string());
}

std::vector<fs::path> homeRootDirectories() {
  std::vector<fs::path> paths;
  std::error_code ec;

  for (const auto &entry : fs::directory_iterator(homeDir(), ec)) {
    if (entry.is_directory() && !isHiddenPath(entry.path())) paths.emplace_back(entry.path());
  }

  return paths;
}

std::filesystem::path downloadsFolder() { return homeDir() / "Downloads"; }
std::filesystem::path documentsFolder() { return homeDir() / "Documents"; }

std::string getLastPathComponent(const std::filesystem::path &path) {
  if (!path.has_filename() && path.has_parent_path()) { return path.parent_path().filename(); }

  return path.filename();
}
