#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace clipboard_proto {

enum class Command : uint8_t {
  SelectionNotification = 0x01,
  SetClipboard = 0x02,
};

struct Offer {
  std::string mime_type;
  std::vector<uint8_t> data;
};

struct Selection {
  std::vector<Offer> offers;
};

} // namespace clipboard_proto
