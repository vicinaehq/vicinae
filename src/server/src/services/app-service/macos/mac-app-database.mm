#include "mac-app-database.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include <QDebug>
#include <QUrl>
#include <qlogging.h>

namespace fs = std::filesystem;

namespace {

NSString *toNSString(const QString &s) {
  return [NSString stringWithCharacters:reinterpret_cast<const unichar *>(s.utf16()) length:s.size()];
}

NSString *toNSString(const fs::path &p) { return [NSString stringWithUTF8String:p.c_str()]; }

QString bundleIdentifierAt(NSURL *url) {
  if (!url) return {};
  NSBundle *b = [NSBundle bundleWithURL:url];
  return (b && b.bundleIdentifier) ? QString::fromNSString(b.bundleIdentifier) : QString();
}

bool looksLikePath(const QString &target) {
  return target.startsWith('/') || target.startsWith('~') || target.startsWith("./") ||
         target.startsWith("../");
}

bool looksLikeUTI(const QString &target) {
  if (target.isEmpty() || target.contains('/') || target.contains(' ')) return false;
  return target.contains('.');
}

} // namespace

MacAppDatabase::MacAppDatabase() { scan(defaultSearchPaths()); }

std::vector<fs::path> MacAppDatabase::defaultSearchPaths() const {
  std::vector<fs::path> paths;
  paths.reserve(5);

  paths.emplace_back("/Applications");
  paths.emplace_back("/Applications/Utilities");
  paths.emplace_back("/System/Applications");
  paths.emplace_back("/System/Applications/Utilities");

  if (const char *home = std::getenv("HOME")) {
    paths.emplace_back(fs::path(home) / "Applications");
  }

  return paths;
}

bool MacAppDatabase::scan(const std::vector<fs::path> &paths) {
  m_apps.clear();
  m_appsById.clear();

  auto add = [&](std::shared_ptr<MacApplication> app) {
    if (!app || m_appsById.contains(app->id())) return;
    m_appsById.emplace(app->id(), app);
    m_apps.emplace_back(std::move(app));
  };

  for (const auto &dir : paths) {
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) continue;

    for (const auto &entry : fs::directory_iterator(dir, fs::directory_options::skip_permission_denied, ec)) {
      if (!entry.path().filename().string().ends_with(".app")) continue;
      add(MacApplication::fromBundle(entry.path()));
    }
  }

  add(MacApplication::fromBundle("/System/Library/CoreServices/Finder.app"));

  return true;
}

bool MacAppDatabase::launch(const AbstractApplication &app, const std::vector<QString> &args,
                            const std::optional<QString> &launchPrefix) const {
  (void)launchPrefix;

  fs::path const bundlePath = app.path();
  std::error_code ec;
  if (!fs::exists(bundlePath, ec)) {
    qWarning() << "Bundle does not exist:" << bundlePath.c_str();
    return false;
  }

  @autoreleasepool {
    NSURL *bundleURL = [NSURL fileURLWithPath:toNSString(bundlePath)];
    if (!bundleURL) return false;

    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    NSWorkspaceOpenConfiguration *cfg = [NSWorkspaceOpenConfiguration configuration];

    if (args.empty()) {
      [ws openApplicationAtURL:bundleURL configuration:cfg completionHandler:nil];
    } else {
      NSMutableArray<NSURL *> *urls = [NSMutableArray arrayWithCapacity:args.size()];
      for (const QString &arg : args) {
        QUrl const q(arg);
        NSURL *u = nil;
        if (!q.scheme().isEmpty() && q.scheme() != QStringLiteral("file")) {
          u = [NSURL URLWithString:toNSString(arg)];
        } else {
          QString const p = q.isLocalFile() ? q.toLocalFile() : arg;
          u = [NSURL fileURLWithPath:toNSString(p)];
        }
        if (u) [urls addObject:u];
      }
      [ws openURLs:urls withApplicationAtURL:bundleURL configuration:cfg completionHandler:nil];
    }
  }
  return true;
}

bool MacAppDatabase::launchTerminalCommand(const std::vector<QString> &cmdline,
                                           const LaunchTerminalCommandOptions &opts,
                                           const std::optional<QString> &prefix) const {
  (void)cmdline;
  (void)opts;
  (void)prefix;
  return false;
}

std::vector<MacAppDatabase::AppPtr> MacAppDatabase::findOpeners(const Target &target) const {
  std::vector<QString> ids;

  @autoreleasepool {
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    NSArray<NSURL *> *urls = nil;

    if (target.contains(QStringLiteral("://"))) {
      NSURL *url = [NSURL URLWithString:toNSString(target)];
      if (url) urls = [ws URLsForApplicationsToOpenURL:url];
    } else if (looksLikePath(target)) {
      NSURL *url = [NSURL fileURLWithPath:toNSString(target)];
      urls = [ws URLsForApplicationsToOpenURL:url];
    } else if (looksLikeUTI(target)) {
      UTType *ut = [UTType typeWithIdentifier:toNSString(target)];
      if (ut) urls = [ws URLsForApplicationsToOpenContentType:ut];
    }

    if (urls) {
      ids.reserve(urls.count);
      for (NSURL *u in urls) {
        auto id = bundleIdentifierAt(u);
        if (!id.isEmpty()) ids.emplace_back(std::move(id));
      }
    }
  }

  std::vector<AppPtr> result;
  result.reserve(ids.size());
  for (const auto &id : ids) {
    if (auto app = findById(id)) result.emplace_back(std::move(app));
  }
  return result;
}

MacAppDatabase::AppPtr MacAppDatabase::findDefaultOpener(const Target &target) const {
  QString id;

  @autoreleasepool {
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    NSURL *appURL = nil;

    if (target.contains(QStringLiteral("://"))) {
      NSURL *url = [NSURL URLWithString:toNSString(target)];
      if (url) appURL = [ws URLForApplicationToOpenURL:url];
    } else if (looksLikePath(target)) {
      NSURL *url = [NSURL fileURLWithPath:toNSString(target)];
      appURL = [ws URLForApplicationToOpenURL:url];
    } else if (looksLikeUTI(target)) {
      UTType *ut = [UTType typeWithIdentifier:toNSString(target)];
      if (ut) appURL = [ws URLForApplicationToOpenContentType:ut];
    }

    id = bundleIdentifierAt(appURL);
  }

  return id.isEmpty() ? nullptr : findById(id);
}

MacAppDatabase::AppPtr MacAppDatabase::findById(const QString &id) const {
  if (auto it = m_appsById.find(id); it != m_appsById.end()) return it->second;
  return nullptr;
}

std::vector<MacAppDatabase::AppPtr> MacAppDatabase::list() const { return {m_apps.begin(), m_apps.end()}; }

MacAppDatabase::AppPtr MacAppDatabase::findByClass(const QString &name) const {
  if (auto direct = findById(name)) return direct;
  for (const auto &app : m_apps) {
    if (app->matchesWindowClass(name)) return app;
  }
  return nullptr;
}

MacAppDatabase::AppPtr MacAppDatabase::fileBrowser() const {
  return findById(QStringLiteral("com.apple.finder"));
}

MacAppDatabase::AppPtr MacAppDatabase::genericTextEditor() const {
  QString id;
  @autoreleasepool {
    NSURL *appURL = [[NSWorkspace sharedWorkspace] URLForApplicationToOpenContentType:UTTypePlainText];
    id = bundleIdentifierAt(appURL);
  }
  return id.isEmpty() ? nullptr : findById(id);
}

MacAppDatabase::AppPtr MacAppDatabase::webBrowser() const {
  QString id;
  @autoreleasepool {
    NSURL *appURL = [[NSWorkspace sharedWorkspace] URLForApplicationToOpenContentType:UTTypeHTML];
    id = bundleIdentifierAt(appURL);
  }
  return id.isEmpty() ? nullptr : findById(id);
}

MacAppDatabase::AppPtr MacAppDatabase::terminalEmulator() const { return nullptr; }

bool MacAppDatabase::showInFileBrowser(const fs::path &path, bool select) const {
  std::error_code ec;
  if (!fs::exists(path, ec)) {
    auto parent = path.parent_path();
    if (parent.empty() || parent == path) return false;
    return showInFileBrowser(parent, false);
  }

  @autoreleasepool {
    NSURL *url = [NSURL fileURLWithPath:toNSString(path)];
    if (!url) return false;

    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    if (select) {
      [ws activateFileViewerSelectingURLs:@[ url ]];
    } else {
      [ws openURL:url];
    }
  }
  return true;
}

