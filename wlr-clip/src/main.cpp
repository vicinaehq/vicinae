#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <ranges>
#include <iostream>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <sys/syscall.h>
#include <unistd.h>
#include <vector>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include "data-control-client.hpp"
#include "display.hpp"
#include "proto.hpp"
#include "wayland-wlr-data-control-client-protocol.h"

constexpr const char *CONCEALED_MIME = "omnicast/concealed";

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
    } else {
      Proto::Array args{"selection"};
      Proto::Marshaler marshaler;

      auto transformOffer = [&](const std::string &mime) -> Proto::Dict {
        Proto::Dict offerData;

        offerData["file_path"] = offer.receive(*this, mime).string();
        offerData["mime_type"] = mime;

        return offerData;
      };

      auto offers = offer.mimes() |
                    std::views::filter([](const auto &mime) { return mime != CONCEALED_MIME; }) |
                    std::views::transform(transformOffer) | std::ranges::to<Proto::Array>();

      args.push_back(offers);
      auto message = marshaler.marshalSized(args);

      write(STDOUT_FILENO, message.data(), message.size());
    }
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

int main() {
  Clipman clipman;

  if (isatty(STDIN_FILENO)) {
    std::cerr << "omni-wlr-clip started directly from TTY. Start interacting with the clipboard and "
                 "selection events will show up here"
              << std::endl;
  }

  clipman.start();
}
