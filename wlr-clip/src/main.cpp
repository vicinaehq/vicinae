#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <netinet/in.h>
#include <iostream>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <sys/syscall.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include "data-control-client.hpp"
#include "display.hpp"
#include "wayland-wlr-data-control-client-protocol.h"
#include "proto/wlr-clipboard.pb.h"

class Clipman : public WaylandDisplay,
                public WaylandRegistry::Listener,
                public DataControlManager::DataDevice::Listener {
  std::unique_ptr<WaylandRegistry> _registry;
  std::unique_ptr<DataControlManager> _dcm;
  std::unique_ptr<WaylandSeat> _seat;

  void global(WaylandRegistry &reg, uint32_t name, const char *interface, uint32_t version) override {
    if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
      auto manager = reg.bind<zwlr_data_control_manager_v1>(name, &zwlr_data_control_manager_v1_interface,
                                                            std::min(version, 1u));
      _dcm = std::make_unique<DataControlManager>(manager);
    }

    if (strcmp(interface, wl_seat_interface.name) == 0) {
      _seat =
          std::make_unique<WaylandSeat>(reg.bind<wl_seat>(name, &wl_seat_interface, std::min(version, 1u)));
    }
  }

  void selection(DataControlManager::DataDevice &device,
                 DataControlManager::DataDevice::DataOffer &offer) override {
    if (isatty(STDOUT_FILENO)) {
      std::cout << "********** " << "BEGIN SELECTION" << "**********" << std::endl;
      for (const auto &mime : offer.mimes()) {
        auto path = offer.receive(*this, mime);
        std::cout << std::left << std::setw(30) << mime << path << std::endl;
      }
      std::cout << "********** " << "END SELECTION" << "**********" << std::endl;

      return;
    }

    proto::ext::wlrclip::Selection selection;

    for (const auto &mime : offer.mimes()) {
      auto dataOffer = selection.add_offers();

      dataOffer->set_data(offer.receive(*this, mime));
      dataOffer->set_mime_type(mime);
    }

    std::string data;

    selection.SerializeToString(&data);

    uint32_t size = htonl(data.size());

    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(data.data(), data.size());
    std::cout.flush();
  }

public:
  Clipman() : _dcm(nullptr), _seat(nullptr) {
    _registry = registry();
    _registry->addListener(this);
  }

  void start() {
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
};

int main(int ac, char **av) {
  Clipman clipman;

  if (isatty(STDIN_FILENO)) {
    std::cerr << av[0]
              << " started directly from TTY. Start interacting with the clipboard and "
                 "selection events will show up here"
              << std::endl;
  }

  clipman.start();
}
