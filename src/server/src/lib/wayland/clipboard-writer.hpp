#pragma once
#include "ext-data-control-v1-client-protocol.h"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include <QMimeData>
#include <unordered_map>
#include <string>

namespace Wayland {

class ClipboardWriter {
public:
  static bool isAvailable();
  static bool write(const QMimeData *data);

  struct ActiveSource {
    std::unordered_map<std::string, QByteArray> offers;
  };

  static void extSend(void *data, ext_data_control_source_v1 *source, const char *mimeType, int32_t fd);
  static void extCancelled(void *data, ext_data_control_source_v1 *source);
  static void wlrSend(void *data, zwlr_data_control_source_v1 *source, const char *mimeType, int32_t fd);
  static void wlrCancelled(void *data, zwlr_data_control_source_v1 *source);

private:
  static void writeToFd(ActiveSource *active, const char *mimeType, int32_t fd);
};

} // namespace Wayland
