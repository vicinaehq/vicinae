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

class ExtClipman : public WaylandDisplay, public WaylandRegistry::Listener, public ExtDataDevice::Listener {

public:
  static ExtClipman *instance();
  void start();
  ExtClipman();

private:
  std::unique_ptr<WaylandRegistry> _registry;
  std::unique_ptr<ExtDataControlManager> _dcm;
  std::unique_ptr<WaylandSeat> _seat;

  void global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) override;
  void selection(ExtDataDevice &device, ExtDataOffer &offer) override;
  void primarySelection(ExtDataDevice &device, ExtDataOffer &offer) override;
};
