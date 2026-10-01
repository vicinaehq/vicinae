#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <set>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <utility>
#include "services/screenshots/macos/macos-screenshot-provider.hpp"

namespace {

QString screenshotDirectory() {
  CFPropertyListRef value = CFPreferencesCopyAppValue(CFSTR("location"), CFSTR("com.apple.screencapture"));
  NSString *location = nil;
  if (value) {
    if (CFGetTypeID(value) == CFStringGetTypeID()) location = [(__bridge NSString *)value copy];
    CFRelease(value);
  }
  if (location.length > 0) return QString::fromNSString(location.stringByExpandingTildeInPath);
  return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

id captureAttribute(const QString &path, const char *name) {
  std::array<char, 1024> buffer;
  const auto length = getxattr(QFile::encodeName(path).constData(), name, buffer.data(), buffer.size(), 0, 0);
  if (length <= 0) return nil;
  NSData *data = [NSData dataWithBytes:buffer.data() length:static_cast<NSUInteger>(length)];
  return [NSPropertyListSerialization propertyListWithData:data
                                                   options:NSPropertyListImmutable
                                                    format:nil
                                                     error:nil];
}

bool hasCaptureAttribute(const QString &path) {
  id image = captureAttribute(path, "com.apple.metadata:kMDItemIsScreenCapture");
  if ([image isKindOfClass:NSNumber.class] && [image boolValue]) return true;
  id type = captureAttribute(path, "com.apple.metadata:kMDItemScreenCaptureType");
  return [type isKindOfClass:NSString.class] && [type length] > 0;
}

ScreenshotResult readFiles(std::vector<Screenshot> candidates, const QString &directory, bool queryFailed) {
  @autoreleasepool {
    ScreenshotResult result;
    QDir folder(directory);
    const auto files = folder.entryInfoList(QDir::Files | QDir::Readable | QDir::NoSymLinks);
    candidates.reserve(candidates.size() + files.size());
    for (const auto &file : files) {
      if (hasCaptureAttribute(file.absoluteFilePath())) {
        candidates.emplace_back(Screenshot{.path = file.absoluteFilePath().toStdString()});
      }
    }

    std::set<std::pair<dev_t, ino_t>> seen;
    result.items.reserve(candidates.size());
    const QMimeDatabase mimeDb;
    for (auto &item : candidates) {
      const auto path = QString::fromStdString(item.path.string());
      const QFileInfo file(path);
      if (!file.isFile() || !file.isReadable() || file.size() == 0) continue;
      struct stat identity;
      if (stat(QFile::encodeName(path).constData(), &identity) != 0 ||
          !seen.emplace(identity.st_dev, identity.st_ino).second)
        continue;
      if (mimeDb.mimeTypeForFile(file).name().startsWith("video/")) {
        item.kind = Screenshot::Kind::Recording;
        NSMetadataItem *metadata =
            [[NSMetadataItem alloc] initWithURL:[NSURL fileURLWithPath:path.toNSString()]];
        id duration = [metadata valueForAttribute:@"kMDItemDurationSeconds"];
        if ([duration isKindOfClass:NSNumber.class]) {
          const double seconds = [duration doubleValue];
          if (std::isfinite(seconds) && seconds >= 0 &&
              seconds < static_cast<double>(std::numeric_limits<qint64>::max()))
            item.durationSeconds = seconds;
        }
      } else {
        QImageReader reader(path);
        item.size = reader.size();
        if (!item.size.isValid()) continue;
      }
      if (!item.createdAt.isValid()) item.createdAt = file.birthTime();
      if (!item.createdAt.isValid()) item.createdAt = file.lastModified();
      result.items.emplace_back(std::move(item));
    }
    std::ranges::sort(result.items, [](const Screenshot &a, const Screenshot &b) {
      if (a.createdAt != b.createdAt) return a.createdAt > b.createdAt;
      return a.path < b.path;
    });
    if (queryFailed) {
      result.error = QCoreApplication::translate(
          "MacosScreenshotProvider",
          "Spotlight is unavailable. Showing screenshots and recordings from the screenshot folder.");
    }
    return result;
  }
}

} // namespace

void MacosScreenshotProvider::refresh() {
  QtConcurrent::run([] {
    @autoreleasepool {
      const auto directory = screenshotDirectory();
      MDQueryRef query = MDQueryCreate(
          kCFAllocatorDefault, CFSTR("kMDItemIsScreenCapture == 1 || kMDItemScreenCaptureType == '*'"),
          nullptr, nullptr);
      if (!query) return readFiles({}, directory, true);

      NSArray *scopes = @[ (__bridge NSString *)kMDQueryScopeHome, directory.toNSString() ];
      MDQuerySetSearchScope(query, (__bridge CFArrayRef)scopes, 0);
      const bool succeeded = MDQueryExecute(query, kMDQuerySynchronous);
      std::vector<Screenshot> candidates;
      if (succeeded) {
        const auto count = MDQueryGetResultCount(query);
        candidates.reserve(count);
        for (CFIndex i = 0; i < count; ++i) {
          const auto item = (MDItemRef)MDQueryGetResultAtIndex(query, i);
          NSString *path = CFBridgingRelease(MDItemCopyAttribute(item, kMDItemPath));
          if (![path isKindOfClass:NSString.class]) continue;
          NSDate *date = CFBridgingRelease(MDItemCopyAttribute(item, kMDItemContentCreationDate));
          QDateTime createdAt;
          if ([date isKindOfClass:NSDate.class]) {
            createdAt =
                QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(date.timeIntervalSince1970 * 1000));
          }
          candidates.emplace_back(Screenshot{.path = path.fileSystemRepresentation, .createdAt = createdAt});
        }
      }
      CFRelease(query);
      return readFiles(std::move(candidates), directory, !succeeded);
    }
  }).then(this, [this](ScreenshotResult result) { emit refreshed(std::move(result)); });
}
