#include "mac-app-database.hpp"
#include "services/app-service/abstract-app-db.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include <QDebug>
#include <QUrl>
#include <qlogging.h>

#include <fstream>

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

enum class TargetKind { Unknown, Url, Path, Uti };

struct ClassifiedTarget {
  TargetKind kind = TargetKind::Unknown;
  QString normalized;
};

ClassifiedTarget classifyTarget(const QString &target) {
  if (target.isEmpty()) return {};

  if (target.contains(QStringLiteral("://"))) return {TargetKind::Url, target};

  bool const looksLikePath = target.startsWith('/') || target.startsWith('~') ||
                             target.startsWith("./") || target.startsWith("../");

  if (looksLikePath) {
    @autoreleasepool {
      NSString *expanded = [[NSString stringWithCharacters:reinterpret_cast<const unichar *>(target.utf16())
                                                    length:target.size()] stringByExpandingTildeInPath];
      return {TargetKind::Path, QString::fromNSString(expanded)};
    }
  }

  std::error_code ec;
  if (fs::exists(target.toStdString(), ec)) return {TargetKind::Path, target};

  if (target.contains('.') && !target.contains(' ')) return {TargetKind::Uti, target};

  return {};
}

void collectEmbeddedApps(const fs::path &bundle, std::vector<fs::path> &out) {
  std::error_code ec;
  fs::path const embeddedDir = bundle / "Contents" / "Applications";
  if (!fs::is_directory(embeddedDir, ec)) return;

  for (const auto &entry :
       fs::directory_iterator(embeddedDir, fs::directory_options::skip_permission_denied, ec)) {
    if (entry.path().filename().string().ends_with(".app")) { out.emplace_back(entry.path()); }
  }
}

std::vector<fs::path> collectAppPaths(const std::vector<fs::path> &roots) {
  std::vector<fs::path> paths;

  auto addBundle = [&](const fs::path &bundle) {
    paths.emplace_back(bundle);
    collectEmbeddedApps(bundle, paths);
  };

  for (const auto &root : roots) {
    std::error_code ec;
    if (!fs::is_directory(root, ec)) continue;

    fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec);
    fs::recursive_directory_iterator const end;
    if (ec) continue;

    while (it != end) {
      const auto path = it->path();

      if (path.filename().string().ends_with(".app")) {
        addBundle(path);
        it.disable_recursion_pending();
      } else if (it.depth() >= 1) {
        it.disable_recursion_pending();
      }

      it.increment(ec);
      if (ec) ec.clear();
    }
  }

  addBundle("/System/Library/CoreServices/Finder.app");

  return paths;
}

std::string shellQuote(const QString &arg) {
  std::string s = arg.toStdString();
  std::string out = "'";
  out.reserve(s.size() + 2);
  for (char c : s) {
    if (c == '\'') {
      out += "'\\''";
    } else {
      out += c;
    }
  }
  out += '\'';
  return out;
}

} // namespace

MacAppDatabase::MacAppDatabase() { scan(defaultSearchPaths()); }

std::vector<fs::path> MacAppDatabase::defaultSearchPaths() const {
  std::vector<fs::path> paths;
  paths.reserve(4);
  paths.emplace_back("/Applications");
  paths.emplace_back("/System/Applications");
  paths.emplace_back("/System/Library/CoreServices/Applications");
  if (const char *home = std::getenv("HOME")) {
    paths.emplace_back(fs::path(home) / "Applications");
  }
  return paths;
}

bool MacAppDatabase::scan(const std::vector<fs::path> &paths) {
  m_apps.clear();
  m_appsById.clear();

  auto scanned = collectAppPaths(paths);
  m_apps.reserve(scanned.size());

  for (const auto &path : scanned) {
    auto app = MacApplication::fromBundle(path);
    if (!app || m_appsById.contains(app->id())) continue;
    m_appsById.emplace(app->id(), app);
    m_apps.emplace_back(std::move(app));
  }

  return !m_apps.empty();
}

bool MacAppDatabase::launch(const AbstractApplication &app, const std::vector<QString> &args) const {
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
                                           const LaunchTerminalCommandOptions &opts) const {
  if (cmdline.empty()) return false;

  std::string script = "#!/bin/bash\n";

  if (opts.title) {
    script += "printf '\\033]0;%s\\007' ";
    script += shellQuote(*opts.title);
    script += '\n';
  }

  if (opts.workingDirectory) {
    script += "cd ";
    script += shellQuote(*opts.workingDirectory);
    script += " || exit 1\n";
  }

  bool first = true;
  for (const auto &arg : cmdline) {
    if (!first) script += ' ';
    script += shellQuote(arg);
    first = false;
  }
  script += '\n';

  if (opts.hold) { script += "echo; read -n 1 -s -r -p 'Press any key to close...'\n"; }

  fs::path const scriptPath =
      fs::path(QString::fromNSString(NSTemporaryDirectory()).toStdString()) /
      (QString("vicinae-%1.command").arg(QString::fromNSString([[NSUUID UUID] UUIDString])).toStdString());

  {
    std::ofstream ofs(scriptPath);
    if (!ofs) {
      qWarning() << "Failed to write terminal script to" << scriptPath.c_str();
      return false;
    }
    ofs << script;
  }

  std::error_code ec;
  fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add, ec);
  if (ec) {
    qWarning() << "Failed to chmod terminal script:" << ec.message().c_str();
    return false;
  }

  // Pinned to Terminal.app; opts.emulator ignored.
  @autoreleasepool {
    NSURL *scriptURL = [NSURL fileURLWithPath:toNSString(scriptPath)];
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    NSURL *terminalURL = [ws URLForApplicationWithBundleIdentifier:@"com.apple.Terminal"];
    if (!terminalURL) {
      qWarning() << "Terminal.app not found";
      return false;
    }
    NSWorkspaceOpenConfiguration *cfg = [NSWorkspaceOpenConfiguration configuration];
    [ws openURLs:@[ scriptURL ] withApplicationAtURL:terminalURL configuration:cfg completionHandler:nil];
  }

  return true;
}

namespace {

NSArray<NSURL *> *appURLsForTarget(NSWorkspace *ws, const ClassifiedTarget &t) {
  switch (t.kind) {
  case TargetKind::Url: {
    NSURL *url = [NSURL URLWithString:toNSString(t.normalized)];
    return url ? [ws URLsForApplicationsToOpenURL:url] : nil;
  }
  case TargetKind::Path: {
    NSURL *url = [NSURL fileURLWithPath:toNSString(t.normalized)];
    return url ? [ws URLsForApplicationsToOpenURL:url] : nil;
  }
  case TargetKind::Uti: {
    UTType *ut = [UTType typeWithIdentifier:toNSString(t.normalized)];
    return ut ? [ws URLsForApplicationsToOpenContentType:ut] : nil;
  }
  case TargetKind::Unknown:
    return nil;
  }
  return nil;
}

NSURL *defaultAppURLForTarget(NSWorkspace *ws, const ClassifiedTarget &t) {
  switch (t.kind) {
  case TargetKind::Url: {
    NSURL *url = [NSURL URLWithString:toNSString(t.normalized)];
    return url ? [ws URLForApplicationToOpenURL:url] : nil;
  }
  case TargetKind::Path: {
    NSURL *url = [NSURL fileURLWithPath:toNSString(t.normalized)];
    return url ? [ws URLForApplicationToOpenURL:url] : nil;
  }
  case TargetKind::Uti: {
    UTType *ut = [UTType typeWithIdentifier:toNSString(t.normalized)];
    return ut ? [ws URLForApplicationToOpenContentType:ut] : nil;
  }
  case TargetKind::Unknown:
    return nil;
  }
  return nil;
}

} // namespace

std::vector<MacAppDatabase::AppPtr> MacAppDatabase::findOpeners(const Target &target) const {
  auto const classified = classifyTarget(target);
  std::vector<QString> ids;

  @autoreleasepool {
    NSArray<NSURL *> *urls = appURLsForTarget([NSWorkspace sharedWorkspace], classified);
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
  auto const classified = classifyTarget(target);
  QString id;

  @autoreleasepool {
    NSURL *appURL = defaultAppURLForTarget([NSWorkspace sharedWorkspace], classified);
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

MacAppDatabase::AppPtr MacAppDatabase::terminalEmulator() const {
  return findById(QStringLiteral("com.apple.Terminal"));
}

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

bool MacAppDatabase::openLocation(const AbstractApplication &app) const {
  return showInFileBrowser(app.path(), true);
}

AbstractAppDatabase::AppPtr MacAppDatabase::locationOpener(const AbstractApplication &app) const {
  return fileBrowser();
}