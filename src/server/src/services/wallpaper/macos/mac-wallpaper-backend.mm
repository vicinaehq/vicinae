#include "mac-wallpaper-backend.hpp"
#import <AppKit/AppKit.h>

namespace {

NSImageScaling scalingForFit(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Cover:
  case WallpaperFit::Contain:
    return NSImageScaleProportionallyUpOrDown;
  case WallpaperFit::Stretch:
    return NSImageScaleAxesIndependently;
  case WallpaperFit::Center:
  case WallpaperFit::Tile: // no tiling through this API; centering is the closest
    return NSImageScaleNone;
  }
  return NSImageScaleProportionallyUpOrDown;
}

std::expected<void, std::string> apply(const WallpaperRequest &request) {
  @autoreleasepool {
    NSString *path = [NSString stringWithUTF8String:request.path.c_str()];
    NSURL *url = path ? [NSURL fileURLWithPath:path] : nil;
    if (!url) return std::unexpected("invalid wallpaper path");

    NSDictionary *options = @{
      NSWorkspaceDesktopImageScalingKey : @(scalingForFit(request.fit)),
      NSWorkspaceDesktopImageAllowClippingKey : @(request.fit == WallpaperFit::Cover),
    };

    NSArray<NSScreen *> *targets = [NSScreen screens];

    // request.screen -> NSScreen.localizedName
    if (request.screen) {
      NSString *wanted = [NSString stringWithUTF8String:request.screen->c_str()];
      NSMutableArray<NSScreen *> *matched = [NSMutableArray array];
      for (NSScreen *screen in targets) {
        if (wanted && [screen.localizedName isEqualToString:wanted]) { [matched addObject:screen]; }
      }
      if (matched.count > 0) targets = matched;
    }

    NSWorkspace *workspace = [NSWorkspace sharedWorkspace];

    for (NSScreen *screen in targets) {
      NSError *error = nil;
      if (![workspace setDesktopImageURL:url forScreen:screen options:options error:&error]) {
        NSString *message = error.localizedDescription ?: @"failed to set wallpaper";
        return std::unexpected(std::string(message.UTF8String));
      }
    }

    return {};
  }
}

} // namespace

QFuture<std::expected<void, std::string>>
MacWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  return QtFuture::makeReadyValueFuture(apply(request));
}
