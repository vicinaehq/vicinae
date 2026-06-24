#include "seat.hpp"

wl_seat *WaylandSeat::data() const { return _seat; }

WaylandSeat::WaylandSeat(wl_seat *seat) : _seat(seat) {}

WaylandSeat::~WaylandSeat() {
  if (_seat) { wl_seat_destroy(_seat); }
}
