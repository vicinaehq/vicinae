#include "macos-login-item.hpp"
#include "vicinae.hpp"
#include <filesystem>
#include <fstream>
#include <qlogging.h>
#import <Foundation/Foundation.h>
#import <ServiceManagement/ServiceManagement.h>

namespace vicinae::macos {

namespace {

std::filesystem::path markerPath() { return Omnicast::stateDir() / "login-item-registered"; }

} // namespace

// we only set it once, otherwise we would end up overriding the user preference
// every single time
void registerLoginItemOnce() {
  if (!NSBundle.mainBundle.bundleIdentifier) return;

  const auto marker = markerPath();
  if (std::filesystem::exists(marker)) return;

  SMAppService *service = SMAppService.mainAppService;

  if (service.status != SMAppServiceStatusEnabled) {
    NSError *error = nil;
    if (![service registerAndReturnError:&error]) {
      qWarning() << "Failed to register login item:" << error.localizedDescription.UTF8String;
      return;
    }
  }

  std::ofstream{marker};
}

bool isLoginItemSupported() { return NSBundle.mainBundle.bundleIdentifier != nil; }

bool isLoginItemEnabled() {
  if (!isLoginItemSupported()) return false;
  return SMAppService.mainAppService.status == SMAppServiceStatusEnabled;
}

void setLoginItemEnabled(bool enabled) {
  if (!isLoginItemSupported()) return;

  SMAppService *service = SMAppService.mainAppService;
  const bool currentlyEnabled = service.status == SMAppServiceStatusEnabled;

  if (enabled != currentlyEnabled) {
    NSError *error = nil;
    const BOOL ok = enabled ? [service registerAndReturnError:&error] : [service unregisterAndReturnError:&error];
    if (!ok) {
      qWarning() << "Failed to" << (enabled ? "register" : "unregister")
                 << "login item:" << error.localizedDescription.UTF8String;
      return;
    }
  }

  std::ofstream{markerPath()};
}

} // namespace vicinae::macos
