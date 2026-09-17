#include <Foundation/Foundation.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFutureWatcher>
#include <QImageReader>
#include <QMimeDatabase>
#include <QPointer>
#include <QStandardPaths>
#include <QTimer>
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
#include "services/screenshots/macos/screenshot-files.hpp"

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

} // namespace

ScreenshotResult MacosScreenshots::readFiles(std::vector<Screenshot> candidates, const QString &directory,
                                             bool queryFailed) {
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

struct MacosScreenshotProvider::Impl {
  NSMetadataQuery *query = [[NSMetadataQuery alloc] init];
  id gathered = nil;
  id updated = nil;
  QFileSystemWatcher folders;
  QTimer debounce;
  QTimer settle;
  QTimer timeout;
  QFutureWatcher<ScreenshotResult> scan;
  QString directory;
  bool started = false;
  bool ready = false;
  bool queryFailed = false;
  bool scanning = false;
  bool pending = false;

  ~Impl() {
    [NSNotificationCenter.defaultCenter removeObserver:gathered];
    [NSNotificationCenter.defaultCenter removeObserver:updated];
    [query stopQuery];
  }
};

MacosScreenshotProvider::MacosScreenshotProvider(QObject *parent)
    : AbstractScreenshotProvider(parent), m_impl(std::make_unique<Impl>()) {
  auto &state = *m_impl;
  state.debounce.setSingleShot(true);
  state.debounce.setInterval(300);
  state.settle.setSingleShot(true);
  state.settle.setInterval(1500);
  state.timeout.setSingleShot(true);
  state.timeout.setInterval(5000);
  connect(&state.folders, &QFileSystemWatcher::directoryChanged, this, [this] {
    m_impl->debounce.start();
    // The directory notification can arrive before image data and metadata have finished writing.
    m_impl->settle.start();
  });
  connect(&state.debounce, &QTimer::timeout, this, &MacosScreenshotProvider::refresh);
  connect(&state.settle, &QTimer::timeout, this, &MacosScreenshotProvider::refresh);
  connect(&state.timeout, &QTimer::timeout, this, [this] {
    m_impl->queryFailed = true;
    collect();
  });
  connect(&state.scan, &QFutureWatcher<ScreenshotResult>::finished, this, [this] {
    m_impl->scanning = false;
    if (std::exchange(m_impl->pending, false)) {
      collect();
      return;
    }
    emit refreshed(m_impl->scan.result());
  });

  state.query.predicate =
      [NSPredicate predicateWithFormat:@"kMDItemIsScreenCapture == 1 OR kMDItemScreenCaptureType LIKE '*'"];
  state.query.sortDescriptors = @[ [NSSortDescriptor sortDescriptorWithKey:@"kMDItemContentCreationDate"
                                                                 ascending:NO] ];
  const QPointer<MacosScreenshotProvider> guard(this);
  state.gathered =
      [NSNotificationCenter.defaultCenter addObserverForName:NSMetadataQueryDidFinishGatheringNotification
                                                      object:state.query
                                                       queue:NSOperationQueue.mainQueue
                                                  usingBlock:^(NSNotification *) {
                                                    if (!guard) return;
                                                    guard->m_impl->ready = true;
                                                    guard->m_impl->queryFailed = false;
                                                    guard->m_impl->timeout.stop();
                                                    guard->collect();
                                                  }];
  state.updated = [NSNotificationCenter.defaultCenter addObserverForName:NSMetadataQueryDidUpdateNotification
                                                                  object:state.query
                                                                   queue:NSOperationQueue.mainQueue
                                                              usingBlock:^(NSNotification *) {
                                                                if (guard) guard->m_impl->debounce.start();
                                                              }];
}

MacosScreenshotProvider::~MacosScreenshotProvider() = default;

void MacosScreenshotProvider::refresh() {
  auto &state = *m_impl;
  const auto directory = screenshotDirectory();
  if (state.directory != directory) {
    if (!state.folders.directories().isEmpty()) state.folders.removePaths(state.folders.directories());
    state.directory = directory;
    [state.query stopQuery];
    state.query.searchScopes = @[ NSMetadataQueryUserHomeScope, directory.toNSString() ];
    state.started = false;
    state.ready = false;
  }
  if (!state.folders.directories().contains(directory) && QFileInfo(directory).isDir()) {
    state.folders.addPath(directory);
  }
  if (!state.started) {
    state.started = [state.query startQuery];
    state.queryFailed = !state.started;
    if (state.started) state.timeout.start();
  }
  if (state.ready || state.queryFailed) collect();
}

void MacosScreenshotProvider::collect() {
  auto &state = *m_impl;
  if (state.scanning) {
    state.pending = true;
    return;
  }
  std::vector<Screenshot> candidates;
  [state.query disableUpdates];
  candidates.reserve(state.query.resultCount);
  for (NSMetadataItem *item in state.query.results) {
    NSString *path = [item valueForAttribute:NSMetadataItemPathKey];
    if (![path isKindOfClass:NSString.class]) continue;
    NSDate *date = [item valueForAttribute:@"kMDItemContentCreationDate"];
    QDateTime createdAt;
    if ([date isKindOfClass:NSDate.class]) {
      createdAt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(date.timeIntervalSince1970 * 1000));
    }
    candidates.emplace_back(Screenshot{.path = path.fileSystemRepresentation, .createdAt = createdAt});
  }
  [state.query enableUpdates];
  state.scanning = true;
  state.scan.setFuture(QtConcurrent::run(
      [items = std::move(candidates), directory = state.directory, failed = state.queryFailed]() mutable {
        return MacosScreenshots::readFiles(std::move(items), directory, failed);
      }));
}
