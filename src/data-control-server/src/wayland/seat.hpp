#pragma once
#include <wayland-client.h>

class WaylandSeat {
  wl_seat *_seat;

public:
  wl_seat *data() const;
  WaylandSeat(wl_seat *seat);
  ~WaylandSeat();
};
