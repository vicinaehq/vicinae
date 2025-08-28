#pragma once
#include <memory>
#include <wayland-client-core.h>
#include <wayland-client.h>
#include "registry.hpp"

class WaylandDisplay {
  wl_display *_display;

public:
  WaylandDisplay();
  ~WaylandDisplay();

  std::unique_ptr<WaylandRegistry> registry() const;
  int dispatch() const;
  int roundtrip() const;
  wl_display *display() const;
  int flush() const;
};
