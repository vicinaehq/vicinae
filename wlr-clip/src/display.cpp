#include "display.hpp"

WaylandDisplay::WaylandDisplay() { _display = wl_display_connect(nullptr); }
WaylandDisplay::~WaylandDisplay() {
  if (_display) { wl_display_disconnect(_display); }
};

std::unique_ptr<WaylandRegistry> WaylandDisplay::registry() const {
  return std::make_unique<WaylandRegistry>(wl_display_get_registry(_display));
}
int WaylandDisplay::dispatch() const { return wl_display_dispatch(_display); };
wl_display *WaylandDisplay::display() const { return _display; }
int WaylandDisplay::roundtrip() const { return wl_display_roundtrip(_display); };
int WaylandDisplay::flush() const { return wl_display_flush(_display); }
