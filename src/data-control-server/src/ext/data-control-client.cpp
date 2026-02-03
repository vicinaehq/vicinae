#include "data-control-client.hpp"
#include "ext-data-control-v1-client-protocol.h"

ExtDataControlManager::ExtDataControlManager(ext_data_control_manager_v1 *manager) : _manager(manager) {}

std::unique_ptr<ExtDataDevice> ExtDataControlManager::getDataDevice(const WaylandSeat &seat) {
  return std::make_unique<ExtDataDevice>(ext_data_control_manager_v1_get_data_device(_manager, seat.data()));
}
