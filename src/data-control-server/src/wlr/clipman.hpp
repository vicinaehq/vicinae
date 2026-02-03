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
#include "wayland/display.hpp"

class WlrClipman : public WaylandDisplay, public WaylandRegistry::Listener, public WlrDataDevice::Listener {

public:
  static WlrClipman *instance();
  void start();
  WlrClipman();

private:
  std::unique_ptr<WaylandRegistry> _registry;
  std::unique_ptr<WlrDataControlManager> _dcm;
  std::unique_ptr<WaylandSeat> _seat;

  void global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) override;
  void selection(WlrDataDevice &device, WlrDataOffer &offer) override;
  void primarySelection(WlrDataDevice &device, WlrDataOffer &offer) override;
};
