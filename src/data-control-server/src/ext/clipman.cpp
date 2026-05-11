#include <cerrno>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <ranges>
#include <poll.h>
#include <unistd.h>
#include "clipman.hpp"
#include "clipboard-writer.hpp"
#include "stdin-reader.hpp"
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

void ExtClipman::primarySelection(ExtDataDevice &, ExtDataOffer &offer) {
  if (isatty(STDOUT_FILENO)) {
    Selection::printPrimarySelectionDebug(offer);
    return;
  }
}

void ExtClipman::selection(ExtDataDevice &, ExtDataOffer &offer) {
  if (isatty(STDOUT_FILENO)) {
    Selection::printDebug(Selection::filterMimes(offer.mimes()), offer, "SELECTION");
    return;
  }

  auto &mimes = offer.mimes();
  if (std::ranges::find(mimes, "vicinae/concealed") != mimes.end()) return;

  auto selection = Selection::buildSelection(Selection::filterMimes(mimes), offer);
  m_writer(selection);
}

void ExtClipman::setClipboard(const clipboard_proto::Selection &selection) {
  ClipboardWriter::setSelectionExt(_dcm->raw(), m_device->raw(), selection);
  flush();
}

void ExtClipman::start() {
  roundtrip();

  if (!_dcm) { throw std::runtime_error("ext data control is not available"); }
  if (!_seat) { throw std::runtime_error("seat is not available"); }

  m_device = _dcm->getDataDevice(*_seat);
  m_device->registerListener(this);

  StdinReader stdinReader([this](clipboard_proto::Selection sel) { setClipboard(sel); });

  pollfd fds[2];
  fds[0] = {.fd = fd(), .events = POLLIN, .revents = 0};
  fds[1] = {.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};

  for (;;) {
    while (wl_display_prepare_read(display()) != 0) {
      wl_display_dispatch_pending(display());
    }
    flush();

    fds[0].revents = 0;
    fds[1].revents = 0;

    int ret = poll(fds, 2, -1);
    if (ret < 0) {
      wl_display_cancel_read(display());
      if (errno == EINTR) continue;
      std::cerr << "poll() failed: " << strerror(errno) << '\n';
      break;
    }

    if (fds[0].revents & POLLIN) {
      wl_display_read_events(display());
      wl_display_dispatch_pending(display());
    } else {
      wl_display_cancel_read(display());
    }

    if (fds[1].revents & POLLIN) {
      if (!stdinReader.readAndProcess()) break;
    }

    if (fds[1].revents & (POLLHUP | POLLERR)) break;
  }
}

ExtClipman *ExtClipman::instance(SelectionWriter writer) {
  static ExtClipman app{writer};
  return &app;
}

ExtClipman::ExtClipman(SelectionWriter writer) : m_writer(writer), _dcm(nullptr), _seat(nullptr) {
  _registry = registry();
  _registry->addListener(this);
}
