#include "mac-app.hpp"
#include "ui/image/url.hpp"

#import <Foundation/Foundation.h>

#include <unordered_set>

namespace {

NSString *toNSString(const std::filesystem::path &p) {
  return [NSString stringWithUTF8String:p.c_str()];
}

QString toQString(NSString *s) { return s ? QString::fromNSString(s) : QString(); }

const std::unordered_set<std::string> &knownTerminalBundleIds() {
  static const std::unordered_set<std::string> set = {
      "com.apple.Terminal",
      "com.googlecode.iterm2",
      "com.github.wez.wezterm",
      "io.alacritty",
      "net.kovidgoyal.kitty",
      "com.mitchellh.ghostty",
      "dev.warp.Warp-Stable",
  };
  return set;
}

} // namespace

MacApplication::MacApplication(std::filesystem::path bundlePath, QString id, QString displayName,
                               QString executable, bool displayable)
    : m_bundlePath(std::move(bundlePath)), m_id(std::move(id)), m_displayName(std::move(displayName)),
      m_executable(std::move(executable)), m_displayable(displayable) {}

std::shared_ptr<MacApplication> MacApplication::fromBundle(const std::filesystem::path &bundlePath) {
  @autoreleasepool {
    NSString *nsPath = toNSString(bundlePath);
    if (!nsPath) return nullptr;

    NSURL *url = [NSURL fileURLWithPath:nsPath];
    NSBundle *bundle = [NSBundle bundleWithURL:url];
    if (!bundle) return nullptr;

    NSString *bundleId = bundle.bundleIdentifier;
    if (bundleId.length == 0) return nullptr;

    NSDictionary *info = bundle.infoDictionary;

    NSString *displayName = info[@"CFBundleDisplayName"];
    if (displayName.length == 0) displayName = info[@"CFBundleName"];
    if (displayName.length == 0) {
      displayName = [[nsPath lastPathComponent] stringByDeletingPathExtension];
    }

    NSString *executable = info[@"CFBundleExecutable"];

    bool const isUIElement = [info[@"LSUIElement"] boolValue];
    bool const isBackgroundOnly = [info[@"LSBackgroundOnly"] boolValue];
    bool const displayable = !isUIElement && !isBackgroundOnly;

    return std::make_shared<MacApplication>(bundlePath, toQString(bundleId), toQString(displayName),
                                            toQString(executable), displayable);
  }
}

bool MacApplication::isTerminalEmulator() const {
  return knownTerminalBundleIds().contains(m_id.toStdString());
}

ImageURL MacApplication::iconUrl() const { return ImageURL::macBundle(m_bundlePath); }

bool MacApplication::matchesWindowClass(const QString &wmClass) const {
  return m_id.compare(wmClass, Qt::CaseInsensitive) == 0;
}
