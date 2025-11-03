#pragma once
#include "lib/wayland/seat.hpp"
#include "ext-data-control-v1-client-protocol.h"
#include "data-device.hpp"

class ExtDataControlManager {
public:
private:
  ext_data_control_manager_v1 *_manager;

public:
  ExtDataControlManager(ext_data_control_manager_v1 *manager);

  std::unique_ptr<ExtDataDevice> getDataDevice(const WaylandSeat &seat);
};
