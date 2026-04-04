#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace clipboard_proto {

struct Offer {
  std::string mime_type;
  std::vector<uint8_t> data;
};

struct Selection {
  std::vector<Offer> offers;
};

} // namespace clipboard_proto
