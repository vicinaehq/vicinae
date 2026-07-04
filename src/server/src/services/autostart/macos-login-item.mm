#include "macos-login-item.hpp"
#include "vicinae.hpp"
#include <filesystem>
#include <fstream>
#include <qlogging.h>
#import <Foundation/Foundation.h>
#import <ServiceManagement/ServiceManagement.h>

namespace vicinae::macos {

// we only set it once, otherwise we would end up overriding the user preference
// evert single time
void registerLoginItemOnce() {
  if (!NSBundle.mainBundle.bundleIdentifier) return;

  const auto marker = Omnicast::stateDir() / "login-item-registered";
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

} // namespace vicinae::macos
