#include <iostream>
#include <iomanip>
#include <ranges>
#include <string>
#include <unistd.h>
#include "clipman.hpp"
#include "selection.hpp"

void WlrClipman::global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) {
  if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
    auto manager = reg.bind<zwlr_data_control_manager_v1>(name, &zwlr_data_control_manager_v1_interface,
                                                          std::min(version, 1u));
    _dcm = std::make_unique<WlrDataControlManager>(manager);
  }

  if (strcmp(interface, wl_seat_interface.name) == 0) {
    _seat = std::make_unique<WaylandSeat>(reg.bind<wl_seat>(name, &wl_seat_interface, std::min(version, 1u)));
  }
}

void WlrClipman::primarySelection(WlrDataDevice &device, WlrDataOffer &offer) {
  if (isatty(STDOUT_FILENO)) {
    Selection::printPrimarySelectionDebug(offer);
    return;
  }

  // we don't do anything with the primary selection
}

void WlrClipman::selection(WlrDataDevice &device, WlrDataOffer &offer) {
  auto filteredMimes = Selection::filterMimes(offer.mimes());
  Selection::serializeAndWrite(filteredMimes, offer);
}

void WlrClipman::start() {
  roundtrip();

  if (!_dcm) { throw std::runtime_error("zwlr data control is not available"); }
  if (!_seat) { throw std::runtime_error("seat is not available"); }

  auto dev = _dcm->getDataDevice(*_seat.get());
  dev->registerListener(this);

  for (;;) {
    try {
      if (dispatch() == -1) { exit(1); }
    } catch (const std::exception &e) { std::cerr << "Uncaught exception: " << e.what() << std::endl; }
  }
}

WlrClipman *WlrClipman::instance() {
  static WlrClipman app;
  return &app;
}

WlrClipman::WlrClipman() : _dcm(nullptr), _seat(nullptr) {
  _registry = registry();
  _registry->addListener(this);
}
