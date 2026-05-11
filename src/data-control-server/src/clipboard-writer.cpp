#include <iostream>
#include <unistd.h>
#include "clipboard-writer.hpp"

static void writeToFd(ActiveSource *active, const char *mimeType, int32_t fd) {
  if (auto it = active->offers.find(mimeType); it != active->offers.end()) {
    const auto &buf = it->second;
    size_t written = 0;
    while (written < buf.size()) {
      ssize_t n = ::write(fd, buf.data() + written, buf.size() - written);
      if (n < 0) {
        if (errno == EINTR) continue;
        break;
      }
      written += n;
    }
  }
  close(fd);
}

static ActiveSource *buildActiveSource(const clipboard_proto::Selection &selection) {
  auto *active = new ActiveSource;
  for (const auto &offer : selection.offers) {
    active->offers[offer.mime_type] = offer.data;
  }
  return active;
}

// --- ext-data-control-v1 ---

static void extSend(void *data, ext_data_control_source_v1 *, const char *mimeType, int32_t fd) {
  writeToFd(static_cast<ActiveSource *>(data), mimeType, fd);
}

static void extCancelled(void *data, ext_data_control_source_v1 *source) {
  delete static_cast<ActiveSource *>(data);
  ext_data_control_source_v1_destroy(source);
}

static const ext_data_control_source_v1_listener EXT_SOURCE_LISTENER = {
    .send = extSend,
    .cancelled = extCancelled,
};

void ClipboardWriter::setSelectionExt(ext_data_control_manager_v1 *manager,
                                      ext_data_control_device_v1 *device,
                                      const clipboard_proto::Selection &selection) {
  auto *source = ext_data_control_manager_v1_create_data_source(manager);
  if (!source) return;

  auto *active = buildActiveSource(selection);
  ext_data_control_source_v1_add_listener(source, &EXT_SOURCE_LISTENER, active);

  for (const auto &[mime, _] : active->offers) {
    ext_data_control_source_v1_offer(source, mime.c_str());
  }

  ext_data_control_device_v1_set_selection(device, source);
  std::cerr << "Clipboard set via ext-data-control (" << active->offers.size() << " offers)\n";
}

// --- wlr-data-control-v1 ---

static void wlrSend(void *data, zwlr_data_control_source_v1 *, const char *mimeType, int32_t fd) {
  writeToFd(static_cast<ActiveSource *>(data), mimeType, fd);
}

static void wlrCancelled(void *data, zwlr_data_control_source_v1 *source) {
  delete static_cast<ActiveSource *>(data);
  zwlr_data_control_source_v1_destroy(source);
}

static const zwlr_data_control_source_v1_listener WLR_SOURCE_LISTENER = {
    .send = wlrSend,
    .cancelled = wlrCancelled,
};

void ClipboardWriter::setSelectionWlr(zwlr_data_control_manager_v1 *manager,
                                      zwlr_data_control_device_v1 *device,
                                      const clipboard_proto::Selection &selection) {
  auto *source = zwlr_data_control_manager_v1_create_data_source(manager);
  if (!source) return;

  auto *active = buildActiveSource(selection);
  zwlr_data_control_source_v1_add_listener(source, &WLR_SOURCE_LISTENER, active);

  for (const auto &[mime, _] : active->offers) {
    zwlr_data_control_source_v1_offer(source, mime.c_str());
  }

  zwlr_data_control_device_v1_set_selection(device, source);
  std::cerr << "Clipboard set via wlr-data-control (" << active->offers.size() << " offers)\n";
}
