#pragma once
#include <cstdint>
#include <netinet/in.h>
#include <memory>
#include <sys/syscall.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include "data-control-client.hpp"
#include "lib/wayland/display.hpp"

class Clipman : public WaylandDisplay, public WaylandRegistry::Listener, public DataDevice::Listener {

public:
  static Clipman *instance();
  void start();
  Clipman();

private:
  std::unique_ptr<WaylandRegistry> _registry;
  std::unique_ptr<DataControlManager> _dcm;
  std::unique_ptr<WaylandSeat> _seat;

  void global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) override;
  void selection(DataDevice &device, DataOffer &offer) override;
  void primarySelection(DataDevice &device, DataOffer &offer) override;
};
