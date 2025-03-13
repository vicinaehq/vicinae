#pragma once

#include "display.hpp"
#include "seat.hpp"
#include "wayland-wlr-data-control-client-protocol.h"
#include <memory>
#include <string>
#include <filesystem>
#include <vector>

class DataControlManager {
public:
  class DataDevice {
  public:
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
      std::filesystem::path receive(const WaylandDisplay &display, const std::string &mime);
      const std::vector<std::string> &mimes() const;

      DataOffer(zwlr_data_control_offer_v1 *offer);
      ~DataOffer();
    };

    class Listener {
    public:
      virtual void dataOffer(DataDevice &device, DataOffer &offer) {}
      virtual void selection(DataDevice &device, DataOffer &offer) {}
      virtual void finished(DataDevice &device) {}
      virtual void primarySelection(DataDevice &device, DataOffer &offer) {}
    };

  private:
    zwlr_data_control_device_v1 *_dev;
    std::vector<Listener *> _listeners;
    std::unique_ptr<DataOffer> _offer;

    static void dataOffer(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id);
    static void selection(void *data, zwlr_data_control_device_v1 *device, zwlr_data_control_offer_v1 *id);
    static void finished(void *data, zwlr_data_control_device_v1 *device);
    static void primarySelection(void *data, zwlr_data_control_device_v1 *device,
                                 zwlr_data_control_offer_v1 *id);

    constexpr static const struct zwlr_data_control_device_v1_listener _listener = {.data_offer = dataOffer,
                                                                                    .selection = selection,
                                                                                    .finished = finished,
                                                                                    .primary_selection =
                                                                                        primarySelection};

  public:
    void registerListener(Listener *listener) { _listeners.push_back(listener); }

    DataDevice(zwlr_data_control_device_v1 *dev);
    ~DataDevice();
  };

private:
  zwlr_data_control_manager_v1 *_manager;

public:
  DataControlManager(zwlr_data_control_manager_v1 *manager);

  std::unique_ptr<DataDevice> getDataDevice(const WaylandSeat &seat);
};
