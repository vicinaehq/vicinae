#include "utils.hpp"
#include <cstdlib>

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
