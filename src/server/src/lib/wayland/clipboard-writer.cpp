#include "clipboard-writer.hpp"
#include "globals.hpp"
#include <QDebug>
#include <unistd.h>

namespace Wayland {

static const ext_data_control_source_v1_listener EXT_SOURCE_LISTENER = {
    .send = ClipboardWriter::extSend,
    .cancelled = ClipboardWriter::extCancelled,
};

static const ext_data_control_device_v1_listener EXT_DEVICE_LISTENER = {
    .data_offer = [](void *, ext_data_control_device_v1 *, ext_data_control_offer_v1 *) {},
    .selection = [](void *, ext_data_control_device_v1 *, ext_data_control_offer_v1 *) {},
    .finished = [](void *, ext_data_control_device_v1 *) {},
    .primary_selection = [](void *, ext_data_control_device_v1 *, ext_data_control_offer_v1 *) {},
};

static const zwlr_data_control_source_v1_listener WLR_SOURCE_LISTENER = {
    .send = ClipboardWriter::wlrSend,
    .cancelled = ClipboardWriter::wlrCancelled,
};

static const zwlr_data_control_device_v1_listener WLR_DEVICE_LISTENER = {
    .data_offer = [](void *, zwlr_data_control_device_v1 *, zwlr_data_control_offer_v1 *) {},
    .selection = [](void *, zwlr_data_control_device_v1 *, zwlr_data_control_offer_v1 *) {},
    .finished = [](void *, zwlr_data_control_device_v1 *) {},
    .primary_selection = [](void *, zwlr_data_control_device_v1 *, zwlr_data_control_offer_v1 *) {},
};

bool ClipboardWriter::isAvailable() {
  return Globals::seat() && (Globals::dataControlDeviceManager() || Globals::wlrDataControlManager());
}

bool ClipboardWriter::write(const QMimeData *data) {
  auto *seat = Globals::seat();
  if (!seat) return false;

  auto *active = new ActiveSource;
  for (const auto &format : data->formats()) {
    active->offers[format.toStdString()] = data->data(format);
  }

  if (auto *manager = Globals::dataControlDeviceManager()) {
    static auto *device = [&]() {
      auto *dev = ext_data_control_manager_v1_get_data_device(manager, seat);
      ext_data_control_device_v1_add_listener(dev, &EXT_DEVICE_LISTENER, nullptr);
      return dev;
    }();

    auto *source = ext_data_control_manager_v1_create_data_source(manager);
    if (!source) {
      delete active;
      return false;
    }

    ext_data_control_source_v1_add_listener(source, &EXT_SOURCE_LISTENER, active);

    for (const auto &[mime, _] : active->offers) {
      ext_data_control_source_v1_offer(source, mime.c_str());
    }

    ext_data_control_device_v1_set_selection(device, source);
    return true;
  }

  if (auto *manager = Globals::wlrDataControlManager()) {
    static auto *device = [&]() {
      auto *dev = zwlr_data_control_manager_v1_get_data_device(manager, seat);
      zwlr_data_control_device_v1_add_listener(dev, &WLR_DEVICE_LISTENER, nullptr);
      return dev;
    }();

    auto *source = zwlr_data_control_manager_v1_create_data_source(manager);
    if (!source) {
      delete active;
      return false;
    }

    zwlr_data_control_source_v1_add_listener(source, &WLR_SOURCE_LISTENER, active);

    for (const auto &[mime, _] : active->offers) {
      zwlr_data_control_source_v1_offer(source, mime.c_str());
    }

    zwlr_data_control_device_v1_set_selection(device, source);
    return true;
  }

  delete active;
  return false;
}

void ClipboardWriter::writeToFd(ActiveSource *active, const char *mimeType, int32_t fd) {
  if (auto it = active->offers.find(mimeType); it != active->offers.end()) {
    const auto &buf = it->second;
    ssize_t written = 0;
    while (written < buf.size()) {
      ssize_t n = ::write(fd, buf.data() + written, buf.size() - written);
      if (n <= 0) break;
      written += n;
    }
  }
  close(fd);
}

void ClipboardWriter::extSend(void *data, ext_data_control_source_v1 *, const char *mimeType, int32_t fd) {
  writeToFd(static_cast<ActiveSource *>(data), mimeType, fd);
}

void ClipboardWriter::extCancelled(void *data, ext_data_control_source_v1 *source) {
  delete static_cast<ActiveSource *>(data);
  ext_data_control_source_v1_destroy(source);
}

void ClipboardWriter::wlrSend(void *data, zwlr_data_control_source_v1 *, const char *mimeType, int32_t fd) {
  writeToFd(static_cast<ActiveSource *>(data), mimeType, fd);
}

void ClipboardWriter::wlrCancelled(void *data, zwlr_data_control_source_v1 *source) {
  delete static_cast<ActiveSource *>(data);
  zwlr_data_control_source_v1_destroy(source);
}

} // namespace Wayland
