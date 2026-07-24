#pragma once
#include <memory>
#include "services/layout-switch/abstract-layout-switch-service.hpp"

std::unique_ptr<AbstractLayoutSwitchService> createLayoutSwitchService();
