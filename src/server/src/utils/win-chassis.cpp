#include "utils/win-chassis.hpp"
#include <windows.h>
#include <powerbase.h>

namespace vicinae::win {

std::string chassisType() {
  switch (PowerDeterminePlatformRoleEx(POWER_PLATFORM_ROLE_V2)) {
  case PlatformRoleDesktop:
  case PlatformRoleWorkstation:
    return "desktop";
  case PlatformRoleMobile:
  case PlatformRoleSlate:
    return "laptop";
  case PlatformRoleUnspecified:
    return "unknown";
  default:
    return "other";
  }
}

} // namespace vicinae::win
