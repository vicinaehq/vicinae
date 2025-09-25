#include "app.hpp"
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include "proto/wlr-clipboard.pb.h"

void Clipman::global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) {
  if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
    auto manager = reg.bind<zwlr_data_control_manager_v1>(name, &zwlr_data_control_manager_v1_interface,
                                                          std::min(version, 1u));
    _dcm = std::make_unique<DataControlManager>(manager);
  }

  if (strcmp(interface, wl_seat_interface.name) == 0) {
    _seat = std::make_unique<WaylandSeat>(reg.bind<wl_seat>(name, &wl_seat_interface, std::min(version, 1u)));
  }
}

void Clipman::primarySelection(DataDevice &device, DataOffer &offer) {
  if (isatty(STDOUT_FILENO)) {
    std::cout << "********** " << "BEGIN PRIMARY SELECTION" << "**********" << std::endl;
    for (const auto &mime : offer.mimes()) {
      auto path = offer.receive(mime);
      std::cout << std::left << std::setw(30) << mime << path << std::endl;
    }
    std::cout << "********** " << "END PRIMARY SELECTION" << "**********" << std::endl;

    return;
  }

  // we don't do anything with the primary selection
}

void Clipman::selection(DataDevice &device, DataOffer &offer) {
  if (isatty(STDOUT_FILENO)) {
    std::cout << "********** " << "BEGIN SELECTION" << "**********" << std::endl;
    for (const auto &mime : offer.mimes()) {
      auto path = offer.receive(mime);
      std::cout << std::left << std::setw(30) << mime << path << std::endl;
    }
    std::cout << "********** " << "END SELECTION" << "**********" << std::endl;

    return;
  }

  proto::ext::wlrclip::Selection selection;

  for (const auto &mime : offer.mimes()) {
    auto dataOffer = selection.add_offers();

    dataOffer->set_data(offer.receive(mime));
    dataOffer->set_mime_type(mime);
  }

  std::string data;

  selection.SerializeToString(&data);

  uint32_t size = htonl(data.size());

  std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
  std::cout.write(data.data(), data.size());
  std::cout.flush();
}

void Clipman::start() {
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

Clipman *Clipman::instance() {
  static Clipman app;

  return &app;
}

Clipman::Clipman() : _dcm(nullptr), _seat(nullptr) {
  _registry = registry();
  _registry->addListener(this);
}
