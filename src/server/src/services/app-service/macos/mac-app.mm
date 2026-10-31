#include "mac-app.hpp"
#include "ui/image/url.hpp"

#import <Foundation/Foundation.h>

#include <system_error>
#include <unordered_set>

namespace {

NSString *toNSString(const std::filesystem::path &p) { return [NSString stringWithUTF8String:p.c_str()]; }

QString toQString(NSString *s) { return s ? QString::fromNSString(s) : QString(); }

NSString *finderDisplayName(NSString *path) {
  NSString *name = [[NSFileManager defaultManager] displayNameAtPath:path];
  if ([[name pathExtension] caseInsensitiveCompare:@"app"] == NSOrderedSame) {
    name = [name stringByDeletingPathExtension];
  }
  return [name stringByReplacingOccurrencesOfString:@"\u00AD" withString:@""];
}

const std::unordered_set<std::string> &knownTerminalBundleIds() {
  static const std::unordered_set<std::string> set = {
      "com.apple.Terminal",   "com.googlecode.iterm2", "com.github.wez.wezterm", "io.alacritty",
      "net.kovidgoyal.kitty", "com.mitchellh.ghostty", "dev.warp.Warp-Stable",
  };
  return set;
}

} // namespace

MacApplication::MacApplication(std::filesystem::path bundlePath, QString id,
                               std::optional<QString> bundleIdentifier, QString displayName,
                               std::optional<QString> unlocalizedName, QString executable)
    : m_bundlePath(std::move(bundlePath)), m_id(std::move(id)),
      m_bundleIdentifier(std::move(bundleIdentifier)), m_displayName(std::move(displayName)),
      m_unlocalizedName(std::move(unlocalizedName)), m_executable(std::move(executable)) {}

std::shared_ptr<MacApplication> MacApplication::fromBundle(const std::filesystem::path &bundlePath) {
  @autoreleasepool {
    NSString *nsPath = toNSString(bundlePath);
    if (!nsPath) return nullptr;

    // NSBundle and CFBundleCreate cache bundles per path for the process lifetime, so an app
    // scanned while still being copied would stay invisible until restart.
    NSURL *url = [NSURL fileURLWithPath:nsPath];
    NSDictionary *info = CFBridgingRelease(CFBundleCopyInfoDictionaryInDirectory((__bridge CFURLRef)url));
    if (!info) return nullptr;

    NSString *bundleId = info[@"CFBundleIdentifier"];
    NSString *executable = info[@"CFBundleExecutable"];
    std::optional<QString> bundleIdentifier;
    QString id;

    if (bundleId.length > 0) {
      bundleIdentifier = toQString(bundleId);
      id = *bundleIdentifier;
    } else {
      if (executable.length == 0) return nullptr;
      NSString *executablePath = [nsPath
          stringByAppendingPathComponent:[@"Contents/MacOS" stringByAppendingPathComponent:executable]];
      if (![[NSFileManager defaultManager] isExecutableFileAtPath:executablePath]) return nullptr;

      std::error_code error;
      auto const canonicalPath = std::filesystem::canonical(bundlePath, error);
      if (error) return nullptr;
      id = QStringLiteral("macos:path:") + QString::fromStdString(canonicalPath.string());
    }

    NSString *displayName = finderDisplayName(nsPath);
    if (displayName.length == 0) displayName = info[@"CFBundleDisplayName"];
    if (displayName.length == 0) displayName = info[@"CFBundleName"];
    if (displayName.length == 0) { displayName = [[nsPath lastPathComponent] stringByDeletingPathExtension]; }

    NSString *unlocalizedName = info[@"CFBundleDisplayName"];
    if (unlocalizedName.length == 0) unlocalizedName = info[@"CFBundleName"];
    std::optional<QString> unlocalized;
    if (unlocalizedName.length > 0 && ![unlocalizedName isEqualToString:displayName]) {
      unlocalized = toQString(unlocalizedName);
    }

    return std::make_shared<MacApplication>(bundlePath, std::move(id), std::move(bundleIdentifier),
                                            toQString(displayName), std::move(unlocalized),
                                            toQString(executable));
  }
}

bool MacApplication::isTerminalEmulator() const {
  return knownTerminalBundleIds().contains(m_id.toStdString());
}

ImageURL MacApplication::iconUrl() const {
  if (!m_iconPath) {
    std::error_code error;
    std::filesystem::path const resolvedPath = std::filesystem::canonical(m_bundlePath, error);
    m_iconPath = error ? m_bundlePath : resolvedPath;
  }
  return ImageURL::macBundle(*m_iconPath);
}

bool MacApplication::matchesWindowClass(const QString &wmClass) const {
  return m_bundleIdentifier && m_bundleIdentifier->compare(wmClass, Qt::CaseInsensitive) == 0;
}
