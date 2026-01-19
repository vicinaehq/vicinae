#pragma once
#include <memory>
#include "common/types.hpp"

class NavigationController;
class CommandController;
class ServiceRegistry;
class OverlayController;
class SettingsController;

/**
 * Global object that can be used to access global vicinae state (services, navigation utils...)
 */
struct ApplicationContext : NonCopyable {
  std::unique_ptr<NavigationController> navigation;
  std::unique_ptr<OverlayController> overlay;
  ServiceRegistry *services;
  std::unique_ptr<SettingsController> settings;
};
