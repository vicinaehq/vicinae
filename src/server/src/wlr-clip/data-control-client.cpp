#include "data-control-client.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"

DataControlManager::DataControlManager(zwlr_data_control_manager_v1 *manager) : _manager(manager) {}

std::unique_ptr<DataDevice> DataControlManager::getDataDevice(const WaylandSeat &seat) {
  return std::make_unique<DataDevice>(zwlr_data_control_manager_v1_get_data_device(_manager, seat.data()));
}
