#pragma once
#include <memory>
#include "services/tray-host/abstract-tray-host.hpp"

std::unique_ptr<AbstractTrayHost> createTrayHost();
