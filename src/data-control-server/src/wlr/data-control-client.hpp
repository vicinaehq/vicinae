#pragma once
#include "wayland/seat.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "data-device.hpp"

class WlrDataControlManager {
public:
private:
  zwlr_data_control_manager_v1 *_manager;

public:
  WlrDataControlManager(zwlr_data_control_manager_v1 *manager);

  std::unique_ptr<WlrDataDevice> getDataDevice(const WaylandSeat &seat);
};
