#include "clipman.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <ranges>
#include <unistd.h>
#include "selection.hpp"

void ExtClipman::global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) {
  if (strcmp(interface, ext_data_control_manager_v1_interface.name) == 0) {
    auto manager = reg.bind<ext_data_control_manager_v1>(name, &ext_data_control_manager_v1_interface,
                                                         std::min(version, 1u));
    _dcm = std::make_unique<ExtDataControlManager>(manager);
  }

  if (strcmp(interface, wl_seat_interface.name) == 0) {
    _seat = std::make_unique<WaylandSeat>(reg.bind<wl_seat>(name, &wl_seat_interface, std::min(version, 1u)));
  }
}

void ExtClipman::primarySelection(ExtDataDevice &device, ExtDataOffer &offer) {
  if (isatty(STDOUT_FILENO)) {
    Selection::printPrimarySelectionDebug(offer);
    return;
  }

  // we don't do anything with the primary selection
}

void ExtClipman::selection(ExtDataDevice &device, ExtDataOffer &offer) {
  auto filteredMimes = Selection::filterMimes(offer.mimes());
  Selection::serializeAndWrite(filteredMimes, offer);
}

void ExtClipman::start() {
  roundtrip();

  if (!_dcm) { throw std::runtime_error("ext data control is not available"); }
  if (!_seat) { throw std::runtime_error("seat is not available"); }

  auto dev = _dcm->getDataDevice(*_seat.get());
  dev->registerListener(this);

  for (;;) {
    try {
      if (dispatch() == -1) { exit(1); }
    } catch (const std::exception &e) { std::cerr << "Uncaught exception: " << e.what() << std::endl; }
  }
}

ExtClipman *ExtClipman::instance() {
  static ExtClipman app;

  return &app;
}

ExtClipman::ExtClipman() : _dcm(nullptr), _seat(nullptr) {
  _registry = registry();
  _registry->addListener(this);
}
