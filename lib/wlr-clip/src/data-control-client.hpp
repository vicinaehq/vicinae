#pragma once
#include "seat.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "data-device.hpp"

class DataControlManager {
public:
private:
  zwlr_data_control_manager_v1 *_manager;

public:
  DataControlManager(zwlr_data_control_manager_v1 *manager);

  std::unique_ptr<DataDevice> getDataDevice(const WaylandSeat &seat);
};
