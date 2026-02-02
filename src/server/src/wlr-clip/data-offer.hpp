#pragma once
#include "lib/wayland/display.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include <vector>
#include <string>

class DataOffer {
  zwlr_data_control_offer_v1 *_offer;
  std::vector<std::string> _mimes;
  char _buf[1 << 16];

public:
  static void offer(void *data, zwlr_data_control_offer_v1 *offer, const char *mime);

  constexpr static struct zwlr_data_control_offer_v1_listener _listener = {.offer = offer};

  /**
   * Receives the data associated with the specified mime type.
   * The passing of the display is required to properly dispatch the receive request.
   * Instead of the raw data itself, a path to a temporary file is returned.
   * The caller is responsible for the cleaning of this file after they are done
   * processing it.
   */
  std::string receive(const std::string &mime);
  const std::vector<std::string> &mimes() const;
  zwlr_data_control_offer_v1 *pointer() const { return _offer; }

  DataOffer(zwlr_data_control_offer_v1 *offer);
  ~DataOffer();
};
