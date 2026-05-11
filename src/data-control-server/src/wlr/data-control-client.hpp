#pragma once
#include "wayland/seat.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "data-device.hpp"

class WlrDataControlManager {
public:
  WlrDataControlManager(zwlr_data_control_manager_v1 *manager);

  std::unique_ptr<WlrDataDevice> getDataDevice(const WaylandSeat &seat);
  zwlr_data_control_manager_v1 *raw() const { return _manager; }

private:
  zwlr_data_control_manager_v1 *_manager;
};
