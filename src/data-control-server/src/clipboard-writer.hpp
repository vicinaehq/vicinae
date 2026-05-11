#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "ext-data-control-v1-client-protocol.h"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "clipboard-protocol.hpp"

struct ActiveSource {
  std::unordered_map<std::string, std::vector<uint8_t>> offers;
};

namespace ClipboardWriter {

void setSelectionExt(ext_data_control_manager_v1 *manager, ext_data_control_device_v1 *device,
                     const clipboard_proto::Selection &selection);
void setSelectionWlr(zwlr_data_control_manager_v1 *manager, zwlr_data_control_device_v1 *device,
                     const clipboard_proto::Selection &selection);

} // namespace ClipboardWriter
